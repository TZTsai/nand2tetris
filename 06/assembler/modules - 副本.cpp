#include "modules.h"

string DecimalToBits(int n)
{
    assert(0 <= n < 32768);
    string b;
    for (int i = 0; i < 15; i++)
    {
        b = to_string(n % 2) + b;
        n /= 2;
    }
    return b;
}

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
        getline(prog, currentInstr);
        removeComment(currentInstr);
        if (currentInstr == "") advance();
        parse();
    }
}

static bool isSymbol(string str)
{
    for (auto it = str.begin(); it != str.end(); it++)
    {
        if (isalnum(*it) || LEGAL_PUNCTUATIONS.find(*it) != string::npos)
            continue;
        return false;
    }
    return !str.empty();
}

bool isNumeral(string s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

void Parser::parse()
{
    char front = currentInstr[0];
    if (front == '@')
        type = A_COMMAND;
    else if (front == '(')
        type = L_COMMAND;
    else
        type = C_COMMAND;

    if (type != C_COMMAND)
    // parse A_COMMAND or L_COMMAND
    {
        string str = currentInstr.substr(1);
        if (front == '(') str.pop_back();
        if (!isSymbol(str)) throw SyntaxError();
        symbol_str = str;
    }
    else
    // parse C_COMMAND
    {
        int equalPos = currentInstr.find('=');
        int semicolonPos = currentInstr.find(';');
        size_t compPos = 0, compLen = string::npos;
        if (equalPos != string::npos)
        {
            dest_str = currentInstr.substr(0, equalPos);
            compPos = equalPos + 1;
        }
        else dest_str = "null";
        if (semicolonPos != string::npos)
        {
            jump_str = currentInstr.substr(semicolonPos + 1);
            compLen = semicolonPos - equalPos - 1;
        }
        else jump_str = "null";
        comp_str = currentInstr.substr(compPos, compLen);
    }
}

command_t Parser::commandType()
{
    return type;
}

string Parser::symbol()
{
    return symbol_str;
}

string Parser::dest()
{
    return dest_str;
}

string Parser::comp()
{
    return comp_str;
}

string Parser::jump()
{
    return jump_str;
}

Code::Code()
{
  destDict.insert(pair<string, bits>("null", "000"));
  destDict.insert(pair<string, bits>("M", "001"));
  destDict.insert(pair<string, bits>("D", "010"));
  destDict.insert(pair<string, bits>("MD", "011"));
  destDict.insert(pair<string, bits>("A", "100"));
  destDict.insert(pair<string, bits>("AM", "101"));
  destDict.insert(pair<string, bits>("AD", "110"));
  destDict.insert(pair<string, bits>("AMD", "111"));
  compDict.insert(pair<string, bits>("0", "0101010"));
  compDict.insert(pair<string, bits>("1", "0111111"));
  compDict.insert(pair<string, bits>("-1", "0111010"));
  compDict.insert(pair<string, bits>("D", "0001100"));
  compDict.insert(pair<string, bits>("A", "0110000"));
  compDict.insert(pair<string, bits>("M", "1110000"));
  compDict.insert(pair<string, bits>("!D", "0001101"));
  compDict.insert(pair<string, bits>("!A", "0110001"));
  compDict.insert(pair<string, bits>("!M", "1110001"));
  compDict.insert(pair<string, bits>("-D", "0001111"));
  compDict.insert(pair<string, bits>("-A", "0110011"));
  compDict.insert(pair<string, bits>("-M", "1110011"));
  compDict.insert(pair<string, bits>("D+1", "0011111"));
  compDict.insert(pair<string, bits>("A+1", "0110111"));
  compDict.insert(pair<string, bits>("M+1", "1110111"));
  compDict.insert(pair<string, bits>("D-1", "0001110"));
  compDict.insert(pair<string, bits>("A-1", "0110010"));
  compDict.insert(pair<string, bits>("M-1", "1110010"));
  compDict.insert(pair<string, bits>("D+A", "0000010"));
  compDict.insert(pair<string, bits>("D+M", "1000010"));
  compDict.insert(pair<string, bits>("D-A", "0010011"));
  compDict.insert(pair<string, bits>("D-M", "1010011"));
  compDict.insert(pair<string, bits>("A-D", "0000111"));
  compDict.insert(pair<string, bits>("M-D", "1000111"));
  compDict.insert(pair<string, bits>("D&A", "0000000"));
  compDict.insert(pair<string, bits>("D&M", "1000000"));
  compDict.insert(pair<string, bits>("D|A", "0010101"));
  compDict.insert(pair<string, bits>("D|M", "1010101"));
  jumpDict.insert(pair<string, bits>("null", "000"));
  jumpDict.insert(pair<string, bits>("JGT", "001"));
  jumpDict.insert(pair<string, bits>("JEQ", "010"));
  jumpDict.insert(pair<string, bits>("JGE", "011"));
  jumpDict.insert(pair<string, bits>("JLT", "100"));
  jumpDict.insert(pair<string, bits>("JNE", "101"));
  jumpDict.insert(pair<string, bits>("JLE", "110"));
  jumpDict.insert(pair<string, bits>("JMP", "111"));
}

bits Code::dest(string mnemonic)
{
    map<string, bits>::iterator p = destDict.find(mnemonic);
    if (p == destDict.end()) throw SyntaxError();
    return p->second;
}

bits Code::comp(string mnemonic)
{
    map<string, bits>::iterator p = compDict.find(mnemonic);
    if (p == compDict.end()) throw SyntaxError();
    return p->second;
}

bits Code::jump(string mnemonic)
{
    map<string, bits>::iterator p = jumpDict.find(mnemonic);
    if (p == jumpDict.end()) throw SyntaxError();
    return p->second;
}

SymbolTable::SymbolTable()
{
    for (size_t i = 0; i < 16; i++)
    {
        ostringstream os;
        os << "R" << i;
        addEntry(os.str(), i);
    }
    addEntry("SP", 0);
    addEntry("LCL", 1);
    addEntry("ARG", 2);
    addEntry("THIS", 3);
    addEntry("THAT", 4);
    addEntry("SCREEN", 16384);
    addEntry("KBD", 24576);
}

void SymbolTable::addEntry(string symbol, int address)
{
    if (contains(symbol))
        cout << "Warning: rebinding symbol " << symbol << " as " << address << endl;
    symbolDict.insert(pair<string, int>(symbol, address));
}

bool SymbolTable::contains(string symbol)
{
    map<string, int>::iterator p = symbolDict.find(symbol);
    return p != symbolDict.end();
}

int SymbolTable::GetAddress(string symbol)
{
    map<string, int>::iterator p = symbolDict.find(symbol);
    if (p != symbolDict.end())
        return p->second;
    cerr << "Unbound symbol: " << symbol << ".\n";
    throw SyntaxError();
}