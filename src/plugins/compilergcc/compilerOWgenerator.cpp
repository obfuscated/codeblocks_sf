/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifdef __WXMSW__ // For Windows Only

#include <sdk.h>
#ifndef CB_PRECOMP
#   include "compiler.h"
#   include "cbproject.h"
#   include "projectbuildtarget.h"
#   include "logmanager.h"
#   include "macrosmanager.h"
#endif
#include "compileoptionsbase.h"
#include "compilerOWgenerator.h"


CompilerOWGenerator::CompilerOWGenerator()
{
    //ctor
    m_DebuggerType = wxEmptyString;
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

                // Replace any macros
                Manager::Get()->GetMacrosManager()->ReplaceMacros(Temp, target);

// TODO (Biplab#5#): Move the linker options parsing code to a different function
                //Let's not scan all the options unnecessarily
                if (Temp.Matches(_T("-b*")))
                {
                    Temp = MapTargetType(Temp, target->GetTargetType());
                    if (!Temp.IsEmpty() && LinkerOptions.Find(_T("system")) == wxNOT_FOUND)
                        LinkerOptions += Temp;
                }
                // TODO: Map and Set All Debug Flags
                else if (Temp.Matches(_T("-d*")) && Temp.Length() <= 4)
                {
                    LinkerOptions = LinkerOptions + MapDebugOptions(Temp);
                }
                // Debugger Type: -hw (Watcom), -hd (Dwarf), -hc (CodeView)
                else if (Temp.Matches(_T("-h?")))
                {
                    MapDebuggerOptions(Temp);
                }
                else if (Temp.StartsWith(_T("-l=")))
                {
                    Temp = Temp.AfterFirst(_T('='));
                    if (LinkerOptions.Find(_T("system")) == wxNOT_FOUND && !Temp.IsEmpty())
                        LinkerOptions += _T("system ") + Temp + _T(" ");
                }
                else if (Temp.StartsWith(_T("-fm")))
                {
                    LinkerOptions += _T("option map");
                    int pos = Temp.Find(_T('='));
                    if (pos != wxNOT_FOUND)
                        LinkerOptions += Temp.Mid(pos);
                    LinkerOptions.Append(_T(" "));
                }
                else if (Temp.StartsWith(_T("-k")))
                {
                    LinkerOptions += _T("option stack=") + Temp.Mid(2) + _T(" ");
                }
                else if (Temp.StartsWith(_T("@")))
                {
                    LinkerOptions += Temp + _T(" ");
                }
            }
        }
        /* Following code will allow user to add any valid linker option
        *  in target's linker option section.
        */
        if (!OtherLinkerOptions.IsEmpty())
        {
            Count = OtherLinkerOptions.GetCount();
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
    if (target)
        Result = GetOrderedOptions(target, ortLinkerOptions, LinkerOptionsArr[1], LinkerOptionsArr[0]);
    // Now append compiler level options
    Result << LinkerOptionsArr[2];

    return Result;
}

wxString CompilerOWGenerator::SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString Result;
    wxString targetStr, projectStr, compilerStr;
    wxArrayString Libs;

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

    if (!Result.IsEmpty())
        Result.Prepend(_T("library "));
    return Result;
}

wxString CompilerOWGenerator::MapTargetType(const wxString& Opt, int target_type)
{
    if (Opt.IsSameAs(_T("-bt=nt")) || Opt.IsSameAs(_T("-bcl=nt")))
    {
        if (target_type == ttExecutable || target_type == ttStaticLib) // Win32 Executable
            return _T("system nt_win ");
        else if (target_type == ttConsoleOnly) // Console
            return _T("system nt ");
        else if (target_type == ttDynamicLib) // DLL
            return _T("system nt_dll ");
        else
            return _T("system nt_win ref '_WinMain@16' "); // Default to Win32 executables
    }
    else if (Opt.IsSameAs(_T("-bt=linux")) || Opt.IsSameAs(_T("-bcl=linux")))
    {
        /* The support is experimental. Need proper manual to improve it. */
        return _T("system linux ");
    }
    return wxEmptyString;
}

/* The following function will be expanded later
   to incorporate detailed debug options
*/
wxString CompilerOWGenerator::MapDebugOptions(const wxString& Opt)
{
    if (Opt.IsSameAs(_T("-d0"))) // No Debug
    {
        return wxEmptyString;
    }
    if (Opt.IsSameAs(_T("-d1")))
    {
        return wxString(_T("debug ") + m_DebuggerType + _T("lines "));
    }
    if (Opt.IsSameAs(_T("-d2")) || Opt.IsSameAs(_T("-d3")))
    {
        return wxString(_T("debug ") + m_DebuggerType + _T("all "));
    }
    // Nothing Matched
    return wxEmptyString;
}

void CompilerOWGenerator::MapDebuggerOptions(const wxString& Opt)
{
  if (Opt.IsSameAs(_T("-hw")))
  {
      m_DebuggerType = _T("watcom ");
  }
  else if (Opt.IsSameAs(_T("-hd")))
  {
      m_DebuggerType = _T("dwarf ");
  }
  else if (Opt.IsSameAs(_T("-hc")))
  {
      m_DebuggerType = _T("codeview ");
  }
  else
  {
      m_DebuggerType = wxEmptyString;
  }
}

#endif // __WXMSW__
