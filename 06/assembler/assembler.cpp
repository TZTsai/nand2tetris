#include "modules.h"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        cerr << "Wrong number of arguments. There should be two arguments as the names of the source file and the dest file.\n";
        return -1;
    }
    string sourceFile = argv[1];
    string destFile = argv[2];
    Parser P1(sourceFile);
    Parser P2(sourceFile);
    Code C;
    SymbolTable ST;
    ofstream FO(destFile);
    string binInstr;
    int ROMaddress = 0, RAMaddress = 16;

    // read for the first time
    while (P1.hasMoreCommands())
    {
        P1.advance();
        if (P1.commandType() == L_COMMAND)
        {
            string symbol = P1.symbol();
            ST.addEntry(symbol, ROMaddress);
        }
        else ROMaddress++;
    }

    // read for the second time
    while (P2.hasMoreCommands())
    {
        P2.advance();
        if (P2.commandType() == C_COMMAND)
        {
            string compCode = C.comp(P2.comp());
            string destCode = C.dest(P2.dest());
            string jumpCode = C.jump(P2.jump());
            binInstr = "111" + compCode + destCode + jumpCode;
            FO << binInstr << endl;
        }
        else if (P2.commandType() == A_COMMAND)
        {
            string symbol = P2.symbol();
            if (isNumeral(symbol))
                binInstr = "0" + DecimalToBits(stoi(symbol));
            else if (ST.contains(symbol))
                binInstr = "0" + DecimalToBits(ST.GetAddress(symbol));
            else
            {
                ST.addEntry(symbol, RAMaddress);
                binInstr = "0" + DecimalToBits(RAMaddress);
                RAMaddress++;
            }
            FO << binInstr << endl;
        }
    }
    FO.close();
    return 0;
}
