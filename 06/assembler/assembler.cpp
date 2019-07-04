#include "modules.h"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        cerr << "Wrong number of arguments. There should two arguments as the names of the source file and the dest file.\n";
        return -1;
    }
    string sourceFile = argv[1];
    string destFile = argv[2];
    Parser P(sourceFile);
    Code C;
    ofstream FO(destFile);
    string binInstr;
    while (P.hasMoreCommands())
    {
        P.advance();
        if (P.commandType == C_COMMAND)
        {
            string compCode = C.comp(P.comp());
            string destCode = C.dest(P.dest());
            string jumpCode = C.jump(P.jump());
            binInstr = "111" + compCode + destCode + jumpCode;
        }
        else if (P.commandType == A_COMMAND)
        {
            binInstr = "0" + DecimalToBits(P.symbol);
        }
        FO << binInstr << endl;
    }
    FO.close();
    return 0;
}
