#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "msvc7workspaceloader.h"

#include "globals.h"
#include "importers_globals.h"
#include "manager.h"
#include "messagemanager.h"
#include "projectmanager.h"
#include "compilerfactory.h"
#include "compiler.h"

#include <cassert>

MSVC7WorkspaceLoader::MSVC7WorkspaceLoader()
{
	//ctor
}

MSVC7WorkspaceLoader::~MSVC7WorkspaceLoader()
{
	//dtor
}

bool MSVC7WorkspaceLoader::Open(const wxString& filename)
{
    bool askForCompiler = false;
    bool askForTargets = false;
    switch (wxMessageBox(_("Do you want the imported projects to use the default compiler?\n"
                        "(If you answer No, you will be asked for each and every project"
                        " which compiler to use...)"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
    {
        case wxYES: askForCompiler = false; break;
        case wxNO: askForCompiler = true; break;
        case wxCANCEL: return false;
    }
    switch (wxMessageBox(_("Do you want to import all configurations (e.g. Debug/Release) from the "
                        "imported projects?\n"
                        "(If you answer No, you will be asked for each and every project"
                        " which configurations to import...)"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
    {
        case wxYES: askForTargets = false; break;
        case wxNO: askForTargets = true; break;
        case wxCANCEL: return false;
    }

    wxFileInputStream file(filename);
    if (!file.Ok())
        return false; // error opening file???

    wxArrayString comps;
    wxTextInputStream input(file);

    // read "header"
    if (!file.Eof())
    {
        wxString line = input.ReadLine();
        if (line.IsEmpty())
        {
            Manager::Get()->GetMessageManager()->DebugLog("Unsupported format.");
            return false;
        }
        comps = GetArrayFromString(line, ",");
        line = comps[0];
        line.Trim(true);
        line.Trim(false);
        if (line != "Microsoft Visual Studio Solution File")
        {
            Manager::Get()->GetMessageManager()->DebugLog("Unsupported format.");
            return false;
        }
        line = comps.GetCount() > 1 ? comps[1] : wxString(wxEmptyString);
        line.Trim(true);
        line.Trim(false);
        if (line != "Format Version 7.00")
            Manager::Get()->GetMessageManager()->DebugLog("Format not recognized. Will try to parse though...");
    }
    
    ImportersGlobals::UseDefaultCompiler = !askForCompiler;
    ImportersGlobals::ImportAllTargets = !askForTargets;

    int count = 0;
    cbProject* currentProject = 0;
    wxString currentIdcode = "";
    bool depSection = false;  // dependencies section
    bool globalDeps = false;  // global dependencies or project dependencies
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(true);
        line.Trim(false);

        if (line.StartsWith("Project(")) {
// example wanted line:
//Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "OgreMain", "OgreMain\scripts\OgreMain.vcproj", "{35AFBABB-DF05-43DE-91A7-BB828A874015}"
            wxArrayString keyvalue = GetArrayFromString(line, "=");
            if (keyvalue.GetCount() != 2)
                continue;
        
            // the second part contains the project title and filename
            comps = GetArrayFromString(keyvalue[1], ",");
            if (comps.GetCount() < 3)
                continue;
        
            // read project title and trim quotes
            wxString prjTitle = comps[0];
            prjTitle.Trim(true);
            prjTitle.Trim(false);
            if (prjTitle.IsEmpty())
                continue;
            if (prjTitle.GetChar(0) == '\"')
            {
                prjTitle.Truncate(prjTitle.Length() - 1);
                prjTitle.Remove(0, 1);
            }

            // read project filename and trim quotes
            wxString prjFile = comps[1];
            prjFile.Trim(true);
            prjFile.Trim(false);
            if (prjFile.IsEmpty())
                continue;
            if (prjFile.GetChar(0) == '\"')
            {
                prjFile.Truncate(prjFile.Length() - 1);
                prjFile.Remove(0, 1);
            }

            // read project idcode, i.e. "{35AFBABB-DF05-43DE-91A7-BB828A874015}"
            currentIdcode = comps[2];
            currentIdcode.Replace("\"", "");
            currentIdcode.Replace("{", "");
            currentIdcode.Replace("}", "");

            ++count;
            wxFileName wfname = filename;
            wxFileName fname = prjFile;
            fname.MakeAbsolute(wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            Manager::Get()->GetMessageManager()->DebugLog("Found project '%s' in '%s'", prjTitle.c_str(), fname.GetFullPath().c_str());
            currentProject = Manager::Get()->GetProjectManager()->LoadProject(fname.GetFullPath());
            if (currentProject) initDependencies(currentProject, currentIdcode);
        }
        else if (line.StartsWith("GlobalSection(ProjectDependencies)")) {
        	depSection = true;
        	globalDeps = true;
        }
        else if (line.StartsWith("ProjectSection(ProjectDependencies)")) {
        	depSection = true;
        	globalDeps = false;
        }
        else if (depSection && (line.StartsWith("EndGlobalSection") || line.StartsWith("EndProjectSection"))) {
        	depSection = false;
        }        
        else if (depSection && line.StartsWith("{")) { // start reading a dependency
        	wxArrayString idx = GetArrayFromString(line, "=");
        	if (idx.GetCount() != 2) continue;
        	if (globalDeps) {
// {31635C8-67BF-4808-A918-0FBF822771BD}.0 = {658BFA12-8417-49E5-872A-33F0973544DC}
// i.e. project on the left of '=' depend on the project on the right        		
                idx[0]= idx[0].BeforeFirst('}');
                idx[0].Replace("{", "");
                idx[1].Replace("{", "");
                idx[1].Replace("}", "");
                addDependency(idx[0], idx[1]);
            }
            else {
// {F87429BF-4583-4A67-BD6F-6CA8AA27702A} = {F87429BF-4583-4A67-BD6F-6CA8AA27702A}
// i.e. both idcodes are the dependency
                idx[1].Replace("{", "");
                idx[1].Replace("}", "");
                addDependency(currentIdcode, idx[1]);
            }
        }
    }

    resolveDependencies();
    ImportersGlobals::ResetDefaults();

    m_Title = wxFileName(filename).GetName() + _(" workspace");
    return count != 0;
}

bool MSVC7WorkspaceLoader::Save(const wxString& title, const wxString& filename)
{
    // no support for saving solution files (.sln)
    return false;
}

void MSVC7WorkspaceLoader::initDependencies(cbProject* project, const wxString& idcode) {
    // just set the initial project dependencies as empty and register the idcode
    _projdeps[idcode] = ProjDeps(project);
}

void MSVC7WorkspaceLoader::addDependency(const wxString& projIdcode, const wxString& depIdcode) {
    // add the dependency to the last project
    HashProjdeps::iterator it = _projdeps.find(projIdcode);
    if (it != _projdeps.end()) it->second._deps.Add(depIdcode);
}

void MSVC7WorkspaceLoader::resolveDependencies() {
    HashProjdeps::iterator pIt;
    HashProjdeps::iterator sIt;
    ProjectBuildTarget* target1;
    ProjectBuildTarget* target2;
    ProjDeps p;
    unsigned int i;
    int j;
    // assign dependencies to projects
    // quick hack: we add the library targets of the dependent projects to the current project only
    // a real project/target dependency feature should be implemented in the sdk
    for (pIt = _projdeps.begin(); pIt != _projdeps.end(); ++pIt) {
        p = pIt->second;
        for (i=0; i<p._deps.GetCount(); ++i) {
            sIt = _projdeps.find(p._deps[i]);
            if (sIt != _projdeps.end()) {
                assert(p._project->GetBuildTargetsCount() == sIt->second._project->GetBuildTargetsCount());
                for (j=0; j<p._project->GetBuildTargetsCount(); ++j) {
                    target1 = sIt->second._project->GetBuildTarget(j);
                    target2 = p._project->GetBuildTarget(j);
                    wxString deps = target2->GetExternalDeps();
                    deps <<target1->GetOutputFilename() << ';';
                    target2->SetExternalDeps(deps);
                    TargetType type = target1->GetTargetType();
                    if (type==ttDynamicLib) {
                        // target1->GetStaticLibFilename() do not work since it uses the filename instead of output filename
                        Compiler* compiler = CompilerFactory::Compilers[sIt->second._project->GetCompilerIndex()];
                        wxString prefix = compiler->GetSwitches().libPrefix;                        
                        wxString suffix = compiler->GetSwitches().libExtension;
                        wxFileName fname = target1->GetOutputFilename();
                        if (!fname.GetName().StartsWith(prefix)) fname.SetName(prefix + fname.GetName());
                        fname.SetExt(suffix);                        
                        target2->AddLinkLib(fname.GetFullPath());
                    }
                    else if (type==ttStaticLib) target2->AddLinkLib(target1->GetOutputFilename());
               }                    
            }
        }        
    }
    
    //target->AddCommandsBeforeBuild(const wxString& command);
}

