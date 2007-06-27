#ifdef __WXMSW__ // For Windows Only

#include <sdk.h>
#ifndef CB_PRECOMP
#   include "compiler.h"
#   include "cbproject.h"
#   include "projectbuildtarget.h"
#   include "messagemanager.h"
#   include "macrosmanager.h"
#endif
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
    wxString Result = compiler->GetSwitches().libDirs + _T(" ");
    if (target)
    {
        wxString tmp, targetStr, projectStr;
        // First prepare the target
        const wxArrayString targetArr = target->GetLibDirs();
        for (size_t i = 0; i < targetArr.GetCount(); ++i)
        {
            tmp = targetArr[i];
            Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp, target);
            targetStr << tmp << _T(";");
        }
        // Now for project
        const wxArrayString projectArr = target->GetParentProject()->GetLibDirs();
        for (size_t i = 0; i < projectArr.GetCount(); ++i)
        {
            tmp = projectArr[i];
            Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp, target);
            projectStr << tmp << _T(";");
        }
        // Decide order and arrange it
        Result << GetOrderedOptions(target, ortLibDirs, projectStr, targetStr);
    }
    // Finally add the compiler options
    const wxArrayString compilerArr = compiler->GetLibDirs();
    wxString tmp, compilerStr;
    for (size_t i = 0; i < compilerArr.GetCount(); ++i)
    {
        tmp = compilerArr[i];
        Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp, target);
        compilerStr << tmp << _T(";");
    }
    // Now append it
    Result << compilerStr;
    // Remove last ';' char
    Result = Result.Trim(true);
    if (Result.Right(1).IsSameAs(_T(';')))
        Result = Result.RemoveLast();
    return Result;
}

wxString CompilerOWGenerator::SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString Temp, LinkerOptions, Result;
    wxArrayString ComLinkerOptions, OtherLinkerOptions, LinkerOptionsArr;
    int i, j, Count;

    for (j = 0; j < 3; ++j)
    {
        LinkerOptions = wxEmptyString;
        if (j == 0 && target)
        {
            ComLinkerOptions = target->GetCompilerOptions();
            OtherLinkerOptions = target->GetLinkerOptions();
        }
        else if (j == 1 && target)
        {
            ComLinkerOptions = target->GetParentProject()->GetCompilerOptions();
            OtherLinkerOptions = target->GetParentProject()->GetLinkerOptions();
        }
        else if (j == 2)
        {
            ComLinkerOptions = compiler->GetCompilerOptions();
            OtherLinkerOptions = compiler->GetLinkerOptions();
        }
        if (!ComLinkerOptions.IsEmpty())
        {
            Count = ComLinkerOptions.GetCount();
            for (i = 0; i < Count; ++i)
            {
                Temp = ComLinkerOptions[i];

// TODO (Biplab#5#): Move the linker options parsing code to a different function
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
        /* Following code will allow user to add any valid linker option
        *  in target's linker option section.
        */
        if (!OtherLinkerOptions.IsEmpty())
        {
            int Count = OtherLinkerOptions.GetCount();
            for (i = 0; i < Count; ++i)
            {
                Temp = OtherLinkerOptions[i];
                /* Let's make a small check. It should not start with - or /  */
                if ((Temp[0] != _T('-')) && (Temp[0] != _T('/')))
                    LinkerOptions = LinkerOptions + Temp + _T(" ");
            }
        }
        // Finally add it to an array
        LinkerOptionsArr.Add(LinkerOptions);
    }
    // Arrange them in specified order
    Result = GetOrderedOptions(target, ortLinkerOptions, LinkerOptionsArr[1], LinkerOptionsArr[0]);
    // Now append compiler level options
    Result << LinkerOptionsArr[2];

    /* If the system flag couldn't be resolved due to conflict or
       other reasons, following code will ensure that the system flag
       is specified to linker to ensure linking
    */
    if (Result.Find(_T("system")) == wxNOT_FOUND)
        return Result + MapTargetType(target->GetTargetType());
    else
        return Result;
}

wxString CompilerOWGenerator::SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString Result;
    wxString targetStr, projectStr, compilerStr;
    wxArrayString Libs;

    Result = _T("library ");
    if (target)
    {
        // Start with target first
        Libs = target->GetLinkLibs();
        for (size_t i = 0; i < Libs.GetCount(); ++i)
            targetStr << Libs[i] + _T(",");
        // Next process project
        Libs = target->GetParentProject()->GetLinkLibs();
        for (size_t i = 0; i < Libs.GetCount(); ++i)
            projectStr << Libs[i] + _T(",");
        // Set them in proper order
        if (!targetStr.IsEmpty() || !projectStr.IsEmpty())
            Result << GetOrderedOptions(target, ortLinkerOptions, projectStr, targetStr);
    }
    // Now prepare compiler libraries, if any
    Libs = compiler->GetLinkLibs();
    for (size_t i = 0; i < Libs.GetCount(); ++i)
        compilerStr << Libs[i] << _T(",");
    // Append it to result
    Result << compilerStr;
    // Now trim trailing spaces, if any, and the ',' at the end
    Result = Result.Trim(true);
    if (Result.Right(1).IsSameAs(_T(',')))
        Result = Result.RemoveLast();

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
