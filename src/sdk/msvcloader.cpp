#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "globals.h"
#include "msvcloader.h"

MSVCLoader::MSVCLoader(cbProject* project)
    : m_pProject(project)
{
	//ctor
}

MSVCLoader::~MSVCLoader()
{
	//dtor
}

bool MSVCLoader::Open(const wxString& filename)
{
    // open file
    wxFile file(filename);

    if (!file.IsOpened())
        return false;

    char* buff = m_FileContents.GetWriteBuf(file.Length());
    file.Read(buff, file.Length());
    file.Close();
    m_FileContents.UngetWriteBuf();
    
    // the file is read, now process it
    Manager::Get()->GetMessageManager()->DebugLog(_("Importing MSVC project: %s"), filename.c_str());
    bool ret = ProcessContents();
    Manager::Get()->GetMessageManager()->DebugLog(_("Finished %s"), ret ? "succesfully" : "with errors");
    return ret;
}

bool MSVCLoader::Save(const wxString& filename)
{
    // no support to save MSVC projects
    return false;
}

bool MSVCLoader::ProcessContents()
{
    /*
    OK, this will be a *simple* loader for MSVC files. Don't expect too much.
    It is only added for the convenience of the users...
    
    We 'll get the project name from:
        # Microsoft Developer Studio Project File - Name="game" - Package Owner=<4>
    We 'll find the project files by looking for:
        SOURCE=<relative_filename>
    We 'll also read (and convert) the following:
        # ADD CPP
        # ADD LINK32
    */

    m_pProject->ClearAllProperties();
    m_pProject->SetModified(true);

    wxString tmp;
    wxChar ch;
    
    // step 1: get the name
    int idx = m_FileContents.Find("# Microsoft Developer Studio Project File - Name=\"");
    if (idx == -1)
    {
        wxMessageBox(_("Can't read project's name..."), _("Error"), wxICON_ERROR);
        return false;
    }
    // we 're at the name starting quote
    idx += 50;
    ch = m_FileContents.GetChar(idx);
    while (ch && ch != '\"')
    {
        tmp << ch;
        ch = m_FileContents.GetChar(++idx);
    }
    m_pProject->SetTitle(tmp);
    
    // step 2: get the flags
    idx = m_FileContents.Find("# ADD BASE CPP");
    if (idx == -1)
    {
        wxMessageBox(_("Can't read project's base compiler flags..."), _("Error"), wxICON_ERROR);
        return false;
    }
    // we 're at the name starting quote
    tmp = "";
    idx += 14;
    ch = m_FileContents.GetChar(idx);
    while (ch && ch != '\r' && ch != '\n')
    {
        tmp << ch;
        ch = m_FileContents.GetChar(++idx);
    }
    ProcessCompilerOptions(tmp);
    //Manager::Get()->GetMessageManager()->DebugLog("Project base compiler flags: " + tmp);
    
    idx = m_FileContents.Find("# ADD CPP");
    if (idx == -1)
    {
        wxMessageBox(_("Can't read project's compiler flags..."), _("Error"), wxICON_ERROR);
        return false;
    }
    // we 're at the name starting quote
    idx += 9;
    tmp = "";
    ch = m_FileContents.GetChar(idx);
    while (ch && ch != '\r' && ch != '\n')
    {
        tmp << ch;
        ch = m_FileContents.GetChar(++idx);
    }
    ProcessCompilerOptions(tmp);
    //Manager::Get()->GetMessageManager()->DebugLog("Project compiler flags: " + tmp);

    // step 3: get the linker flags
    idx = m_FileContents.Find("# ADD BASE LINK32");
    if (idx == -1)
    {
        idx = m_FileContents.Find("# ADD BASE LIB32");
        if (idx == -1)
        {
            wxMessageBox(_("Can't read project's base linker flags..."), _("Error"), wxICON_ERROR);
            return false;
        }
    }
    // we 're at the name starting quote
    tmp = "";
    idx += 17;
    ch = m_FileContents.GetChar(idx);
    while (ch && ch != '\r' && ch != '\n')
    {
        tmp << ch;
        ch = m_FileContents.GetChar(++idx);
    }
    ProcessLinkerOptions(tmp);
    //Manager::Get()->GetMessageManager()->DebugLog("Project base linker flags: " + tmp);
    
    idx = m_FileContents.Find("# ADD LINK32");
    if (idx == -1)
    {
        idx = m_FileContents.Find("# ADD LIB32");
        if (idx == -1)
        {
            wxMessageBox(_("Can't read project's linker flags..."), _("Error"), wxICON_ERROR);
            return false;
        }
    }
    // we 're at the name starting quote
    tmp = "";
    idx += 12;
    ch = m_FileContents.GetChar(idx);
    while (ch && ch != '\r' && ch != '\n')
    {
        tmp << ch;
        ch = m_FileContents.GetChar(++idx);
    }
    ProcessLinkerOptions(tmp);
    //Manager::Get()->GetMessageManager()->DebugLog("Project linker flags: " + tmp);

    // step 4: locate files
    idx = m_FileContents.Find("SOURCE=");
    while (idx != -1)
    {
        tmp = "";
        idx += 7;
        ch = m_FileContents.GetChar(idx);
        while (ch != '\r' && ch != '\n')
        {
            tmp << ch;
            ch = m_FileContents.GetChar(++idx);
        }
        m_pProject->AddFile(0, RemoveQuotes(tmp));
        
        m_FileContents.Remove(1, idx + 7);
        idx = m_FileContents.Find("SOURCE=");
    }

    return true;
}

void MSVCLoader::ProcessCompilerOptions(const wxString& opts)
{
    wxArrayString array;
    array = GetArrayFromString(opts, " ");
    
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        wxString opt = array[i];
        opt.Trim();

        if (opt.Matches("/D"))
            m_pProject->AddCompilerOption("-D" + RemoveQuotes(array[++i]));
        else if (opt.Matches("/U"))
            m_pProject->AddCompilerOption("-U" + RemoveQuotes(array[++i]));
        else if (opt.Matches("/I"))
            m_pProject->AddIncludeDir(RemoveQuotes(array[++i]));
        else if (opt.Matches("/W0"))
            m_pProject->AddCompilerOption("-w");
        else if (opt.Matches("/O1") ||
                opt.Matches("/O2") ||
                opt.Matches("/O3"))
            m_pProject->AddCompilerOption("-O2");
        else if (opt.Matches("/W1") ||
                opt.Matches("/W2") ||
                opt.Matches("/W3"))
            m_pProject->AddCompilerOption("-W");
        else if (opt.Matches("/W4"))
            m_pProject->AddCompilerOption("-Wall");
        else if (opt.Matches("/WX"))
            m_pProject->AddCompilerOption("-Werror");
        else if (opt.Matches("/GX"))
            m_pProject->AddCompilerOption("-fexceptions");
        else if (opt.Matches("/Ob0"))
            m_pProject->AddCompilerOption("-fno-inline");
        else if (opt.Matches("/Ob2"))
            m_pProject->AddCompilerOption("-finline-functions");
        else if (opt.Matches("/Oy"))
            m_pProject->AddCompilerOption("-fomit-frame-pointer");
        else if (opt.Matches("/GB"))
            m_pProject->AddCompilerOption("-mcpu=pentiumpro -D_M_IX86=500");
        else if (opt.Matches("/G6"))
            m_pProject->AddCompilerOption("-mcpu=pentiumpro -D_M_IX86=600");
        else if (opt.Matches("/G5"))
            m_pProject->AddCompilerOption("-mcpu=pentium -D_M_IX86=500");
        else if (opt.Matches("/G4"))
            m_pProject->AddCompilerOption("-mcpu=i486 -D_M_IX86=400");
        else if (opt.Matches("/G3"))
            m_pProject->AddCompilerOption("-mcpu=i386 -D_M_IX86=300");
        else if (opt.Matches("/Za"))
            m_pProject->AddCompilerOption("-ansi");
        else if (opt.Matches("/Zp1"))
            m_pProject->AddCompilerOption("-fpack-struct");
        else if (opt.Matches("/nologo"))
        {
            // do nothing (ignore silently)
        }
        else
            Manager::Get()->GetMessageManager()->DebugLog("Unknown compiler option: " + opt);
    }
}

void MSVCLoader::ProcessLinkerOptions(const wxString& opts)
{
    wxArrayString array;
    array = GetArrayFromString(opts, " ");
    
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        wxString opt = array[i];
        opt.Trim();
        if (opt.Matches("/dll"))
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(0);
            if (bt)
                bt->SetTargetType(ttDynamicLib);
        }
        else if (opt.StartsWith("/libpath:"))
        {
            opt.Remove(0, 9);
            m_pProject->AddLibDir(RemoveQuotes(opt));
        }
        else if (opt.StartsWith("/base:"))
        {
            opt.Remove(0, 6);
            m_pProject->AddLinkerOption("--image-base " + RemoveQuotes(opt));
        }
        else if (opt.StartsWith("/implib:"))
        {
            opt.Remove(0, 8);
            m_pProject->AddLinkerOption("--implib " + RemoveQuotes(opt));
        }
        else if (opt.StartsWith("/map:"))
        {
            opt.Remove(0, 5);
            m_pProject->AddLinkerOption("-Map " + RemoveQuotes(opt) + ".map");
        }
        else if (opt.StartsWith("/subsystem:"))
        {
            opt.Remove(0, 11);
            ProjectBuildTarget* bt = 0L;
            if (opt.Matches("windows"))
            {
                bt = m_pProject->GetBuildTarget(0);
                if (bt)
                    bt->SetTargetType(ttExecutable);
            }
            else if (opt.Matches("console"))
            {
                bt = m_pProject->GetBuildTarget(0);
                if (bt)
                    bt->SetTargetType(ttConsoleOnly);
            }
        }
        else if (!opt.StartsWith("/"))
        {
            // probably linking lib
            int idx = opt.Find(".lib");
            if (idx != -1)
                opt.Remove(idx);
/* TODO (mandrav#1#): Make it compiler-agnostic */
            m_pProject->AddLinkerOption("-l" + opt);
        }
        else if (opt.Matches("/nologo"))
        {
            // do nothing (ignore silently)
        }
        else
            Manager::Get()->GetMessageManager()->DebugLog("Unknown linker option: " + opt);
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
