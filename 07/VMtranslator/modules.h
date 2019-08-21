#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

using namespace std;

class OpenFileError {};

class SyntaxError {};

enum command_t
{
    C_ARITHMETIC, C_PUSH, C_POP, C_LABEL,
    C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL
};

class Parser
{
    ifstream prog;
    string currentInstr;
    command_t type;

public:
    Parser(string fileName);
    bool hasMoreCommands();
    void advance();
    command_t commandType();
    string arg1();
    int arg2();
    ~Parser();
};

