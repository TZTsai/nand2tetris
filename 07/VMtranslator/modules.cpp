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

void CodeWriter::writeArithmetic(string cmd)
{
    out << "@SP\n" << "A=M-1\n";
    if (cmd == "not" || cmd == "neg")
    {
        if (cmd == "not")
            out << "M=!M\n";
        else out << "M=-M\n";
    }
    else
    {
        out << "D=M\n";
        out << "@SP\n" << "M=M-1\n" << "A=M-1\n";
        if (cmd == "add" || cmd == "sub" 
        || cmd == "and" || cmd == "or")
        {
            if (cmd == "add")
                out << "M=D+M\n";
            else if (cmd == "sub")
                out << "M=M-D\n";
            else if (cmd == "and")
                out << "M=D&M\n";
            else out << "M=D|M";
        }
        else
        {
            out << "D=M-D\n" << "M=-1\n";  // first set M to be true
            out << "@TRUE\n" << "D;";  // if the comparison holds, jump to the end
            if (cmd == "gt")
                out << "JGT\n";
            else if (cmd == "eq")
                out << "JEQ\n";
            else out << "JLT\n";
            out << "@SP\n" << "A=M-1\n";
            out << "M=0\n" << "(TRUE)\n";
        }
    }
}

void CodeWriter::WritePushPop(command_t type, string segment, int index)
{
    if (segment == "constant")
    {
        if (type == C_PUSH)
        {
            out << "@" << index << endl;
            out << "D=A\n";
            out << "@SP\n" << "A=M\n";
            out << "M=D\n";
            out << "@SP\n" << "M=M+1\n";
        }
        else throw SyntaxError();
    }
    else
    {
        return;  // modify this later
    }
}