#include <modules.h>

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
    if (line.find_first_not_of(' ') == string::npos)
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


const string arithmeticCmd[] =
{"add", "sub", "neg"};

const string logicalCmd[] = 
{"eq", "gt", "lt", "and", "or", "not"};

command_t Parser::commandType()
{
    if (cmd == "add" || cmd == "sub" || cmd == "neg"
    || cmd == "eq" || cmd == "gt" || cmd == "lt" ||
    cmd == "and" || cmd == "or" || cmd == "not")
        return C_ARITHMETIC;
    
}