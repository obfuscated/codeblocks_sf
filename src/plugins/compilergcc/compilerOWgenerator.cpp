#ifdef __WXMSW__ // For Windows Only

#include <sdk.h>
#include "compileoptionsbase.h"
#include "compilerOWgenerator.h"


CompilerOWGenerator::CompilerOWGenerator()
{
    //ctor
}

CompilerOWGenerator::~CompilerOWGenerator()
{
    //dtor
}

wxString CompilerOWGenerator::SetupLibrariesDirs(Compiler* compiler, ProjectBuildTarget* target)
{
    wxArrayString LibDirs = compiler->GetLibDirs();
    if (LibDirs.IsEmpty())
        return wxEmptyString;
    int i, Count = LibDirs.GetCount();
    wxString Result = _T("");
    for (i = 0; i < Count; ++i)
        Result = Result + compiler->GetSwitches().libDirs + LibDirs[i] + _T(" ");
    //Now Read all the targets
    LibDirs = target->GetLibDirs();
    Count = LibDirs.GetCount();
    for (i = 0; i < Count; ++i)
        Result = Result + compiler->GetSwitches().libDirs + LibDirs[i] + _T(" ");
    LibDirs = compiler->GetLinkLibs();
    return Result;
}

wxString CompilerOWGenerator::SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString Temp, LinkerOptions;
    wxArrayString ComLinkerOptions;
    int i, j, Count;
    LinkerOptions = Temp = wxEmptyString;

    for (j = 0; j < 2; ++j)
    {
        if (j == 0)
            ComLinkerOptions = target->GetParentProject()->GetCompilerOptions();
        else
            ComLinkerOptions = target->GetCompilerOptions();
        if (!ComLinkerOptions.IsEmpty())
        {
            Count = ComLinkerOptions.GetCount();
            for (i = 0; i < Count; ++i)
            {
                Temp = ComLinkerOptions[i];

                //Let's not scan all the options unnecessarily
                if (Temp.Matches(_T("-b*")))
                {
                    if (Temp.IsSameAs(_T("-bc")))
                    {
                        if (target->GetTargetType() == ttConsoleOnly)
                        {
                            LinkerOptions = LinkerOptions + MapTargetType(target->GetTargetType());
                            break;
                        }
                        else
                        {
                            LOG_ERROR(_T("Compiler Option and Target Type are incompatible. Please check your project settings."));
                            break;
                        }
                    }
                    else if (Temp.IsSameAs(_T("-bg")))
                    {
                        if (target->GetTargetType() == ttExecutable)
                        {
                            LinkerOptions = LinkerOptions + MapTargetType(target->GetTargetType());
                            break;
                        }
                        else
                        {
                            LOG_ERROR(_T("Compiler Option and Target Type are incompatible. Please check your project settings."));
                            break;
                        }
                    }
                    else if (Temp.IsSameAs(_T("-bd")))
                    {
                        if (target->GetTargetType() == ttDynamicLib)
                        {
                            LinkerOptions = LinkerOptions + MapTargetType(target->GetTargetType());
                            break;
                        }
                        else
                        {
                            LOG_ERROR(_T("Compiler Option and Target Type are incompatible. Please check your project settings."));
                            break;
                        }
                    }
                }

                // TODO: Map and Set All Debug Flags
                if (Temp.Matches(_T("-d*")) && Temp.Length() <= 4)
                    LinkerOptions = LinkerOptions + MapDebugOptions(Temp);
            }
        }
    }

    /* If the system flag couldn't be resolved due to conflict or
       other reasons, following code will ensure that the system flag
       is specified to linker to ensure linking
    */
    if (LinkerOptions.Find(_T("system")) == wxNOT_FOUND)
        return LinkerOptions + MapTargetType(target->GetTargetType());
    else
        return LinkerOptions;
}

wxString CompilerOWGenerator::SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString Result;
    int i, Count;
    wxArrayString Libs;

    Libs = target->GetParentProject()->GetLinkLibs();
    Result = _T("");
    Count = Libs.GetCount();
    for (i = 0; i < Count; ++i)
        Result = Result + _T("library ") + Libs[i] + _T(" ");
    Libs = target->GetLinkLibs();
    Count = Libs.GetCount();
    for (i = 0; i < Count; ++i)
        Result = Result + _T("library ") + Libs[i] + _T(" ");

    return Result;
}

wxString CompilerOWGenerator::MapTargetType(int Opt)
{
    switch (Opt)
    {
        case 0: // Win32 Executable
            return _T("system nt_win ");
            break;
        case 1: // Console
            return _T("system nt ");
            break;
        case 3: // DLL
            return _T("system nt_dll ");
            break;
        default:
            return _T("system nt_win ref '_WinMain@16' "); // Default to Win32 executables
            break;
    }
    return wxEmptyString;
}

/* The following function will be expanded later
   to incorporate detailed debug options
*/
wxString CompilerOWGenerator::MapDebugOptions(wxString Opt)
{
    if (Opt.IsSameAs(_T("-d0"))) // No Debug
        return wxEmptyString;
    if (Opt.IsSameAs(_T("-d1")))
        return _T("debug watcom lines ");
    if (Opt.IsSameAs(_T("-d2")))
        return _T("debug watcom all ");
    // Nothing Matched
    return wxEmptyString;
}

#endif // __WXMSW__
