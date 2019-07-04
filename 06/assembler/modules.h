#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <map>

using namespace std;

class OpenFileError {};

class SyntaxError {};

enum command_t {A_COMMAND, C_COMMAND, L_COMMAND};

typedef string bits;

const string LEGAL_PUNCTUATIONS = "_.$:";

string DecimalToBits(string num);

class Parser
{
    ifstream prog;
    string currentInstr;
    command_t type;
    string symbol_str;
    string dest_str;
    string comp_str;
    string jump_str;

public:
    Parser(string fileName);
    bool hasMoreCommands();
    void advance();
    void parse();
    command_t commandType();
    string symbol();
    string dest();
    string comp();
    string jump();
    ~Parser();
};

class Code
{
    map<string, bits> destDict;
    map<string, bits> compDict;
    map<string, bits> jumpDict;

public:
    Code();
    bits dest(string mnemonic);  // return bits with len = 3
    bits comp(string mnemonic);  // return bits with len = 7
    bits jump(string mnemonic);  // return bits with len = 3
};
