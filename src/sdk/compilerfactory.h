#ifndef COMPILERFACTORY_H
#define COMPILERFACTORY_H

#include "settings.h"
#include "compiler.h"

// Well, not really a factory ;)

WX_DEFINE_ARRAY(Compiler*, CompilersArray);

class DLLIMPORT CompilerFactory
{
    public:
        static void RegisterCompiler(Compiler* compiler);
        static void RegisterUserCompilers();
        static int CreateCompilerCopy(Compiler* compiler);
        static void RemoveCompiler(Compiler* compiler);
        static void UnregisterCompilers();
        static void SaveSettings();
        static void LoadSettings();
        static bool CompilerIndexOK(int compilerIdx);
        static CompilersArray Compilers;
};

#endif // COMPILERFACTORY_H
