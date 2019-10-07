#include "modules.h"
#include "dirent.h"

void writeASM(string vmPath, string vmFileName, CodeWriter &asmWriter)
{
    Parser vmParser(vmPath);
    asmWriter.setFileName(vmFileName);
    while (vmParser.hasMoreCommands())
    {
        vmParser.advance();
        command_t type = vmParser.commandType();
        if (type == C_ARITHMETIC)
            asmWriter.writeArithmetic(vmParser.arg1());
        else if (type == C_PUSH)
            asmWriter.WritePushPop(C_PUSH, vmParser.arg1(), 
            vmParser.arg2());
        else if (type == C_POP)
            asmWriter.WritePushPop(C_POP, vmParser.arg1(),
            vmParser.arg2());
        else
            continue;  // modify this later
    }
}

int main(int argc, char* argv[])
{
    string path(argv[1]);
    string fileName = path.substr(path.rfind('/')+1);
    int extension_pos = fileName.find(".vm");
    if (extension_pos == string::npos)  // a directory
    {
        CodeWriter asmWriter(fileName+".asm");
        DIR *dir; struct dirent *ent;
        if ((dir = opendir (path.c_str())) != NULL)
        {
            while ((ent = readdir (dir)) != NULL)
                writeASM(ent->d_name, fileName, asmWriter);
            closedir (dir);
        } 
        else return -1;
    }
    else  // a single file
    {
        fileName = fileName.substr(0, extension_pos);
        CodeWriter asmWriter(fileName+".asm");
        writeASM(path, fileName, asmWriter);
    }
    return 0;
}