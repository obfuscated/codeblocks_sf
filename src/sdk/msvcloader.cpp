#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "globals.h"
#include "importers_globals.h"
#include "msvcloader.h"
#include "compilerfactory.h"
#include "compiler.h"
#include "multiselectdlg.h"

MSVCLoader::MSVCLoader(cbProject* project)
    : m_pProject(project),
    m_ConvertSwitches(true)
{
	//ctor
}

MSVCLoader::~MSVCLoader()
{
	//dtor
}

bool MSVCLoader::Open(const wxString& filename)
{
 /* NOTE (mandrav#1#): not necessary to ask for switches conversion... */
    m_ConvertSwitches = m_pProject->GetCompilerIndex() == 0; // GCC

   m_Filename = filename;
    if (!ReadConfigurations())
        return false;

    // the file is read, now process it
    Manager::Get()->GetMessageManager()->DebugLog(_("Importing MSVC project: %s"), filename.c_str());

    // delete all targets of the project (we 'll create new ones from the imported configurations)
    while (m_pProject->GetBuildTargetsCount())
        m_pProject->RemoveBuildTarget(0);

    wxArrayInt selected_indices;
    if (ImportersGlobals::ImportAllTargets)
    {
        // don't ask; just fill selected_indices with all indices
        for (size_t i = 0; i < m_Configurations.GetCount(); ++i)
            selected_indices.Add(i);
    }
    else
    {
        // ask the user to select a configuration - multiple choice ;)
        MultiSelectDlg dlg(0, m_Configurations, true, _("Select configurations to import:"), m_Filename.GetName());
        if (dlg.ShowModal() == wxID_CANCEL)
        {
            Manager::Get()->GetMessageManager()->DebugLog("Canceled...");
            return false;
        }
        selected_indices = dlg.GetSelectedIndices();
    }

    // create all selected targets
    for (size_t i = 0; i < selected_indices.GetCount(); ++i)
    {
        if (!ParseConfiguration(selected_indices[i]))
            return false;
    }
    
    m_pProject->SetTitle(m_Filename.GetName());
    return ParseSourceFiles();
}

bool MSVCLoader::Save(const wxString& filename)
{
    // no support to save MSVC projects
    return false;
}

bool MSVCLoader::ReadConfigurations()
{
    m_Configurations.Clear();
    m_ConfigurationsLineIndex.Clear();
    m_Type = ttExecutable;
    m_BeginTargetLine = -1;

    wxFileInputStream file(m_Filename.GetFullPath());
    if (!file.Ok())
        return false; // error opening file???

    wxArrayString comps;
    wxTextInputStream input(file);

    int currentLine = 0;
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(true);
        line.Trim(false);
        int size = -1;
        if (line.StartsWith("# TARGTYPE"))
        {
            int idx = line.Find(' ', true);
            if (idx != -1)
            {
                int typ = atoi(line.Mid(idx + 3, 4));
                switch (typ)
                {
                    case 101: m_Type = ttExecutable; break;
                    case 102: m_Type = ttDynamicLib; break;
                    case 103: m_Type = ttConsoleOnly; break;
                    case 104: m_Type = ttStaticLib; break;
                    // I 've seen 0x010a "Generic project" which was empty.
                    // don't know what to do with it...
                    default: break;
                }
                Manager::Get()->GetMessageManager()->DebugLog("Project type set to %d", typ);
            }
            continue;
        }
        else if (line.StartsWith("!IF  \"$(CFG)\" =="))
            size = 16;
        else if (line.StartsWith("!ELSEIF  \"$(CFG)\" =="))
            size = 20;
        else if (line == "# Begin Target")
        {
            // done
            m_BeginTargetLine = currentLine;
            break;
        }
        if (size != -1)
        {
            // read configuration name
            line.Remove(0, size);
            line.Trim(true);
            line.Trim(false);
            wxString tmp = RemoveQuotes(line);
            if (m_Configurations.Index(tmp) == wxNOT_FOUND)
            {
                m_Configurations.Add(tmp);
                m_ConfigurationsLineIndex.Add(currentLine + 1);
                Manager::Get()->GetMessageManager()->DebugLog("Detected configuration '%s'", tmp.c_str());
            }
        }
        ++currentLine;
    }
    return true;
}

bool MSVCLoader::ParseConfiguration(int index)
{
    wxFileInputStream file(m_Filename.GetFullPath());
    if (!file.Ok())
        return false; // error opening file???

    // create new target
    ProjectBuildTarget* bt = m_pProject->AddBuildTarget(m_Configurations[index]);
    if (!bt)
        return false;
    bt->SetCompilerIndex(m_pProject->GetCompilerIndex());
    bt->SetTargetType(m_Type);
    bt->SetOutputFilename(bt->SuggestOutputFilename());

    wxTextInputStream input(file);

    // go to the configuration's line
    int currentLine = 0;
    while (!file.Eof() && currentLine <= m_ConfigurationsLineIndex[index])
    {
        input.ReadLine();
        ++currentLine;
    }
    
    // start parsing the configuration
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(true);
        line.Trim(false);
        
        // we want empty lines (skipped) or lines starting with #
        // if we encounter a line starting with !, we break out of here
        if (line.GetChar(0) == '!')
            break;
        if (line.IsEmpty() || line.GetChar(0) != '#')
            continue;
        
//        if (line.StartsWith("# PROP BASE Output_Dir "))
        if (line.StartsWith("# PROP Output_Dir "))
        {
            line.Remove(0, 18);
            line.Trim(true);
            line.Trim(false);
            wxString tmp = RemoveQuotes(line);
            if (!line.IsEmpty())
            {
                wxFileName out = bt->GetOutputFilename();
                out.SetPath(out.GetPath() + wxFileName::GetPathSeparator() + tmp);
                bt->SetOutputFilename(out.GetFullPath());
            }
        }
//        else if (line.StartsWith("# PROP BASE Intermediate_Dir "))
        else if (line.StartsWith("# PROP Intermediate_Dir "))
        {
            line.Remove(0, 24);
            line.Trim(true);
            line.Trim(false);
            wxString tmp = RemoveQuotes(line);
            if (!line.IsEmpty())
            {
                bt->SetObjectOutput(tmp);
            }
        }
        else if (line.StartsWith("# ADD BASE CPP "))
        {
            line.Remove(0, 15);
            line.Trim(true);
            line.Trim(false);
            ProcessCompilerOptions(bt, line);
        }
        else if (line.StartsWith("# ADD CPP "))
        {
            line.Remove(0, 10);
            line.Trim(true);
            line.Trim(false);
            ProcessCompilerOptions(bt, line);
        }
        else if (line.StartsWith("# ADD BASE LIB32 "))
        {
            line.Remove(0, 17);
            line.Trim(true);
            line.Trim(false);
            ProcessLinkerOptions(bt, line);
        }
        else if (line.StartsWith("# ADD LIB32 "))
        {
            line.Remove(0, 12);
            line.Trim(true);
            line.Trim(false);
            ProcessLinkerOptions(bt, line);
        }
    }
    return true;
}

bool MSVCLoader::ParseSourceFiles()
{
    wxFileInputStream file(m_Filename.GetFullPath());
    if (!file.Ok())
        return false; // error opening file???

    wxTextInputStream input(file);

    // go to the begining of source files
    int currentLine = 0;
    while (!file.Eof() && currentLine < m_BeginTargetLine)
    {
        input.ReadLine();
        ++currentLine;
    }

    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(true);
        line.Trim(false);

        // we 're only interested in lines starting with SOURCE=
        if (!line.StartsWith("SOURCE="))
            continue;
        
        line.Remove(0, 7);
        line.Trim(true);
        line.Trim(false);
        
        ProjectFile* pf = m_pProject->AddFile(0, RemoveQuotes(line));
        if (pf)
        {
            // add it to all configurations, not just the first
            for (int i = 1; i < m_pProject->GetBuildTargetsCount(); ++i)
                pf->AddBuildTarget(m_pProject->GetBuildTarget(i)->GetTitle());
        }
    }
    return true;
}

void MSVCLoader::ProcessCompilerOptions(ProjectBuildTarget* target, const wxString& opts)
{
    wxArrayString array;
    array = GetArrayFromString(opts, " ");
    
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        wxString opt = array[i];
        opt.Trim();

        if (m_ConvertSwitches)
        {
            if (opt.Matches("/D"))
                target->AddCompilerOption("-D" + RemoveQuotes(array[++i]));
            else if (opt.Matches("/U"))
                target->AddCompilerOption("-U" + RemoveQuotes(array[++i]));
            else if (opt.Matches("/Zi") || opt.Matches("/ZI"))
                target->AddCompilerOption("-g");
            else if (opt.Matches("/I"))
                target->AddIncludeDir(RemoveQuotes(array[++i]));
            else if (opt.Matches("/W0"))
                target->AddCompilerOption("-w");
            else if (opt.Matches("/O1") ||
                    opt.Matches("/O2") ||
                    opt.Matches("/O3"))
                target->AddCompilerOption("-O2");
            else if (opt.Matches("/W1") ||
                    opt.Matches("/W2") ||
                    opt.Matches("/W3"))
                target->AddCompilerOption("-W");
            else if (opt.Matches("/W4"))
                target->AddCompilerOption("-Wall");
            else if (opt.Matches("/WX"))
                target->AddCompilerOption("-Werror");
            else if (opt.Matches("/GX"))
                target->AddCompilerOption("-fexceptions");
            else if (opt.Matches("/Ob0"))
                target->AddCompilerOption("-fno-inline");
            else if (opt.Matches("/Ob2"))
                target->AddCompilerOption("-finline-functions");
            else if (opt.Matches("/Oy"))
                target->AddCompilerOption("-fomit-frame-pointer");
            else if (opt.Matches("/GB"))
                target->AddCompilerOption("-mcpu=pentiumpro -D_M_IX86=500");
            else if (opt.Matches("/G6"))
                target->AddCompilerOption("-mcpu=pentiumpro -D_M_IX86=600");
            else if (opt.Matches("/G5"))
                target->AddCompilerOption("-mcpu=pentium -D_M_IX86=500");
            else if (opt.Matches("/G4"))
                target->AddCompilerOption("-mcpu=i486 -D_M_IX86=400");
            else if (opt.Matches("/G3"))
                target->AddCompilerOption("-mcpu=i386 -D_M_IX86=300");
            else if (opt.Matches("/Za"))
                target->AddCompilerOption("-ansi");
            else if (opt.Matches("/Zp1"))
                target->AddCompilerOption("-fpack-struct");
            else if (opt.Matches("/nologo"))
            {
                // do nothing (ignore silently)
            }
            else
                Manager::Get()->GetMessageManager()->DebugLog("Unhandled compiler option: " + opt);
        }
        else // !m_ConvertSwitches
        {
            // only differentiate includes and definitions
            if (opt.Matches("/I"))
                target->AddIncludeDir(RemoveQuotes(array[++i]));
            else if (opt.Matches("/D"))
                target->AddCompilerOption("/D" + RemoveQuotes(array[++i]));
            else if (opt.Matches("/U"))
                target->AddCompilerOption("/U" + RemoveQuotes(array[++i]));
            else if (opt.StartsWith("/Yu"))
                Manager::Get()->GetMessageManager()->DebugLog("Ignoring precompiled headers option (/Yu)");
            else
                target->AddCompilerOption(opt);
        }
    }
}

void MSVCLoader::ProcessLinkerOptions(ProjectBuildTarget* target, const wxString& opts)
{
    wxArrayString array;
    array = GetArrayFromString(opts, " ");
    
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        wxString opt = array[i];
        opt.Trim();
        
        if (m_ConvertSwitches)
        {
            if (opt.StartsWith("/libpath:"))
            {
                opt.Remove(0, 9);
                target->AddLibDir(RemoveQuotes(opt));
            }
            else if (opt.StartsWith("/base:"))
            {
                opt.Remove(0, 6);
                target->AddLinkerOption("--image-base " + RemoveQuotes(opt));
            }
            else if (opt.StartsWith("/implib:"))
            {
                opt.Remove(0, 8);
                target->AddLinkerOption("--implib " + RemoveQuotes(opt));
            }
            else if (opt.StartsWith("/map:"))
            {
                opt.Remove(0, 5);
                target->AddLinkerOption("-Map " + RemoveQuotes(opt) + ".map");
            }
            else if (opt.Matches("/nologo"))
            {
                // do nothing (ignore silently)
            }
            else if (opt.StartsWith("/out:"))
            {
                // do nothing; it is handled below, in common options
            }
            else
                Manager::Get()->GetMessageManager()->DebugLog("Unknown linker option: " + opt);
        }
        else // !m_ConvertSwitches
        {
            if (opt.StartsWith("/libpath:"))
            {
                opt.Remove(0, 9);
                target->AddLibDir(RemoveQuotes(opt));
            }
            else
            {
                // don't add linking lib (added below, in common options)
                int idx = opt.Find(".lib");
                if (idx == -1)
                    target->AddLinkerOption(opt);
            }
        }

        // common options
        if (!opt.StartsWith("/"))
        {
            // probably linking lib
            int idx = opt.Find(".lib");
            if (idx != -1)
            {
                opt.Remove(idx);
                target->AddLinkLib(opt);
            }
        }
        else if (opt.StartsWith("/out:"))
        {
            opt.Remove(0, 5);
            opt = RemoveQuotes(opt);
            if (m_Type == ttStaticLib)
            {
                // convert lib filename based on compiler
/* NOTE (mandrav#1#): I think I should move this code somewhere more accessible...
I need it here and there... */
                wxFileName orig = target->GetOutputFilename();
                wxFileName newf = opt;
                if (newf.IsRelative())
                    newf.MakeAbsolute(m_pProject->GetBasePath());
                Compiler* compiler = CompilerFactory::Compilers[m_pProject->GetCompilerIndex()];
                newf.SetExt(compiler->GetSwitches().libExtension);
                wxString name = newf.GetName();
                wxString prefix = compiler->GetSwitches().libPrefix;
                if (!prefix.IsEmpty() && !name.StartsWith(prefix))
                    newf.SetName(prefix + name);
                target->SetOutputFilename(newf.GetFullPath());
            }
            else
                target->SetOutputFilename(opt);
        }
    }
}

wxString MSVCLoader::RemoveQuotes(const wxString& src)
{
    wxString res = src;
    if (res.StartsWith("\""))
    {
        res.Remove(0, 1);
        res.Remove(res.Length() - 1);
    }
//    Manager::Get()->GetMessageManager()->DebugLog("Removing quotes: %s --> %s", src.c_str(), res.c_str());
    return res;
}
