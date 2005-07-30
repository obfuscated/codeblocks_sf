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
#include "compilerfactory.h"
#include "compiler.h"

MSVC7Loader::MSVC7Loader(cbProject* project)
    : m_pProject(project),
    m_ConvertSwitches(false),
    m_Version(0)
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
    ret.Replace("$(IntDir)", m_IntDir); 
    ret.Replace("$(INTDIR)", m_IntDir);
    ret.Replace("$(ConfigurationName)", m_ConfigurationName);
    ret.Replace("$(ProjectName)", m_ProjectName);
    ret.Replace("$(TargetPath)", m_TargetPath);
    ret.Replace("$(TargetFileName)", m_TargetFilename);
    ret.Replace("\"", "");
    //ret.Replace("&quot;", "\"");

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

    wxString ver = root->Attribute("Version");
    if (ver.IsSameAs("7.0") || ver.IsSameAs("7.00")) m_Version = 70;
    if (ver.IsSameAs("7.1") || ver.IsSameAs("7.10")) m_Version = 71;
    if ((m_Version!=70) && (m_Version!=71))
    {
        // seems to work with visual 8 too ;)
        pMsg->DebugLog(_("Project version is '%s'. Although this loader was designed for version 7.xx, will try to import..."), ver.c_str());
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

    // See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vcext/html/vxlrfvcprojectenginelibraryruntimelibraryoption.asp


    m_OutDir = ReplaceMSVCMacros(conf->Attribute("OutputDirectory"));
    m_IntDir = ReplaceMSVCMacros(conf->Attribute("IntermediateDirectory"));
    if (m_IntDir.StartsWith(".\\")) m_IntDir.Remove(0,2);
    bt->SetObjectOutput(m_IntDir);
    
    // see MSDN: ConfigurationTypes Enumeration
    wxString conftype = conf->Attribute("ConfigurationType");
    if (conftype.IsSameAs("1")) // typeApplication 1, no difference between console or gui here, we must check the subsystem property of the linker
        bt->SetTargetType(ttExecutable);
    else if (conftype.IsSameAs("2")) // typeDynamicLibrary 2 
        bt->SetTargetType(ttDynamicLib);
    else if (conftype.IsSameAs("4")) // typeStaticLibrary 4 
        bt->SetTargetType(ttStaticLib);
    else if (conftype.IsSameAs("10")) // typeGeneric 10 
        bt->SetTargetType(ttCommandsOnly);
    else { // typeUnknown 0 
        bt->SetTargetType(ttCommandsOnly);
        Manager::Get()->GetMessageManager()->DebugLog("unrecognized project type");                
    }

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
            wxString tmp;
                        
            if (bt->GetTargetType()==ttExecutable) {
                tmp = tool->Attribute("SubSystem");
                //subSystemNotSet 0 
                //subSystemConsole 1 
                //subSystemWindows 2 
                if (tmp.IsSameAs("1")) {
                    bt->SetTargetType(ttConsoleOnly); 
                    //bt->AddLinkerOption("/SUBSYSTEM:CONSOLE"); // don't know if it is necessary
                }
            } // else we keep executable
    	
            tmp = ReplaceMSVCMacros(tool->Attribute("OutputFile"));
            tmp = UnixFilename(tmp);
            if (tmp.Last() == '.') tmp.RemoveLast();
            if (bt->GetTargetType() == ttStaticLib) {
                // convert the lib name
                Compiler* compiler = CompilerFactory::Compilers[m_pProject->GetCompilerIndex()];
                wxString prefix = compiler->GetSwitches().libPrefix;                        
                wxString suffix = compiler->GetSwitches().libExtension;
                wxFileName fname = tmp;
                if (!fname.GetName().StartsWith(prefix)) fname.SetName(prefix + fname.GetName());
                fname.SetExt(suffix);                        
                tmp = fname.GetFullPath();
            }
            bt->SetOutputFilename(tmp);
            m_TargetPath = wxFileName(tmp).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            m_TargetFilename = wxFileName(tmp).GetFullName();

            tmp = tool->Attribute("AdditionalLibraryDirectories");
            wxArrayString arr = GetArrayFromString(tmp, ";");
            if (arr.GetCount()==1) arr = GetArrayFromString(tmp, ",");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                bt->AddLibDir(ReplaceMSVCMacros(arr[i]));
            }

            if (!m_ConvertSwitches) // no point importing this option, if converting to GCC
            {
                tmp = tool->Attribute("IgnoreDefaultLibraryNames");
                arr = GetArrayFromString(tmp, ";");
                if (arr.GetCount()==1) arr = GetArrayFromString(tmp, ",");
                for (unsigned int i = 0; i < arr.GetCount(); ++i)
                {
                    bt->AddLinkerOption("/NODEFAULTLIB:" + arr[i]);
                }
            }

#if 0
            // no need since "/nologo" appear on the invocation commands of compilers/linkers
            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("SuppressStartupBanner");
                if (tmp.IsSameAs("TRUE"))
                    bt->AddLinkerOption("/nologo");
            }
#endif

            tmp = tool->Attribute("GenerateDebugInformation");
            if (tmp.IsSameAs("TRUE"))
            {
                //bt->AddCompilerOption(m_ConvertSwitches ? "-g" : "/Zi"); // no !
                if (!m_ConvertSwitches)
                    bt->AddLinkerOption("/debug");
            }
            
            // other options: /MACHINE:I386, /INCREMENTAL:YES, /STACK:10000000
            if (!m_ConvertSwitches) {
                arr = GetArrayFromString(ReplaceMSVCMacros(tool->Attribute("AdditionalOptions")), " ");
                for (unsigned int i = 0; i < arr.GetCount(); ++i) bt->AddLinkerOption(arr[i]);
            }
            // else ignore all options
            
            tmp = ReplaceMSVCMacros(tool->Attribute("AdditionalDependencies"));
            arr = GetArrayFromString(tmp, " ");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                tmp = arr[i];
                if (tmp.Right(4).CmpNoCase(".lib") == 0)
                    tmp.Remove(tmp.Length() - 4);
                bt->AddLinkLib(tmp);
            }
            
            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("LinkIncremental");
                if (tmp.IsSameAs("1")) // 1 -> no, default is yes
                    bt->AddLinkerOption("/INCREMENTAL:NO");
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
        	unsigned int i;
        	wxString tmp;
        	wxArrayString arr;
        	
            // compiler
            tmp = tool->Attribute("AdditionalIncludeDirectories");
            // vc70 uses ";" while vc71 uses "," separators
            // NOTE (mandrav#1#): No, that is *not* the case (what were they thinking at MS?)
            // try with comma (,) which is the newest I believe
            arr = GetArrayFromString(tmp, ",");
            if (arr.GetCount() == 1) // if it fails, try with semicolon
                arr = GetArrayFromString(tmp, ";");
            for (i = 0; i < arr.GetCount(); ++i)
            {
                bt->AddIncludeDir(ReplaceMSVCMacros(arr[i]));
            }
            
            tmp = tool->Attribute("PreprocessorDefinitions");
            arr = GetArrayFromString(tmp, ",");
            if (arr.GetCount() == 1) // if it fails, try with semicolon
                arr = GetArrayFromString(tmp, ";");
            for (unsigned int i = 0; i < arr.GetCount(); ++i)
            {
                if (m_ConvertSwitches)
                    bt->AddCompilerOption("-D" + arr[i]);
                else
                    bt->AddCompilerOption("/D" + arr[i]);
            }
            
            tmp = tool->Attribute("WarningLevel");
            if (m_ConvertSwitches)
            {
                if (tmp.IsSameAs("0"))
                    bt->AddCompilerOption("-w");
                else if (tmp.IsSameAs("1") || tmp.IsSameAs("2") || tmp.IsSameAs("3"))
                    bt->AddCompilerOption("-W");
                else if (tmp.IsSameAs("4"))
                    bt->AddCompilerOption("-Wall");
            }
            else
            {
                bt->AddCompilerOption("/W" + tmp);
            }

            tmp = tool->Attribute("DebugInformationFormat");
            if (tmp.IsSameAs("3"))
              bt->AddCompilerOption(m_ConvertSwitches ? "-g" : "/Zi"); // no !


            tmp = tool->Attribute("InlineFunctionExpansion");
            if (!m_ConvertSwitches && tmp.IsSameAs("1")) bt->AddCompilerOption("/Ob1");

            /* Optimization :
            optimizeDisabled 0 
            optimizeMinSpace 1 
            optimizeMaxSpeed 2 
            optimizeFull 3 
            optimizeCustom 4 
            */
            tmp = tool->Attribute("Optimization");
            if (m_ConvertSwitches) {
                if      (tmp.IsSameAs("0")) bt->AddCompilerOption("-O0");
                else if (tmp.IsSameAs("1")) bt->AddCompilerOption("-O1");
                else if (tmp.IsSameAs("2")) bt->AddCompilerOption("-O2");
                else if (tmp.IsSameAs("3")) bt->AddCompilerOption("-O3");
                //else if (tmp.IsSameAs("4")) bt->AddCompilerOption("-O1"); // nothing to do ?
            }
            else {
                if      (tmp.IsSameAs("0")) bt->AddCompilerOption("/Od");
                else if (tmp.IsSameAs("1")) bt->AddCompilerOption("/O1");
                else if (tmp.IsSameAs("2")) bt->AddCompilerOption("/O2");
                else if (tmp.IsSameAs("3")) bt->AddCompilerOption("/Ox");
                //else if (tmp.IsSameAs("4")) bt->AddCompilerOption("/O1"); // nothing to do ?
            }

            if (!m_ConvertSwitches)
            {
                tmp = tool->Attribute("Detect64BitPortabilityProblems");
                if (tmp.IsSameAs("TRUE"))
                    bt->AddCompilerOption("/Wp64");

                tmp = tool->Attribute("MinimalRebuild");
                if (tmp.IsSameAs("TRUE"))
                    bt->AddCompilerOption("/Gm");
                    
                /*
                RuntimeLibrary :
                rtMultiThreaded          0 --> /MT
                rtMultiThreadedDebug     1 --> /MTd
                rtMultiThreadedDLL       2 --> /MD 
                rtMultiThreadedDebugDLL  3 --> /MDd
                rtSingleThreaded         4 --> /ML
                rtSingleThreadedDebug    5 --> /MLd
                */
                tmp = tool->Attribute("RuntimeLibrary");                
                if      (tmp.IsSameAs("0")) bt->AddCompilerOption("/MT");
                else if (tmp.IsSameAs("1")) bt->AddCompilerOption("/MTd");
                else if (tmp.IsSameAs("2")) bt->AddCompilerOption("/MD");
                else if (tmp.IsSameAs("3")) bt->AddCompilerOption("/MDd");
                else if (tmp.IsSameAs("4")) bt->AddCompilerOption("/ML");
                else if (tmp.IsSameAs("5")) bt->AddCompilerOption("/MLd");                               

#if 0
                tmp = tool->Attribute("SuppressStartupBanner");
                if (tmp.IsSameAs("TRUE")) bt->AddCompilerOption("/nologo");
#endif

/*
runtimeBasicCheckNone 0 
runtimeCheckStackFrame 1  --> /RTCs or /GZ
runtimeCheckUninitVariables 2 
runtimeBasicCheckAll 3 
*/

                tmp = tool->Attribute("BasicRuntimeChecks");
                if (tmp.IsSameAs("1"))
                    bt->AddCompilerOption("/GZ");
  
                tmp = tool->Attribute("ExceptionHandling");
                if (tmp.IsSameAs("TRUE")) bt->AddCompilerOption("EHsc"); // add C++ exception handling

            }

            tmp = tool->Attribute("RuntimeTypeInfo");
            if (tmp.IsSameAs("TRUE"))
                bt->AddCompilerOption(m_ConvertSwitches ? "-frtti" : "/GR");

/*
AdditionalOptions=" /Zm1000 /GR  -DCMAKE_INTDIR=\&quot;Debug\&quot;"
ObjectFile="Debug\"
/Zm<n> max memory alloc (% of default)
*/
        	  tmp = tool->Attribute("AdditionalOptions");        	
          	//tmp = ReplaceMSVCMacros(tmp);
            arr = GetArrayFromString(tmp, " ");
            for (i=0; i<arr.GetCount(); ++i) {
              if (arr[i].IsSameAs("/D") || arr[i].IsSameAs("-D")) {
                  bt->AddCompilerOption((m_ConvertSwitches?"-D":"/D") + arr[i+1]);
                  ++i;
              }
              else if (arr[i].StartsWith("/D") || arr[i].StartsWith("-D"))
                	bt->AddCompilerOption((m_ConvertSwitches?"-D":"/D") + arr[i].Mid(2));
              else if (arr[i].IsSameAs("/Zi"))
                	bt->AddCompilerOption(m_ConvertSwitches?"-g":"/Zi");
              else if (!m_ConvertSwitches) 
                  bt->AddCompilerOption(arr[i]);
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
            if (exclude.IsSameAs("TRUE"))
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
