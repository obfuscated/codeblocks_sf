#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include "manager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "globals.h"
#include "msvc7loader.h"
#include "multiselectdlg.h"
#include "importers_globals.h"

MSVC7Loader::MSVC7Loader(cbProject* project)
    : m_pProject(project),
    m_ConvertSwitches(false),
    m_Version(vcOther)
{
	//ctor
}

MSVC7Loader::~MSVC7Loader()
{
	//dtor
}

wxString MSVC7Loader::ReplaceMSVCMacros(const wxString& str)
{
    wxString ret = str;
    ret.Replace("$(OutDir)", m_OutDir);
    ret.Replace("$(ConfigurationName)", m_ConfigurationName);
    ret.Replace("$(ProjectName)", m_ProjectName);
    ret.Replace("$(TargetPath)", m_TargetPath);
    ret.Replace("$(TargetFileName)", m_TargetFilename);
    if (ret.StartsWith("\""))
    {
        ret.Remove(0, 1);
        ret.Remove(ret.Length() - 1);
    }

    // search for other $(...) occurences (env.vars)
    while (true)
    {
        int len = ret.Length();
        int idx = ret.Find("$(");
        if (idx == -1)
            break; // no more
        idx += 2; // move to first letter of env.var
        // find the last letter
        int last = -1;
        for (int i = idx; i < len; ++i)
        {
            if (ret.GetChar(i) == ')')
            {
                last = i;
                break;
            }
        }
        if (last < idx)
            break;
        wxString var = ret.Mid(idx, last - idx);
        if (var.IsEmpty())
            break;
        wxString envvar;
        if (!wxGetEnv(var, &envvar))
            break;
        ret.Replace("$(" + var + ")", envvar);
        
        wxFileName fname(ret);
        if (fname.IsAbsolute())
        {
            fname.MakeRelativeTo(m_pProject->GetBasePath());
            ret = fname.GetFullPath();
        }
    }

    return ret;
}

bool MSVC7Loader::Open(const wxString& filename)
{
    MessageManager* pMsg = Manager::Get()->GetMessageManager();
    if (!pMsg)
        return false;

/* NOTE (mandrav#1#): not necessary to ask for switches conversion... */
    m_ConvertSwitches = m_pProject->GetCompilerIndex() == 0; // GCC
    m_ProjectName = wxFileName(filename).GetName();

    pMsg->DebugLog(_("Importing MSVC 7.xx project: %s"), filename.c_str());

    TiXmlDocument doc(filename.c_str());
    if (!doc.LoadFile())
        return false;

    pMsg->DebugLog("Parsing project file...");
    TiXmlElement* root;
    
    root = doc.FirstChildElement("VisualStudioProject");
    if (!root)
    {
        pMsg->DebugLog("Not a valid MS Visual Studio project file...");
        return false;
    }
    if (strcmp(root->Attribute("ProjectType"), "Visual C++") != 0)
    {
        pMsg->DebugLog("Project is not Visual C++...");
        return false;
    }
    if (strncmp(root->Attribute("Version"), "7.0", 3) == 0)
        m_Version = vc70;
    else if (strncmp(root->Attribute("Version"), "7.1", 3) == 0)
        m_Version = vc71;
    else
    {
        pMsg->DebugLog(_("Project version is '%s'. Although this loader was designed for version 7.xx, will try to import..."), root->Attribute("Version"));
    }

    m_pProject->ClearAllProperties();
    m_pProject->SetModified(true);
    m_pProject->SetTitle(root->Attribute("Name"));

    // delete all targets of the project (we 'll create new ones from the imported configurations)
    while (m_pProject->GetBuildTargetsCount())
        m_pProject->RemoveBuildTarget(0);

    return DoSelectConfiguration(root);
}

bool MSVC7Loader::Save(const wxString& filename)
{
    // no support to save MSVC7 projects
    return false;
}

bool MSVC7Loader::DoSelectConfiguration(TiXmlElement* root)
{
    TiXmlElement* config = root->FirstChildElement("Configurations");
    if (!config)
    {
        Manager::Get()->GetMessageManager()->DebugLog("No 'Configurations' node...");
        return false;
    }
    
    TiXmlElement* confs = config->FirstChildElement("Configuration");
    if (!confs)
    {
        Manager::Get()->GetMessageManager()->DebugLog("No 'Configuration' node...");
        return false;
    }
    
    // build an array of all configurations
    wxArrayString configurations;
    while (confs)
    {
        configurations.Add(confs->Attribute("Name"));
        confs = confs->NextSiblingElement();
    }

    wxArrayInt selected_indices;
    if (ImportersGlobals::ImportAllTargets)
    {
        // don't ask; just fill selected_indices with all indices
        for (size_t i = 0; i < configurations.GetCount(); ++i)
            selected_indices.Add(i);
    }
    else
    {
        // ask the user to select a configuration - multiple choice ;)
        MultiSelectDlg dlg(0, configurations, true, _("Select configurations to import:"), m_pProject->GetTitle());
        if (dlg.ShowModal() == wxID_CANCEL)
        {
            Manager::Get()->GetMessageManager()->DebugLog("Canceled...");
            return false;
        }
        selected_indices = dlg.GetSelectedIndices();
    }

    confs = config->FirstChildElement("Configuration");
    int current_sel = 0;
    bool success = true;
    for (size_t i = 0; i < selected_indices.GetCount(); ++i)
    {
        // re-iterate configurations to find each selected one
        while (confs && current_sel++ < selected_indices[i])
            confs = confs->NextSiblingElement();
        if (!confs)
        {
            Manager::Get()->GetMessageManager()->DebugLog("Cannot find configuration nr %d...", selected_indices[i]);
            success = false;
            break;
        }

        Manager::Get()->GetMessageManager()->DebugLog("Importing configuration: %s", configurations[selected_indices[i]].c_str());

        // prepare the configuration name
        m_ConfigurationName = configurations[selected_indices[i]];
        int pos = m_ConfigurationName.Find('|');
        if (pos != wxNOT_FOUND)
            m_ConfigurationName.Remove(pos);

        // parse the selected configuration
        success = success && DoImport(confs);
        confs = confs->NextSiblingElement();
    }
    return success && DoImportFiles(root, selected_indices.GetCount());
}

bool MSVC7Loader::DoImport(TiXmlElement* conf)
{
    ProjectBuildTarget* bt = m_pProject->GetBuildTarget(m_ConfigurationName);
    if (!bt)
        bt = m_pProject->AddBuildTarget(m_ConfigurationName);
    bt->SetCompilerIndex(m_pProject->GetCompilerIndex());

    m_OutDir = ReplaceMSVCMacros(conf->Attribute("OutputDirectory"));
    bt->SetObjectOutput(ReplaceMSVCMacros(conf->Attribute("IntermediateDirectory")));
    
    wxString conftype = conf->Attribute("ConfigurationType");
    if (conftype.Matches("1"))
        bt->SetTargetType(ttConsoleOnly);
    else if (conftype.Matches("2"))
        bt->SetTargetType(ttDynamicLib);
    else if (conftype.Matches("3"))
        bt->SetTargetType(ttExecutable); // FIXME: is this correct???
    else if (conftype.Matches("4"))
        bt->SetTargetType(ttStaticLib);

    if (!m_ConvertSwitches)
        m_pProject->AddCompilerOption("/GX"); // add C++ exception handling (by default)

    TiXmlElement* tool = conf->FirstChildElement("Tool");
    if (!tool)
    {
        Manager::Get()->GetMessageManager()->DebugLog("No 'Tool' node...");
        return false;
    }

    while (tool)
    {
        if (strcmp(tool->Attribute("Name"), "VCLinkerTool") == 0 ||
            strcmp(tool->Attribute("Name"), "VCLibrarianTool") == 0)
        {
            // linker
            wxString tmp = ReplaceMSVCMacros(tool->Attribute("OutputFile"));
            tmp = UnixFilename(tmp);
            bt->SetOutputFilename(tmp);
            m_TargetPath = wxFileName(tmp).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            m_TargetFilename = wxFileName(tmp).GetFullName();

            wxString libs = tool->Attribute("AdditionalLibraryDirectories");
            wxArrayString arr = GetArrayFromString(libs, ";");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                bt->AddLibDir(ReplaceMSVCMacros(arr[i]));
            }

            if (!m_ConvertSwitches) // no point importing this option, if converting to GCC
            {
                libs = tool->Attribute("IgnoreDefaultLibraryNames");
                arr = GetArrayFromString(libs, ",");
                for (unsigned int i = 0; i < arr.GetCount(); ++i)
                {
                    bt->AddLinkerOption("/NODEFAULTLIB:" + arr[i]);
                }
            }

            tmp = tool->Attribute("GenerateDebugInformation");
            if (tmp.Matches("TRUE"))
            {
                bt->AddCompilerOption(m_ConvertSwitches ? "-g" : "/Zi");
                if (!m_ConvertSwitches)
                    bt->AddLinkerOption("/debug");
            }

            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("SuppressStartupBanner");
                if (tmp.Matches("TRUE"))
                    bt->AddLinkerOption("/nologo");
            }
            
            m_pProject->AddLinkerOption(ReplaceMSVCMacros(tool->Attribute("AdditionalOptions")));
            libs = ReplaceMSVCMacros(tool->Attribute("AdditionalDependencies"));
            arr = GetArrayFromString(libs, " ");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                tmp = arr[i];
                if (tmp.Right(4).CmpNoCase(".lib") == 0)
                    tmp.Remove(tmp.Length() - 4);
                bt->AddLinkLib(tmp);
            }
//            m_pProject->AddLinkLib(ReplaceMSVCMacros(tool->Attribute("ImportLibrary")));
            
            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("LinkIncremental");
                if (!tmp.Matches("0")) // is this correct???
                    bt->AddLinkerOption("/incremental");
            }
            
            if (!m_ConvertSwitches)
            {
                tmp = ReplaceMSVCMacros(tool->Attribute("ProgramDatabaseFile"));
                if (!tmp.IsEmpty())
                    bt->AddLinkerOption("/pdb:" + UnixFilename(tmp));
            }
        }
        else if (strcmp(tool->Attribute("Name"), "VCCLCompilerTool") == 0)
        {
            // compiler
            wxString incs = tool->Attribute("AdditionalIncludeDirectories");
            // vc70 uses ";" while vc71 uses "," separators
            wxString lsep;
            if (m_Version == vc70)
                lsep = ";";
            else
                lsep = ",";
            wxArrayString arr = GetArrayFromString(incs, lsep);
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                bt->AddIncludeDir(ReplaceMSVCMacros(arr[i]));
            }
            
            wxString defs = tool->Attribute("PreprocessorDefinitions");
            arr = GetArrayFromString(defs, ";");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                if (m_ConvertSwitches)
                    bt->AddCompilerOption("-D" + arr[i]);
                else
                    bt->AddCompilerOption("/D" + arr[i]);
            }
            
            wxString tmp = tool->Attribute("WarningLevel");
            if (m_ConvertSwitches)
            {
                if (tmp.Matches("0"))
                    bt->AddCompilerOption("-w");
                else if (tmp.Matches("1") || tmp.Matches("2") || tmp.Matches("3"))
                    bt->AddCompilerOption("-W");
                else if (tmp.Matches("4"))
                    bt->AddCompilerOption("-Wall");
            }
            else
            {
                m_pProject->AddCompilerOption("/W" + tmp);
            }

            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("Detect64BitPortabilityProblems");
                if (tmp.Matches("TRUE"))
                    bt->AddCompilerOption("/Wp64");

                tmp = tool->Attribute("MinimalRebuild");
                if (tmp.Matches("TRUE"))
                    bt->AddCompilerOption("/Gm");
            }

            tmp = tool->Attribute("RuntimeTypeInfo");
            if (tmp.Matches("TRUE"))
                bt->AddCompilerOption(m_ConvertSwitches ? "-frtti" : "/GR");

            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("SuppressStartupBanner");
                if (tmp.Matches("TRUE"))
                    bt->AddCompilerOption("/nologo");
            }
        }
        else if (strcmp(tool->Attribute("Name"), "VCPreBuildEventTool") == 0)
        {
            // pre-build step
            wxString cmd = ReplaceMSVCMacros(tool->Attribute("CommandLine"));
            if (!cmd.IsEmpty())
                bt->AddCommandsBeforeBuild(cmd);
        }
        else if (strcmp(tool->Attribute("Name"), "VCPostBuildEventTool") == 0)
        {
            // post-build step
            wxString cmd = ReplaceMSVCMacros(tool->Attribute("CommandLine"));
            if (!cmd.IsEmpty())
                bt->AddCommandsAfterBuild(cmd);
        }
        tool = tool->NextSiblingElement();
    }
    return true;
}

bool MSVC7Loader::DoImportFiles(TiXmlElement* root, int numConfigurations)
{
    if (!root)
        return false;

    TiXmlElement* files = root->FirstChildElement("Files");
    if (!files)
        files = root; // might not have "Files" section
    while (files)
    {
        TiXmlElement* file = files->FirstChildElement("File");
        while(file)
        {
            wxString fname = ReplaceMSVCMacros(file->Attribute("RelativePath"));
            if (!fname.IsEmpty())
            {
                ProjectFile* pf = m_pProject->AddFile(0, fname);
                if (pf)
                {
                    // add it to all configurations, not just the first
                    for (int i = 1; i < numConfigurations; ++i)
                        pf->AddBuildTarget(m_pProject->GetBuildTarget(i)->GetTitle());
                    HandleFileConfiguration(file, pf);
                }
            }
            file = file->NextSiblingElement("File");
        }

        // recurse for nested filters
        TiXmlElement* nested = files->FirstChildElement("Filter");
        while(nested)
        {
            DoImportFiles(nested, numConfigurations);
            nested = nested->NextSiblingElement("Filter");
        }

        files = files->NextSiblingElement("Files");
    }
    
    // recurse for nested filters
    TiXmlElement* nested = root->FirstChildElement("Filter");
    while(nested)
    {
        DoImportFiles(nested, numConfigurations);
        nested = nested->NextSiblingElement("Filter");
    }

    return true;
}

// function contributed by Tim Baker
void MSVC7Loader::HandleFileConfiguration(TiXmlElement* file, ProjectFile* pf)
{
    TiXmlElement* fconf = file->FirstChildElement("FileConfiguration");
    while (fconf)
    {
        if (const char* s = fconf->Attribute("ExcludedFromBuild"))
        {
            wxString exclude = s; // can you initialize wxString from NULL?
            if (exclude.Matches("TRUE"))
            {
                wxString name = fconf->Attribute("Name");
                int pos = name.Find('|');
                if (pos != wxNOT_FOUND)
                    name.Remove(pos);
                pf->RemoveBuildTarget(name);
                Manager::Get()->GetMessageManager()->DebugLog(
                    "removed %s from %s",
                    pf->file.GetFullPath().c_str(), name.c_str());
            }
        }
        fconf = fconf->NextSiblingElement("FileConfiguration");
    }
}
