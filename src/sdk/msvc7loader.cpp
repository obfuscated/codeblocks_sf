#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include "manager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "globals.h"
#include "msvc7loader.h"

MSVC7Loader::MSVC7Loader(cbProject* project)
    : m_pProject(project),
    m_ConvertSwitches(true)
{
	//ctor
}

MSVC7Loader::~MSVC7Loader()
{
	//dtor
}

bool MSVC7Loader::Open(const wxString& filename)
{
    MessageManager* pMsg = Manager::Get()->GetMessageManager();
    if (!pMsg)
        return false;

	int conv = wxMessageBox(_("Do you want to convert compiler/linker command-line switches to GCC's switches?"),
                            _("Convert command-line switches?"),
                            wxICON_QUESTION | wxYES | wxNO);
    m_ConvertSwitches = conv == wxID_YES;

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
    if (strncmp(root->Attribute("Version"), "7", 1) != 0)
    {
        wxMessageBox(_("Project version is not 7.xx. It might not be imported correctly..."), _("Warning"), wxICON_WARNING);
    }

    m_pProject->ClearAllProperties();
    m_pProject->SetModified(true);
    m_pProject->SetTitle(root->Attribute("Name"));

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
    int size = configurations.GetCount();

    // ask the user to select a configuration
    wxString* names = new wxString[size];
    for (int i = 0; i < size; ++i)
    {
        names[i] = configurations[i];
    }
    wxSingleChoiceDialog dlg(0,
                            _("Select configuration to import"),
                            _("Import MSVC7 project"),
                            size,
                            names);
    if (dlg.ShowModal() == wxID_CANCEL)
    {
        Manager::Get()->GetMessageManager()->DebugLog("Cancelled...");
        return false;
    }
    
    // re-iterate configurations to find the selected one
    confs = config->FirstChildElement("Configuration");
    for (int i = 0; i < dlg.GetSelection(); ++i)
        confs = confs->NextSiblingElement();
    
    delete[] names;

    // parse the selected configuration
    return DoImport(confs) && DoImportFiles(root);
}

bool MSVC7Loader::DoImport(TiXmlElement* conf)
{
    ProjectBuildTarget* bt = m_pProject->GetBuildTarget(0);
    if (!bt)
        bt = m_pProject->AddBuildTarget(_("default"));

    wxString outdir = conf->Attribute("OutputDirectory");
    bt->SetObjectOutput(conf->Attribute("IntermediateDirectory"));
    
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
        bt->AddCompilerOption("/GX"); // add C++ exception handling (by default)

    TiXmlElement* tool = conf->FirstChildElement("Tool");
    if (!tool)
    {
        Manager::Get()->GetMessageManager()->DebugLog("No 'Tool' node...");
        return false;
    }

    while (tool)
    {
        if (strcmp(tool->Attribute("Name"), "VCCLCompilerTool") == 0)
        {
            // compiler
            wxString incs = tool->Attribute("AdditionalIncludeDirectories");
            wxArrayString arr = GetArrayFromString(incs, ";");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                m_pProject->AddIncludeDir(arr[i]);
            }
            
            wxString defs = tool->Attribute("PreprocessorDefinitions");
            arr = GetArrayFromString(defs, ";");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                if (m_ConvertSwitches)
                    m_pProject->AddCompilerOption("-D" + arr[i]);
                else
                    m_pProject->AddCompilerOption("/D" + arr[i]);
            }
            
            wxString tmp = tool->Attribute("WarningLevel");
            if (m_ConvertSwitches)
            {
                if (tmp.Matches("0"))
                    m_pProject->AddCompilerOption("-w");
                else if (tmp.Matches("1") || tmp.Matches("2") || tmp.Matches("3"))
                    m_pProject->AddCompilerOption("-W");
                else if (tmp.Matches("4"))
                    m_pProject->AddCompilerOption("-Wall");
            }
            else
            {
                m_pProject->AddCompilerOption("/W" + tmp);
            }

            if (m_ConvertSwitches)
            {
                tmp = tool->Attribute("Detect64BitPortabilityProblems");
                if (tmp.Matches("TRUE"))
                    m_pProject->AddCompilerOption("/Wp64");

                tmp = tool->Attribute("MinimalRebuild");
                if (tmp.Matches("TRUE"))
                    m_pProject->AddCompilerOption("/Gm");

            }

            tmp = tool->Attribute("RuntimeTypeInfo");
            if (tmp.Matches("TRUE"))
                m_pProject->AddCompilerOption(m_ConvertSwitches ? "-frtti" : "/GR");

            if (m_ConvertSwitches)
            {
                tmp = tool->Attribute("SuppressStartupBanner");
                if (tmp.Matches("TRUE"))
                    m_pProject->AddCompilerOption("/nologo");
            }
        }
        else if (strcmp(tool->Attribute("Name"), "VCLinkerTool") == 0 ||
                 strcmp(tool->Attribute("Name"), "VCLibrarianTool") == 0)
        {
            // linker
            wxString libs = tool->Attribute("AdditionalLibraryDirectories");
            wxArrayString arr = GetArrayFromString(libs, ";");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                m_pProject->AddLibDir(arr[i]);
            }

            wxString tmp = tool->Attribute("GenerateDebugInformation");
            if (tmp.Matches("TRUE"))
                m_pProject->AddCompilerOption(m_ConvertSwitches ? "-g" : "/Zi");

            if (m_ConvertSwitches)
            {
                tmp = tool->Attribute("SuppressStartupBanner");
                if (tmp.Matches("TRUE"))
                    m_pProject->AddLinkerOption("/nologo");
            }

            tmp = tool->Attribute("OutputFile");
            tmp.Replace("$(OutDir)", outdir);
            bt->SetOutputFilename(UnixFilename(tmp));
            
            bt->AddLinkerOption(tool->Attribute("AdditionalOptions"));
            bt->AddLinkerOption(tool->Attribute("AdditionalDependencies"));
            bt->AddLinkerOption(tool->Attribute("ImportLibrary"));
        }
        
        tool = tool->NextSiblingElement();
    }
    return true;
}

bool MSVC7Loader::DoImportFiles(TiXmlElement* root)
{
    TiXmlElement* files = root->FirstChildElement("Files");
    if (!files)
    {
        Manager::Get()->GetMessageManager()->DebugLog("No 'Files' node...");
        return false;
    }

    TiXmlElement* filter = files->FirstChildElement("Filter");
    if (!filter)
    {
        Manager::Get()->GetMessageManager()->DebugLog("No 'Filter' node...");
        return false;
    }

    while(filter)
    {
        TiXmlElement* file = filter->FirstChildElement("File");
        while(file)
        {
            wxString fname = file->Attribute("RelativePath");
            if (!fname.IsEmpty())
                m_pProject->AddFile(0, fname);
            file = file->NextSiblingElement();
        }
        filter = filter->NextSiblingElement();
    }
    return true;
}
