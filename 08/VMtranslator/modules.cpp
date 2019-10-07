#include "modules.h"

Parser::Parser(string fileName)
{
    prog.open(fileName);
    if (!prog)
    {
        cerr << "cannot open source file " << fileName << "!\n";
        throw OpenFileError();
    }
}

Parser::~Parser()
{
    prog.close();
}

bool Parser::hasMoreCommands()
{
    return prog.good() && prog.peek() != EOF;
}

static void removeComment(string &line)
{
    line = line.substr(0, line.find("//"));
    if (line.find_first_not_of(" \r") == string::npos)
        line = "";
}

void Parser::advance()
{
    if (hasMoreCommands())
    {
        getline(prog, currentLine);
        removeComment(currentLine);
        if (currentLine == "") advance();
        istringstream iss(currentLine);
        iss >> cmd >> arg_1 >> arg_2;
    }
}

command_t Parser::commandType()
{
    if (cmd == "add" || cmd == "sub" || cmd == "neg"
    || cmd == "eq" || cmd == "gt" || cmd == "lt" ||
    cmd == "and" || cmd == "or" || cmd == "not")
        return C_ARITHMETIC;
    if (cmd == "push")
        return C_PUSH;
    if (cmd == "pop")
        return C_POP;
    if (cmd == "label")
        return C_LABEL;
    if (cmd == "goto")
        return C_GOTO;
    if (cmd == "if-goto")
        return C_IF;
    if (cmd == "function")
        return C_FUNCTION;
    if (cmd == "call")
        return C_CALL;
    if (cmd == "return")
        return C_RETURN;
    throw SyntaxError();
}

string Parser::arg1()
{
    if (commandType() == C_ARITHMETIC)
        return cmd;
    else if (commandType() == C_RETURN)
        throw SyntaxError();
    else return arg_1;
}

int Parser::arg2()
{
    if (commandType() == C_PUSH || commandType() == C_POP
    || commandType() == C_FUNCTION || commandType() == C_CALL)
        return arg_2;
    else throw SyntaxError();
}

CodeWriter::CodeWriter(string asmFileName)
{
    label_num = 0;
    out.open(asmFileName);
    if (!out)
    {
        cerr << "cannot open source file " << asmFileName << "!\n";
        throw OpenFileError();
    }
}

CodeWriter::~CodeWriter()
{
    out.close();
}

void CodeWriter::setFileName(string fileName)
{
    vmFileName = fileName;
}

static string getAddressSymbol(const string &segment)
{
    if (segment == "local")
        return "LCL";
    if (segment == "argument")
        return "ARG";
    if (segment == "this")
        return "THIS";
    if (segment == "that")
        return "THAT";
    throw SyntaxError();
}

static void incSP(ostream &out)
{
    out << "@SP\n" << "M=M+1\n";
}

static void decSP(ostream &out)
{
    out << "@SP\n" << "M=M-1\n";
}

static void atStackTop(ostream &out)
{
    out << "@SP\n" << "A=M-1\n";
}

static void regNum(ostream &out, int n)
// store n in D
{
    out << "@" << n << endl;
    out << "D=A\n";
}

static void push(ostream &out)
// push [M] into the stack
// requires A registers the requested address
{
    out << "D=M\n";
    out << "@SP\n" << "A=M\n" << "M=D\n";
    incSP(out);
}

static void pop(ostream &out)
// pop the stack
// register the content at top into D and dec SP
{
    atStackTop(out);
    out << "D=M\n";
    decSP(out);
}

void CodeWriter::writeInit()
{
    out << "// init\n";
    regNum(out, 256);
    out << "@SP\n" << "M=D\n";
    writeCall("Sys.init", 0);
}

void CodeWriter::writeArithmetic(string cmd)
{
    out << "// " << cmd << endl;
    if (cmd == "not" || cmd == "neg")
    {
        atStackTop(out);
        if (cmd == "not")
            out << "M=!M\n";
        else out << "M=-M\n";
    }
    else
    {
        pop(out);
        atStackTop(out);
        if (cmd == "add" || cmd == "sub" 
        || cmd == "and" || cmd == "or")
        {
            if (cmd == "add")
                out << "M=D+M\n";
            else if (cmd == "sub")
                out << "M=M-D\n";
            else if (cmd == "and")
                out << "M=D&M\n";
            else out << "M=D|M\n";
        }
        else
        {
            out << "D=M-D\n" << "M=-1\n"  // first set M to be true
            << "@L" << label_num << "\nD;";  // if the comparison holds, jump to the end
            if (cmd == "gt")
                out << "JGT\n";
            else if (cmd == "eq")
                out << "JEQ\n";
            else if (cmd =="lt")
                out << "JLT\n";
            else throw SyntaxError();
            atStackTop(out);
            out << "M=0\n" << "(L" << label_num << ")\n";
            label_num++;
        }
    }
}

void CodeWriter::gotoAddress(const string &segment, int index)
{
    regNum(out, index);
    if (segment == "local" || segment == "argument" ||
        segment == "this" || segment == "that")
    {
        out << "@" << getAddressSymbol(segment) << endl;
        out << "A=D+M\n";  // address = base + index
    }
    else if (segment == "pointer" || segment == "temp") 
    {
        int base;
        if (segment == "pointer") base = 3;
        else base = 5;
        out << "@" << base << endl;
        out << "A=D+A\n";
    }
    else if (segment == "static")
    {
        out << "@" << vmFileName << "." << index << endl;
    }
    else throw SyntaxError();
}

void CodeWriter::writePushPop(command_t type, string segment, int index)
{
    if (segment == "constant")
    {
        if (type == C_PUSH)
        {
            out << "// push " << segment << " " << index << "\n";
            regNum(out, index);
            out << "@SP\n" << "A=M\n" << "M=D\n";
            incSP(out);
        }
        else throw SyntaxError();
    }
    else
    {
        if (type == C_PUSH)
        {
            out << "// push " << segment << " " << index << "\n";
            gotoAddress(segment, index);
            push(out);
        }
        else  // it will change R13
        {
            out << "// pop " << segment << " " << index << "\n";
            gotoAddress(segment, index);
            out << "D=A\n" << "@R13\n" << "M=D\n";
            pop(out);
            out << "@R13\n" << "A=M\n" << "M=D\n";
        }
    }
}

void CodeWriter::writeLabel(string label)
{
    out << "(" << label << ")\n";
}

void CodeWriter::writeGoto(string label)
{
    out << "// goto " << label << endl;
    out << "@" << label << endl;
    out << "0;JMP\n";
}

void CodeWriter::writeIf(string label)
{
    out << "// if-goto " << label << endl;
    atStackTop(out);
    out << "D=M\n";
    decSP(out);  // maybe if-goto need to decrease SP?
    out << "@" << label << endl;
    out << "D;JNE\n";  
}

void CodeWriter::writeCall(string functionName, int numArgs)
{
    // need to deal with the frame
    writeGoto(functionName);
}