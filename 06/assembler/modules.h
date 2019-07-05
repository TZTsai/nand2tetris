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

typedef string bits;  // define bis as a string of "0"s and "1"s

const string LEGAL_PUNCTUATIONS = "_.$:";

bool isNumeral(string s);

string DecimalToBits(int n);

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

class SymbolTable
{
    map<string, int> symbolDict;

public:
    SymbolTable();
    void addEntry(string symbol, int address);
    bool contains(string symbol);
    int GetAddress(string symbol);
};