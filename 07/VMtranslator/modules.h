#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
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
    string currentLine;
    command_t type;
    string cmd, arg_1;
    int arg_2;

public:
    Parser(string fileName);
    bool hasMoreCommands();
    void advance();
    command_t commandType();
    string arg1();
    int arg2();
    ~Parser();
};

