#include "modules.h"
#include "dirent.h"

void writeASM(string vmFileName, CodeWriter &asmWriter)
{
    Parser vmParser(vmFileName);
    asmWriter.setFileName(vmFileName);
    while (vmParser.hasMoreCommands())
    {
        vmParser.advance();
        if (vmParser.commandType() == C_ARITHMETIC)
            asmWriter.writeArithmetic(vmParser.arg1());
        else if (vmParser.commandType() == C_PUSH)
            asmWriter.WritePushPop(C_PUSH, vmParser.arg1(), 
            vmParser.arg2());
        else if (vmParser.commandType() == C_POP)
            asmWriter.WritePushPop(C_POP, vmParser.arg1(),
            vmParser.arg2());
        else
            continue;  // modify this later
    }
}

int main(int argc, char* argv[])
{
    string FileOrDirName(argv[1]);
    int extension_pos = FileOrDirName.find(".vm");
    if (extension_pos == string::npos)  // a directory
    {
        string projectName = FileOrDirName.substr(FileOrDirName.rfind('/')+1);
        CodeWriter asmWriter(projectName+".asm");
        DIR *dir; struct dirent *ent;
        if ((dir = opendir (FileOrDirName.c_str())) != NULL)
        {
            while ((ent = readdir (dir)) != NULL)
                writeASM(ent->d_name, asmWriter);
            closedir (dir);
        } else return -1;
    }
    else  // a single file
    {
        string fileName = FileOrDirName.substr(FileOrDirName.rfind('/')+1, extension_pos);
        CodeWriter asmWriter(fileName+".asm");
        writeASM(FileOrDirName, asmWriter);
    }
    return 0;
}