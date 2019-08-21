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

static void removeBlankAndComment(string &line)
{
    auto it = line.begin();
    while (it != line.end())
    {
        if (*it == '/' && *(it+1) == '/')
        {
            line.erase(it - line.begin());
            return;
        }
        if (isspace(*it))
        {
            line.erase(it);
            continue;
        }
        it++;
    }
}

void Parser::advance()
{
    if (hasMoreCommands())
    {
        getline(prog, currentInstr);
        
        if (currentInstr == "") advance();
    }
}

