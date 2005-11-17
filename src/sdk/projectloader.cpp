#include "sdk_precomp.h"
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "macrosmanager.h"
#include "cbproject.h"
#include "projectloader.h"
#include "compilerfactory.h"
#include "globals.h"
#include "customvars.h"

ProjectLoader::ProjectLoader(cbProject* project)
    : m_pProject(project),
    m_Upgraded(false),
    m_OpenDirty(false)
{
	//ctor
}

ProjectLoader::~ProjectLoader()
{
	//dtor
}

bool ProjectLoader::Open(const wxString& filename)
{
    MessageManager* pMsg = Manager::Get()->GetMessageManager();
    if (!pMsg)
        return false;

    wxStopWatch sw;
    pMsg->DebugLog(_("Loading project file..."));
    TiXmlDocument doc(filename.mb_str());
    if (!doc.LoadFile())
        return false;

    pMsg->DebugLog(_("Parsing project file..."));
    TiXmlElement* root;
    TiXmlElement* proj;

    root = doc.FirstChildElement("CodeBlocks_project_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_project_file");
        if (!root)
        {
            pMsg->DebugLog(_("Not a valid Code::Blocks project file..."));
            return false;
        }
    }
    proj = root->FirstChildElement("Project");
    if (!proj)
    {
        pMsg->DebugLog(_("No 'Project' element in file..."));
        return false;
    }

    DoProjectOptions(proj);
    DoBuild(proj);
    DoCompilerOptions(proj);
    DoResourceCompilerOptions(proj);
    DoLinkerOptions(proj);
    DoIncludesOptions(proj);
    DoLibsOptions(proj);
    DoExtraCommands(proj);
    DoUnits(proj);

    TiXmlElement* version = root->FirstChildElement("FileVersion");
    if (!version)
    {
        // pre 1.1 version
        ConvertVersion_Pre_1_1();
        // format changed also:
        // removed <IncludeDirs> and <LibDirs> elements and added them as child elements
        // in <Compiler> and <Linker> elements respectively
        // so set m_Upgraded to true, irrespectively of libs detection...
        m_Upgraded = true;
    }
    else
    {
        // do something important based on version
//        wxString major = version->Attribute("major");
//        wxString minor = version->Attribute("minor");
    }

    pMsg->DebugLog(wxString(_T("Done loading project in ")) << sw.Time() << _T("ms"));
    return true;
}

void ProjectLoader::ConvertVersion_Pre_1_1()
{
    // ask to detect linker libraries and move them to the new
    // CompileOptionsBase linker libs container
    wxString msg;
    msg.Printf(_("Project \"%s\" was saved with an earlier version of Code::Blocks.\n"
                "In the current version, link libraries are treated separately from linker options.\n"
                "Do you want to auto-detect the libraries \"%s\" is using and configure it accordingly?"),
                m_pProject->GetTitle().c_str(),
                m_pProject->GetTitle().c_str());
    if (wxMessageBox(msg, _("Question"), wxICON_QUESTION | wxYES_NO) == wxYES)
    {
        // project first
        ConvertLibraries(m_pProject);

        for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
        {
            ConvertLibraries(m_pProject->GetBuildTarget(i));
            m_Upgraded = true;
        }
    }
}

void ProjectLoader::ConvertLibraries(CompileTargetBase* object)
{
    wxArrayString linkerOpts = object->GetLinkerOptions();
    wxArrayString linkLibs = object->GetLinkLibs();

    int compilerIdx = object->GetCompilerIndex();
    Compiler* compiler = CompilerFactory::Compilers[compilerIdx];
    wxString linkLib = compiler->GetSwitches().linkLibs;
    wxString libExt = compiler->GetSwitches().libExtension;
    size_t libExtLen = libExt.Length();

    size_t i = 0;
    while (i < linkerOpts.GetCount())
    {
        wxString opt = linkerOpts[i];
        if (!linkLib.IsEmpty() && opt.StartsWith(linkLib))
        {
            opt.Remove(0, 2);
            wxString ext = compiler->GetSwitches().libExtension;
            if (!ext.IsEmpty())
                ext = _T(".") + ext;
            linkLibs.Add(compiler->GetSwitches().libPrefix + opt + ext);
            linkerOpts.RemoveAt(i, 1);
        }
        else if (opt.Length() > libExtLen && opt.Right(libExtLen) == libExt)
        {
            linkLibs.Add(opt);
            linkerOpts.RemoveAt(i, 1);
        }
        else
            ++i;
    }

    object->SetLinkerOptions(linkerOpts);
    object->SetLinkLibs(linkLibs);
}

void ProjectLoader::DoProjectOptions(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    if (!node)
        return; // no options

    wxString title;
    wxString makefile;
    bool makefile_custom = false;
    int defaultTarget = 0;
    int activeTarget = -1;
    int compilerIdx = 0;
    PCHMode pch_mode = pchSourceDir;

    // loop through all options
    while (node)
    {
        if (node->Attribute("title"))
            title = _U(node->Attribute("title"));

        else if (node->Attribute("makefile")) // there is only one attribute per option, so "else" is a safe optimisation
            makefile = _U(node->Attribute("makefile"));

        else if (node->Attribute("makefile_is_custom"))
            makefile_custom = strncmp(node->Attribute("makefile_is_custom"), "1", 1) == 0;

        else if (node->Attribute("default_target"))
            defaultTarget = atoi(node->Attribute("default_target"));

        else if (node->Attribute("active_target"))
            activeTarget = atoi(node->Attribute("active_target"));

        else if (node->Attribute("compiler"))
            compilerIdx = GetValidCompilerIndex(atoi(node->Attribute("compiler")), _("project"));

        else if (node->Attribute("pch_mode"))
            pch_mode = (PCHMode)atoi(node->Attribute("pch_mode"));

        node = node->NextSiblingElement("Option");
    }

    m_pProject->SetTitle(title);
    m_pProject->SetMakefile(makefile);
    m_pProject->SetMakefileCustom(makefile_custom);
    m_pProject->SetDefaultExecuteTargetIndex(defaultTarget);
    m_pProject->SetActiveBuildTarget(activeTarget);
    m_pProject->SetCompilerIndex(compilerIdx);
    m_pProject->SetModeForPCH(pch_mode);
}

void ProjectLoader::DoBuild(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Build");
    while (node)
    {
        DoBuildTarget(node);
        DoEnvironment(node, m_pProject);
        node = node->NextSiblingElement("Build");
    }
}

void ProjectLoader::DoBuildTarget(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Target");
    if (!node)
        return; // no options

    while (node)
    {
        ProjectBuildTarget* target = 0L;
        wxString title = _U(node->Attribute("title"));
        if (!title.IsEmpty())
            target = m_pProject->AddBuildTarget(title);

        if (target)
        {
            Manager::Get()->GetMessageManager()->DebugLog(_("Loading target %s"), title.c_str());
            DoBuildTargetOptions(node, target);
            DoCompilerOptions(node, target);
            DoResourceCompilerOptions(node, target);
            DoLinkerOptions(node, target);
            DoIncludesOptions(node, target);
            DoLibsOptions(node, target);
            DoExtraCommands(node, target);
            DoEnvironment(node, target);
        }

        node = node->NextSiblingElement("Target");
    }
}

void ProjectLoader::DoBuildTargetOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    if (!node)
        return; // no options

    bool use_console_runner = true;
    wxString output;
    wxString working_dir;
    wxString obj_output;
    wxString deps_output;
    wxString deps;
    wxString added;
    int type = -1;
    int compilerIdx = m_pProject->GetCompilerIndex();
    wxString parameters;
    wxString hostApplication;
    bool includeInTargetAll = true;
    bool createStaticLib = false;
    bool createDefFile = false;
    int projectCompilerOptionsRelation = 3;
    int projectLinkerOptionsRelation = 3;
    int projectIncludeDirsRelation = 3;
    int projectLibDirsRelation = 3;

    while (node)
    {
        if (node->Attribute("use_console_runner"))
            use_console_runner = strncmp(node->Attribute("use_console_runner"), "0", 1) != 0;

        else if (node->Attribute("output"))
            output = _U(node->Attribute("output"));

        else if (node->Attribute("working_dir"))
            working_dir = _U(node->Attribute("working_dir"));

        else if (node->Attribute("object_output"))
            obj_output = _U(node->Attribute("object_output"));

        else if (node->Attribute("deps_output"))
            deps_output = _U(node->Attribute("deps_output"));

        else if (node->Attribute("external_deps"))
            deps = _U(node->Attribute("external_deps"));

        else if (node->Attribute("additional_output"))
            added = _U(node->Attribute("additional_output"));

        else if (node->Attribute("type"))
            type = atoi(node->Attribute("type"));

        else if (node->Attribute("compiler"))
            compilerIdx = GetValidCompilerIndex(atoi(node->Attribute("compiler")), _("build target"));

        else if (node->Attribute("parameters"))
            parameters = _U(node->Attribute("parameters"));

        else if (node->Attribute("host_application"))
            hostApplication = _U(node->Attribute("host_application"));

        else if (node->Attribute("includeInTargetAll"))
            includeInTargetAll = atoi(node->Attribute("includeInTargetAll")) != 0;

        else if (node->Attribute("createDefFile"))
            createDefFile = atoi(node->Attribute("createDefFile")) != 0;

        else if (node->Attribute("createStaticLib"))
            createStaticLib = atoi(node->Attribute("createStaticLib")) != 0;

        else if (node->Attribute("projectCompilerOptionsRelation"))
            projectCompilerOptionsRelation = atoi(node->Attribute("projectCompilerOptionsRelation"));

        else if (node->Attribute("projectLinkerOptionsRelation"))
            projectLinkerOptionsRelation = atoi(node->Attribute("projectLinkerOptionsRelation"));

        else if (node->Attribute("projectIncludeDirsRelation"))
            projectIncludeDirsRelation = atoi(node->Attribute("projectIncludeDirsRelation"));

        else if (node->Attribute("projectLibDirsRelation"))
            projectLibDirsRelation = atoi(node->Attribute("projectLibDirsRelation"));

        node = node->NextSiblingElement("Option");
    }

    if (type != -1)
    {
        target->SetTargetType((TargetType)type); // type *must* come before output filename!
        target->SetOutputFilename(output); // because if no filename defined, one will be suggested based on target type...
        target->SetUseConsoleRunner(use_console_runner);
        if (!working_dir.IsEmpty())
            target->SetWorkingDir(working_dir);
        if (!obj_output.IsEmpty())
            target->SetObjectOutput(obj_output);
        if (!deps_output.IsEmpty())
            target->SetDepsOutput(deps_output);
        target->SetExternalDeps(deps);
        target->SetAdditionalOutputFiles(added);
        target->SetCompilerIndex(compilerIdx);
        target->SetExecutionParameters(parameters);
        target->SetHostApplication(hostApplication);
        target->SetIncludeInTargetAll(includeInTargetAll);
        target->SetCreateDefFile(createDefFile);
        target->SetCreateStaticLib(createStaticLib);
        target->SetOptionRelation(ortCompilerOptions, (OptionsRelation)projectCompilerOptionsRelation);
        target->SetOptionRelation(ortLinkerOptions, (OptionsRelation)projectLinkerOptionsRelation);
        target->SetOptionRelation(ortIncludeDirs, (OptionsRelation)projectIncludeDirsRelation);
        target->SetOptionRelation(ortLibDirs, (OptionsRelation)projectLibDirsRelation);
    }
}

void ProjectLoader::DoCompilerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("Compiler");
    if (!node)
        return; // no options

    TiXmlElement* child = node->FirstChildElement("Add");
    while (child)
    {
        wxString option = _U(child->Attribute("option"));
        wxString dir = _U(child->Attribute("directory"));
        if (!option.IsEmpty())
        {
            if (target)
                target->AddCompilerOption(option);
            else
                m_pProject->AddCompilerOption(option);
        }
        if (!dir.IsEmpty())
        {
            if (target)
                target->AddIncludeDir(dir);
            else
                m_pProject->AddIncludeDir(dir);
        }

        child = child->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoResourceCompilerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("ResourceCompiler");
    if (!node)
        return; // no options

    TiXmlElement* child = node->FirstChildElement("Add");
    while (child)
    {
        wxString dir = _U(child->Attribute("directory"));
        if (!dir.IsEmpty())
        {
            if (target)
                target->AddResourceIncludeDir(dir);
            else
                m_pProject->AddResourceIncludeDir(dir);
        }

        child = child->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoLinkerOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("Linker");
    if (!node)
        return; // no options

    TiXmlElement* child = node->FirstChildElement("Add");
    while (child)
    {
        wxString option = _U(child->Attribute("option"));
        wxString dir = _U(child->Attribute("directory"));
        wxString lib = _U(child->Attribute("library"));
        if (!option.IsEmpty())
        {
            if (target)
                target->AddLinkerOption(option);
            else
                m_pProject->AddLinkerOption(option);
        }
        if (!lib.IsEmpty())
        {
            if (target)
                target->AddLinkLib(lib);
            else
                m_pProject->AddLinkLib(lib);
        }
        if (!dir.IsEmpty())
        {
            if (target)
                target->AddLibDir(dir);
            else
                m_pProject->AddLibDir(dir);
        }

        child = child->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoIncludesOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("IncludeDirs");
    if (!node)
        return; // no options

    TiXmlElement* child = node->FirstChildElement("Add");
    while (child)
    {
        wxString option = _U(child->Attribute("option"));
        if (!option.IsEmpty())
        {
            if (target)
                target->AddIncludeDir(option);
            else
                m_pProject->AddIncludeDir(option);
        }

        child = child->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoLibsOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("LibDirs");
    if (!node)
        return; // no options

    TiXmlElement* child = node->FirstChildElement("Add");
    while (child)
    {
        wxString option = _U(child->Attribute("option"));
        if (!option.IsEmpty())
        {
            if (target)
                target->AddLibDir(option);
            else
                m_pProject->AddLibDir(option);
        }

        child = child->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoExtraCommands(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("ExtraCommands");
    while (node)
    {
        CompileOptionsBase* base = target ? target : (CompileOptionsBase*)m_pProject;
        TiXmlElement* child = node->FirstChildElement("Mode");
        while (child)
        {
            wxString mode = _U(child->Attribute("before"));
            if (mode == _T("always"))
                base->SetAlwaysRunPreBuildSteps(true);
            mode = _U(child->Attribute("after"));
            if (mode == _T("always"))
                base->SetAlwaysRunPostBuildSteps(true);

            child = child->NextSiblingElement("Mode");
        }

        child = node->FirstChildElement("Add");
        while (child)
        {
            wxString before;
            wxString after;

            if (child->Attribute("before"))
                before = _U(child->Attribute("before"));
            if (child->Attribute("after"))
                after = _U(child->Attribute("after"));

            if (!before.IsEmpty())
                base->AddCommandsBeforeBuild(before);
            if (!after.IsEmpty())
                base->AddCommandsAfterBuild(after);

            child = child->NextSiblingElement("Add");
        }
        node = node->NextSiblingElement("ExtraCommands");
    }
}

void ProjectLoader::DoEnvironment(TiXmlElement* parentNode, CompileOptionsBase* base)
{
	if (!base)
        return;
    CustomVars& vars = base->GetCustomVars();

    TiXmlElement* node = parentNode->FirstChildElement("Environment");
    while (node)
    {
        TiXmlElement* child = node->FirstChildElement("Variable");
        while (child)
        {
            wxString name = _U(child->Attribute("name"));
            wxString value = _U(child->Attribute("value"));
            if (!name.IsEmpty())
            	vars.Add(name, value);

            child = child->NextSiblingElement("Variable");
        }
        node = node->NextSiblingElement("Environment");
    }
}

void ProjectLoader::DoUnits(TiXmlElement* parentNode)
{
    Manager::Get()->GetMessageManager()->DebugLog(_U("Loading project files..."));
    TiXmlElement* unit = parentNode->FirstChildElement("Unit");
    while (unit)
    {
        wxString filename = _U(unit->Attribute("filename"));
        if (!filename.IsEmpty())
        {
            ProjectFile* file = m_pProject->AddFile(-1, filename);
            if (!file)
                Manager::Get()->GetMessageManager()->DebugLog(_("Can't load file '%s'"), filename.c_str());
            else
                DoUnitOptions(unit, file);
        }

        unit = unit->NextSiblingElement("Unit");
    }
}

void ProjectLoader::DoUnitOptions(TiXmlElement* parentNode, ProjectFile* file)
{
    int tempval = 0;
    bool foundCompile = false;
    bool foundLink = false;
    bool foundCompilerVar = false;

    TiXmlElement* node = parentNode->FirstChildElement("Option");
    while (node)
    {
        if (node->Attribute("compilerVar"))
        {
            file->compilerVar = _U(node->Attribute("compilerVar"));
            foundCompilerVar = true;
        }
        //
        if (node->QueryIntAttribute("compile", &tempval) == TIXML_SUCCESS)
        {
            file->compile = tempval != 0;
            foundCompile = true;
        }
        //
        if (node->QueryIntAttribute("link", &tempval) == TIXML_SUCCESS)
        {
            file->link = tempval != 0;
            foundLink = true;
        }
        //
        if (node->QueryIntAttribute("weight", &tempval) == TIXML_SUCCESS)
            file->weight = tempval;
        //
        if (node->QueryIntAttribute("useBuildCommand", &tempval) == TIXML_SUCCESS)
            file->useCustomBuildCommand = tempval != 0;
        //
        if (node->Attribute("buildCommand"))
        {
            wxString tmp = _U(node->Attribute("buildCommand"));
            if (!tmp.IsEmpty())
            {
                tmp.Replace(_T("\\n"), _T("\n"));
                file->buildCommand = tmp;
            }
        }
        //
        if (node->QueryIntAttribute("autoDeps", &tempval) == TIXML_SUCCESS)
            file->autoDeps = tempval != 0;
        //
        if (node->Attribute("customDeps"))
        {
            wxString tmp = _U(node->Attribute("customDeps"));
            if (!tmp.IsEmpty())
            {
                tmp.Replace(_T("\\n"), _T("\n"));
                file->customDeps = tmp;
            }
        }
        //
        if (node->Attribute("objectName"))
        {
            wxFileName objName(_U(node->Attribute("objectName")));
            FileType ft = FileTypeOf(file->relativeFilename);
            if (ft != ftResource && ft != ftResourceBin)
            {
                if (objName.GetExt() != CompilerFactory::Compilers[m_pProject->GetCompilerIndex()]->GetSwitches().objectExtension)
                    file->SetObjName(file->relativeFilename);
            }
        }
        //
        if (node->Attribute("target"))
            file->AddBuildTarget(_U(node->Attribute("target")));

        node = node->NextSiblingElement("Option");
    }

    // make sure the "compile" and "link" flags are honored
    if (!foundCompile)
        file->compile = true;
    if (!foundLink)
        file->link = true;
    if (!foundCompilerVar)
        file->compilerVar = _T("CPP");
}

bool ProjectLoader::Save(const wxString& filename)
{
    wxString buffer;
    wxArrayString array;
    CustomVars* vars = 0;

    buffer << _T("<?xml version=\"1.0\"?>") << _T('\n');
    buffer << _T("<!DOCTYPE CodeBlocks_project_file>") << _T('\n');
    buffer << _T("<CodeBlocks_project_file>") << _T('\n');
    buffer << _T('\t') << _T("<FileVersion major=\"") << PROJECT_FILE_VERSION_MAJOR << _T("\" minor=\"") << PROJECT_FILE_VERSION_MINOR << _T("\"/>") << _T('\n');
    buffer << _T('\t') << _T("<Project>") << _T('\n');
    buffer << _T('\t') << _T('\t') << _T("<Option title=\"") << FixEntities(m_pProject->GetTitle()) << _T("\"/>") << _T('\n');
    buffer << _T('\t') << _T('\t') << _T("<Option makefile=\"") << FixEntities(m_pProject->GetMakefile()) << _T("\"/>") << _T('\n');
    buffer << _T('\t') << _T('\t') << _T("<Option makefile_is_custom=\"") << m_pProject->IsMakefileCustom() << _T("\"/>") << _T('\n');
    if (m_pProject->GetModeForPCH() != pchSourceDir)
        buffer << _T('\t') << _T('\t') << _T("<Option pch_mode=\"") << (int)m_pProject->GetModeForPCH() << _T("\"/>") << _T('\n');
    if (m_pProject->GetDefaultExecuteTargetIndex() != 0)
        buffer << _T('\t') << _T('\t') << _T("<Option default_target=\"") << m_pProject->GetDefaultExecuteTargetIndex() << _T("\"/>") << _T('\n');
    if (m_pProject->GetActiveBuildTarget() != -1)
        buffer << _T('\t') << _T('\t') << _T("<Option active_target=\"") << m_pProject->GetActiveBuildTarget() << _T("\"/>") << _T('\n');
    buffer << _T('\t') << _T('\t') << _T("<Option compiler=\"") << m_pProject->GetCompilerIndex() << _T("\"/>") << _T('\n');

    buffer << _T('\t') << _T('\t') << _T("<Build>") << _T('\n');
    for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
        if (!target)
            break;

        buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Target title=\"") << FixEntities(target->GetTitle()) << _T("\">") << _T('\n');
        if (target->GetTargetType() != ttCommandsOnly)
        {
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option output=\"") << FixEntities(target->GetOutputFilename()) << _T("\"/>") << _T('\n');
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option working_dir=\"") << FixEntities(target->GetWorkingDir()) << _T("\"/>") << _T('\n');
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option object_output=\"") << FixEntities(target->GetObjectOutput()) << _T("\"/>") << _T('\n');
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option deps_output=\"") << FixEntities(target->GetDepsOutput()) << _T("\"/>") << _T('\n');
        }
        if (!target->GetExternalDeps().IsEmpty())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option external_deps=\"") << FixEntities(target->GetExternalDeps()) << _T("\"/>") << _T('\n');
        if (!target->GetAdditionalOutputFiles().IsEmpty())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option additional_output=\"") << FixEntities(target->GetAdditionalOutputFiles()) << _T("\"/>") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option type=\"") << target->GetTargetType() << _T("\"/>") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option compiler=\"") << target->GetCompilerIndex() << _T("\"/>") << _T('\n');
        if (target->GetTargetType() == ttConsoleOnly && !target->GetUseConsoleRunner())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option use_console_runner=\"0\"/>") << _T('\n');
        if (!target->GetExecutionParameters().IsEmpty())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option parameters=\"") << FixEntities(target->GetExecutionParameters()) << _T("\"/>") << _T('\n');
        if (!target->GetHostApplication().IsEmpty())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option host_application=\"") << FixEntities(target->GetHostApplication()) << _T("\"/>") << _T('\n');
        if (!target->GetIncludeInTargetAll())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option includeInTargetAll=\"0\"/>") << _T('\n');
        if ((target->GetTargetType() == ttStaticLib || target->GetTargetType() == ttDynamicLib) && target->GetCreateDefFile())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option createDefFile=\"1\"/>") << _T('\n');
        if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option createStaticLib=\"1\"/>") << _T('\n');
        if (target->GetOptionRelation(ortCompilerOptions) != 3) // 3 is the default
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option projectCompilerOptionsRelation=\"") << target->GetOptionRelation(ortCompilerOptions) << _T("\"/>") << _T('\n');
        if (target->GetOptionRelation(ortLinkerOptions) != 3) // 3 is the default
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option projectLinkerOptionsRelation=\"") << target->GetOptionRelation(ortLinkerOptions) << _T("\"/>") << _T('\n');
        if (target->GetOptionRelation(ortIncludeDirs) != 3) // 3 is the default
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option projectIncludeDirsRelation=\"") << target->GetOptionRelation(ortIncludeDirs) << _T("\"/>") << _T('\n');
        if (target->GetOptionRelation(ortResDirs) != 3) // 3 is the default
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option projectResourceIncludeDirsRelation=\"") << target->GetOptionRelation(ortResDirs) << _T("\"/>") << _T('\n');
        if (target->GetOptionRelation(ortLibDirs) != 3) // 3 is the default
            buffer << _T('\t') << _T('\t') << _T('\t') << _T('\t') << _T("<Option projectLibDirsRelation=\"") << target->GetOptionRelation(ortLibDirs) << _T("\"/>") << _T('\n');
        SaveCompilerOptions(buffer, target, 4);
        SaveResourceCompilerOptions(buffer, target, 4);
        SaveLinkerOptions(buffer, target, 4);
        SaveOptions(buffer, target->GetCommandsBeforeBuild(), _T("ExtraCommands"), 4, _T("before"), target->GetAlwaysRunPreBuildSteps() ? _T("<Mode before=\"always\" />") : _T(""));
        SaveOptions(buffer, target->GetCommandsAfterBuild(), _T("ExtraCommands"), 4, _T("after"), target->GetAlwaysRunPostBuildSteps() ? _T("<Mode after=\"always\" />") : _T(""));

        vars = &target->GetCustomVars();
        SaveEnvironment(buffer, vars, 4);

        buffer << _T('\t') << _T('\t') << _T('\t') << _T("</Target>") << _T('\n');
    }
    vars = &m_pProject->GetCustomVars();
    SaveEnvironment(buffer, vars, 3);
    buffer << _T('\t') << _T('\t') << _T("</Build>") << _T('\n');

    SaveCompilerOptions(buffer, m_pProject, 2);
    SaveResourceCompilerOptions(buffer, m_pProject, 2);
    SaveLinkerOptions(buffer, m_pProject, 2);
    SaveOptions(buffer, m_pProject->GetCommandsBeforeBuild(), wxString(_T("ExtraCommands")), 2, _T("before"), m_pProject->GetAlwaysRunPreBuildSteps() ? _T("<Mode before=\"always\" />") : _T(""));
    SaveOptions(buffer, m_pProject->GetCommandsAfterBuild(), wxString(_T("ExtraCommands")), 2, _T("after"), m_pProject->GetAlwaysRunPostBuildSteps() ? _T("<Mode after=\"always\" />") : _T(""));

    int count = m_pProject->GetFilesCount();
    for (int i = 0; i < count; ++i)
    {
        ProjectFile* f = m_pProject->GetFile(i);
        buffer << _T('\t') << _T('\t') << _T("<Unit filename=\"") << FixEntities(f->relativeFilename) << _T("\">") << _T('\n');
        buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option compilerVar=\"") << FixEntities(f->compilerVar) << _T("\"/>") << _T('\n');
        if (!f->compile)
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option compile=\"0\"/>") << _T('\n');
        if (!f->link)
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option link=\"0\"/>") << _T('\n');
        if (f->weight != 50)
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option weight=\"") << f->weight << _T("\"/>") << _T('\n');
        if (f->useCustomBuildCommand)
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option useBuildCommand=\"1\"/>") << _T('\n');
        if (!f->buildCommand.IsEmpty())
        {
            f->buildCommand.Replace(_T("\n"), _T("\\n"));
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option buildCommand=\"") << FixEntities(f->buildCommand) << _T("\"/>") << _T('\n');
        }
        if (!f->autoDeps)
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option autoDeps=\"0\"/>") << _T('\n');
        if (!f->customDeps.IsEmpty())
        {
            f->customDeps.Replace(_T("\n"), _T("\\n"));
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option customDeps=\"") << FixEntities(f->customDeps) << _T("\"/>") << _T('\n');
        }
        if (!f->GetObjName().IsEmpty())
        {
            wxFileName tmp(f->GetObjName());
            if (FileTypeOf(f->relativeFilename) != ftHeader &&
                tmp.GetExt() != CompilerFactory::Compilers[m_pProject->GetCompilerIndex()]->GetSwitches().objectExtension)
            {
                buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option objectName=\"") << FixEntities(f->GetObjName()) << _T("\"/>") << _T('\n');
            }
        }
        for (unsigned int x = 0; x < f->buildTargets.GetCount(); ++x)
            buffer << _T('\t') << _T('\t') << _T('\t') << _T("<Option target=\"") << FixEntities(f->buildTargets[x]) << _T("\"/>") << _T('\n');
		buffer << _T('\t') << _T('\t') << _T("</Unit>") << _T('\n');
    }

    buffer << _T('\t') << _T("</Project>") << _T('\n');
    buffer << _T("</CodeBlocks_project_file>") << _T('\n');

    wxFile file(filename, wxFile::write);
    if (cbWrite(file,buffer))
    {
		m_pProject->SetModified(false);
        return true;
    }
    return false;
}

void ProjectLoader::SaveCompilerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs)
{
    wxString compopts;
    BeginOptionSection(compopts, _T("Compiler"), nrOfTabs);
    bool hasCompOpts = DoOptionSection(compopts, object->GetCompilerOptions(), nrOfTabs + 1, _T("option"));
    bool hasCompDirs = DoOptionSection(compopts, object->GetIncludeDirs(), nrOfTabs + 1, _T("directory"));
    if (hasCompOpts || hasCompDirs)
    {
        EndOptionSection(compopts, _T("Compiler"), nrOfTabs);
        buffer << compopts;
    }
}

void ProjectLoader::SaveResourceCompilerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs)
{
    wxString compopts;
    BeginOptionSection(compopts, _T("ResourceCompiler"), nrOfTabs);
    bool hasCompDirs = DoOptionSection(compopts, object->GetResourceIncludeDirs(), nrOfTabs + 1, _T("directory"));
    if (hasCompDirs)
    {
        EndOptionSection(compopts, _T("ResourceCompiler"), nrOfTabs);
        buffer << compopts;
    }
}

void ProjectLoader::SaveLinkerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs)
{
    wxString linkopts;
    BeginOptionSection(linkopts, _T("Linker"), nrOfTabs);
    bool hasLinkOpts = DoOptionSection(linkopts, object->GetLinkerOptions(), nrOfTabs + 1, _T("option"));
    bool hasLibs = DoOptionSection(linkopts, object->GetLinkLibs(), nrOfTabs + 1, _T("library"));
    bool hasLinkDirs = DoOptionSection(linkopts, object->GetLibDirs(), nrOfTabs + 1, _T("directory"));
    if (hasLinkOpts || hasLibs || hasLinkDirs)
    {
        EndOptionSection(linkopts, _T("Linker"), nrOfTabs);
        buffer << linkopts;
    }
}

void ProjectLoader::SaveEnvironment(wxString& buffer, CustomVars* vars, int nrOfTabs)
{
    if (!vars)
        return;
    const VarsArray& v = vars->GetVars();
    if (v.GetCount() == 0)
        return;
    for (int x = 0; x < nrOfTabs; ++x) buffer << _T('\t');
    buffer << _T("<Environment>") << _T('\n');
    for (unsigned int i = 0; i < v.GetCount(); ++i)
    {
        Var& var = v[i];
        for (int x = 0; x <= nrOfTabs; ++x) buffer << _T('\t');
        buffer << _T("<Variable name=\"") << var.name << _T("\" value=\"") << var.value << _T("\"/>") << _T('\n');
    }
    for (int x = 0; x < nrOfTabs; ++x) buffer << _T('\t');
    buffer << _T("</Environment>") << _T('\n');
}

void ProjectLoader::BeginOptionSection(wxString& buffer, const wxString& sectionName, int nrOfTabs)
{
    wxString local;
    for (int i = 0; i < nrOfTabs; ++i)
        local << _T('\t');
    local << _T("<") << sectionName << _T(">") << _T('\n');
    buffer << local;
}

bool ProjectLoader::DoOptionSection(wxString& buffer, const wxArrayString& array, int nrOfTabs, const wxString& optionName)
{
    if (!array.GetCount())
        return false;

    bool empty = true;
    wxString local;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        if (array[i].IsEmpty())
            continue;

        empty = false;
        for (int x = 0; x < nrOfTabs; ++x)
            local << _T('\t');
        local << _T("<Add ") << optionName << _T("=\"") << FixEntities(array[i]) << _T("\"/>") << _T('\n');
    }
    buffer << local;
    return !empty;
}

void ProjectLoader::EndOptionSection(wxString& buffer, const wxString& sectionName, int nrOfTabs)
{
    wxString local;
    for (int i = 0; i < nrOfTabs; ++i)
        local << _T('\t');
    local << _T("</") << sectionName << _T(">") << _T('\n');
    buffer << local;
}

void ProjectLoader::SaveOptions(wxString& buffer, const wxArrayString& array, const wxString& sectionName, int nrOfTabs, const wxString& optionName, const wxString& extra)
{
    if (!array.GetCount())
        return;

    wxString local;
    BeginOptionSection(local, sectionName, nrOfTabs);
    if (!extra.IsEmpty())
    {
        for (int i = 0; i < nrOfTabs + 1; ++i)
            local << _T('\t');
        local << extra << _T('\n');
    }
    bool notEmpty = DoOptionSection(local, array, nrOfTabs + 1, optionName);
    if (notEmpty || !extra.IsEmpty())
    {
        EndOptionSection(local, sectionName, nrOfTabs);
        buffer << local;
    }
}

int ProjectLoader::GetValidCompilerIndex(int proposal, const wxString& scope)
{
    if (CompilerFactory::CompilerIndexOK(proposal))
        return proposal;

    m_OpenDirty = true;

    wxArrayString compilers;
    for (unsigned int i = 0; i < CompilerFactory::Compilers.GetCount(); ++i)
    {
        compilers.Add(CompilerFactory::Compilers[i]->GetName());
    }

    wxString msg;
    msg.Printf(_("The specified compiler does not exist.\nPlease select the compiler to use for the %s:"), scope.c_str());
    proposal = wxGetSingleChoiceIndex(msg, _("Select compiler"), compilers);

    if (proposal == -1)
    {
        wxMessageBox(_("Setting to default compiler..."), _("Warning"), wxICON_WARNING);
        return CompilerFactory::GetDefaultCompilerIndex();
    }
    return proposal;
}
