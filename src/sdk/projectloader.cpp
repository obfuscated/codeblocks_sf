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
    m_Upgraded(false)
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

    pMsg->DebugLog("Loading project file...");
    TiXmlDocument doc(filename.c_str());
    if (!doc.LoadFile())
        return false;

    pMsg->DebugLog("Parsing project file...");
    TiXmlElement* root;
    TiXmlElement* proj;
    
    root = doc.FirstChildElement("Code::Blocks_project_file");
    if (!root)
    {
        pMsg->DebugLog("Not a valid Code::Blocks project file...");
        return false;
    }
    proj = root->FirstChildElement("Project");
    if (!proj)
    {
        pMsg->DebugLog("No 'Project' element in file...");
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
                ext = "." + ext;
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
    
    // loop through all options
    while (node)
    {
        if (node->Attribute("title"))
            title = node->Attribute("title");
        
        if (node->Attribute("makefile"))
            makefile = node->Attribute("makefile");

        if (node->Attribute("makefile_is_custom"))
            makefile_custom = strncmp(node->Attribute("makefile_is_custom"), "1", 1) == 0;
        
        if (node->Attribute("default_target"))
            defaultTarget = atoi(node->Attribute("default_target"));

        if (node->Attribute("active_target"))
            activeTarget = atoi(node->Attribute("active_target"));

        if (node->Attribute("compiler"))
            compilerIdx = atoi(node->Attribute("compiler"));
        
        node = node->NextSiblingElement("Option");
    }
    
    m_pProject->SetTitle(title);
    m_pProject->SetMakefile(makefile);
    m_pProject->SetMakefileCustom(makefile_custom);
    m_pProject->SetDefaultExecuteTargetIndex(defaultTarget);
    m_pProject->SetActiveBuildTarget(activeTarget);
    m_pProject->SetCompilerIndex(compilerIdx);
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
        wxString title = node->Attribute("title");
        if (!title.IsEmpty())
            target = m_pProject->AddBuildTarget(title);

        if (target)
        {
            Manager::Get()->GetMessageManager()->DebugLog("Loading target %s", title.c_str());
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

        if (node->Attribute("output"))
            output = node->Attribute("output");

        if (node->Attribute("working_dir"))
            working_dir = node->Attribute("working_dir");

        if (node->Attribute("object_output"))
            obj_output = node->Attribute("object_output");

        if (node->Attribute("deps_output"))
            deps_output = node->Attribute("deps_output");
            
        if (node->Attribute("external_deps"))
            deps = node->Attribute("external_deps");

        if (node->Attribute("additional_depfiles"))
            added = node->Attribute("additional_depfiles");
            
        if (node->Attribute("type"))
            type = atoi(node->Attribute("type"));
            
        if (node->Attribute("compiler"))
            compilerIdx = atoi(node->Attribute("compiler"));

        if (node->Attribute("parameters"))
            parameters = node->Attribute("parameters");

        if (node->Attribute("host_application"))
            hostApplication = node->Attribute("host_application");
            
        if (node->Attribute("includeInTargetAll"))
            includeInTargetAll = atoi(node->Attribute("includeInTargetAll")) != 0;

        if (node->Attribute("createDefFile"))
            createDefFile = atoi(node->Attribute("createDefFile")) != 0;

        if (node->Attribute("createStaticLib"))
            createStaticLib = atoi(node->Attribute("createStaticLib")) != 0;

        if (node->Attribute("projectCompilerOptionsRelation"))
            projectCompilerOptionsRelation = atoi(node->Attribute("projectCompilerOptionsRelation"));

        if (node->Attribute("projectLinkerOptionsRelation"))
            projectLinkerOptionsRelation = atoi(node->Attribute("projectLinkerOptionsRelation"));

        if (node->Attribute("projectIncludeDirsRelation"))
            projectIncludeDirsRelation = atoi(node->Attribute("projectIncludeDirsRelation"));

        if (node->Attribute("projectLibDirsRelation"))
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
        wxString option = child->Attribute("option");
        wxString dir = child->Attribute("directory");
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
        wxString dir = child->Attribute("directory");
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
        wxString option = child->Attribute("option");
        wxString dir = child->Attribute("directory");
        wxString lib = child->Attribute("library");
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
        wxString option = child->Attribute("option");
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
        wxString option = child->Attribute("option");
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
            wxString mode = child->Attribute("before");
            if (mode == "always")
                base->SetAlwaysRunPreBuildSteps(true);
            mode = child->Attribute("after");
            if (mode == "always")
                base->SetAlwaysRunPostBuildSteps(true);
    
            child = child->NextSiblingElement("Mode");
        }
    
        child = node->FirstChildElement("Add");
        while (child)
        {
            wxString before;
            wxString after;
            
            if (child->Attribute("before"))
                before = child->Attribute("before");
            if (child->Attribute("after"))
                after = child->Attribute("after");
    
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
            wxString name = child->Attribute("name");
            wxString value = child->Attribute("value");
            if (!name.IsEmpty())
            	vars.Add(name, value);
    
            child = child->NextSiblingElement("Variable");
        }
        node = node->NextSiblingElement("Environment");
    }
}

void ProjectLoader::DoUnits(TiXmlElement* parentNode)
{
    Manager::Get()->GetMessageManager()->DebugLog("Generating project tree...");
    TiXmlElement* unit = parentNode->FirstChildElement("Unit");
    while (unit)
    {
        wxString filename = unit->Attribute("filename");
        if (!filename.IsEmpty())
        {
            ProjectFile* file = m_pProject->AddFile(-1, filename);
            if (!file)
                Manager::Get()->GetMessageManager()->DebugLog("Can't add file '%s'", filename.c_str());
            else
                DoUnitOptions(unit, file);
        }
        
        unit = unit->NextSiblingElement("Unit");
    }
}

void ProjectLoader::DoUnitOptions(TiXmlElement* parentNode, ProjectFile* file)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    while (node)
    {
        if (node->Attribute("compilerVar"))
            file->compilerVar = node->Attribute("compilerVar");
        if (node->Attribute("compile"))
            file->compile = atoi(node->Attribute("compile")) != 0;
        if (node->Attribute("link"))
            file->link = atoi(node->Attribute("link")) != 0;
        if (node->Attribute("weight"))
            file->weight = atoi(node->Attribute("weight"));
        if (node->Attribute("useBuildCommand"))
            file->useCustomBuildCommand = atoi(node->Attribute("useBuildCommand")) != 0;
        if (node->Attribute("buildCommand"))
        {
            wxString tmp = node->Attribute("buildCommand");
            if (!tmp.IsEmpty())
            {
                tmp.Replace("\\n", "\n");
                file->buildCommand = tmp;
            }
        }
        if (node->Attribute("autoDeps"))
            file->autoDeps = atoi(node->Attribute("autoDeps")) != 0;
        if (node->Attribute("customDeps"))
        {
            wxString tmp = node->Attribute("customDeps");
            if (!tmp.IsEmpty())
            {
                tmp.Replace("\\n", "\n");
                file->customDeps = tmp;
            }
        }
        if (node->Attribute("objectName"))
        {
            wxFileName objName(node->Attribute("objectName"));
            FileType ft = FileTypeOf(file->relativeFilename);
            if (ft != ftResource && ft != ftResourceBin)
            {
                if (objName.GetExt() != CompilerFactory::Compilers[m_pProject->GetCompilerIndex()]->GetSwitches().objectExtension)
                    file->SetObjName(file->relativeFilename);
            }
        }
        if (node->Attribute("target"))
            file->AddBuildTarget(node->Attribute("target"));

        node = node->NextSiblingElement("Option");
    }
}

bool ProjectLoader::Save(const wxString& filename)
{
    wxString buffer;
    wxArrayString array;
    CustomVars* vars = 0;

    buffer << "<?xml version=\"1.0\"?>" << '\n';
    buffer << "<!DOCTYPE Code::Blocks_project_file>" << '\n';
    buffer << "<Code::Blocks_project_file>" << '\n';
    buffer << '\t' << "<FileVersion major=\"" << PROJECT_FILE_VERSION_MAJOR << "\" minor=\"" << PROJECT_FILE_VERSION_MINOR << "\"/>" << '\n';
    buffer << '\t' << "<Project>" << '\n';
    buffer << '\t' << '\t' << "<Option title=\"" << FixEntities(m_pProject->GetTitle()) << "\"/>" << '\n';
    buffer << '\t' << '\t' << "<Option makefile=\"" << FixEntities(m_pProject->GetMakefile()) << "\"/>" << '\n';
    buffer << '\t' << '\t' << "<Option makefile_is_custom=\"" << m_pProject->IsMakefileCustom() << "\"/>" << '\n';
    if (m_pProject->GetDefaultExecuteTargetIndex() != 0)
        buffer << '\t' << '\t' << "<Option default_target=\"" << m_pProject->GetDefaultExecuteTargetIndex() << "\"/>" << '\n';
    if (m_pProject->GetActiveBuildTarget() != -1)
        buffer << '\t' << '\t' << "<Option active_target=\"" << m_pProject->GetActiveBuildTarget() << "\"/>" << '\n';
    buffer << '\t' << '\t' << "<Option compiler=\"" << m_pProject->GetCompilerIndex() << "\"/>" << '\n';

    buffer << '\t' << '\t' << "<Build>" << '\n';
    for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
        if (!target)
            break;
            
        buffer << '\t' << '\t' << '\t' << "<Target title=\"" << FixEntities(target->GetTitle()) << "\">" << '\n';
        if (target->GetTargetType() != ttCommandsOnly)
        {
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option output=\"" << FixEntities(target->GetOutputFilename()) << "\"/>" << '\n';
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option working_dir=\"" << FixEntities(target->GetWorkingDir()) << "\"/>" << '\n';
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option object_output=\"" << FixEntities(target->GetObjectOutput()) << "\"/>" << '\n';
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option deps_output=\"" << FixEntities(target->GetDepsOutput()) << "\"/>" << '\n';
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option external_deps=\"" << FixEntities(target->GetExternalDeps()) << "\"/>" << '\n';
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option additional_depfiles=\"" << FixEntities(target->GetAdditionalOutputFiles()) << "\"/>" << '\n';
        }
        buffer << '\t' << '\t' << '\t' << '\t' << "<Option type=\"" << target->GetTargetType() << "\"/>" << '\n';
        buffer << '\t' << '\t' << '\t' << '\t' << "<Option compiler=\"" << target->GetCompilerIndex() << "\"/>" << '\n';
        if (target->GetTargetType() == ttConsoleOnly && !target->GetUseConsoleRunner())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option use_console_runner=\"0\"/>" << '\n';
        if (!target->GetExecutionParameters().IsEmpty())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option parameters=\"" << FixEntities(target->GetExecutionParameters()) << "\"/>" << '\n';
        if (!target->GetHostApplication().IsEmpty())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option host_application=\"" << FixEntities(target->GetHostApplication()) << "\"/>" << '\n';
        if (!target->GetIncludeInTargetAll())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option includeInTargetAll=\"0\"/>" << '\n';
        if ((target->GetTargetType() == ttStaticLib || target->GetTargetType() == ttDynamicLib) && target->GetCreateDefFile())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option createDefFile=\"1\"/>" << '\n';
        if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option createStaticLib=\"1\"/>" << '\n';
        if (target->GetOptionRelation(ortCompilerOptions) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectCompilerOptionsRelation=\"" << target->GetOptionRelation(ortCompilerOptions) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortLinkerOptions) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectLinkerOptionsRelation=\"" << target->GetOptionRelation(ortLinkerOptions) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortIncludeDirs) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectIncludeDirsRelation=\"" << target->GetOptionRelation(ortIncludeDirs) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortResDirs) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectResourceIncludeDirsRelation=\"" << target->GetOptionRelation(ortResDirs) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortLibDirs) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectLibDirsRelation=\"" << target->GetOptionRelation(ortLibDirs) << "\"/>" << '\n';
        SaveCompilerOptions(buffer, target, 4);
        SaveResourceCompilerOptions(buffer, target, 4);
        SaveLinkerOptions(buffer, target, 4);
        SaveOptions(buffer, target->GetCommandsBeforeBuild(), "ExtraCommands", 4, "before", target->GetAlwaysRunPreBuildSteps() ? "<Mode before=\"always\" />" : "");
        SaveOptions(buffer, target->GetCommandsAfterBuild(), "ExtraCommands", 4, "after", target->GetAlwaysRunPostBuildSteps() ? "<Mode after=\"always\" />" : "");

        vars = &target->GetCustomVars();
        SaveEnvironment(buffer, vars, 4);

        buffer << '\t' << '\t' << '\t' << "</Target>" << '\n';
    }
    vars = &m_pProject->GetCustomVars();
    SaveEnvironment(buffer, vars, 3);
    buffer << '\t' << '\t' << "</Build>" << '\n';

    SaveCompilerOptions(buffer, m_pProject, 2);
    SaveResourceCompilerOptions(buffer, m_pProject, 2);
    SaveLinkerOptions(buffer, m_pProject, 2);
    SaveOptions(buffer, m_pProject->GetCommandsBeforeBuild(), "ExtraCommands", 2, "before", m_pProject->GetAlwaysRunPreBuildSteps() ? "<Mode before=\"always\" />" : "");
    SaveOptions(buffer, m_pProject->GetCommandsAfterBuild(), "ExtraCommands", 2, "after", m_pProject->GetAlwaysRunPostBuildSteps() ? "<Mode after=\"always\" />" : "");

    int count = m_pProject->GetFilesCount();
    for (int i = 0; i < count; ++i)
    {
        ProjectFile* f = m_pProject->GetFile(i);
        buffer << '\t' << '\t' << "<Unit filename=\"" << FixEntities(f->relativeFilename) << "\">" << '\n';
        buffer << '\t' << '\t' << '\t' << "<Option compilerVar=\"" << FixEntities(f->compilerVar) << "\"/>" << '\n';
        if (!f->compile)
            buffer << '\t' << '\t' << '\t' << "<Option compile=\"0\"/>" << '\n';
        if (!f->link)
            buffer << '\t' << '\t' << '\t' << "<Option link=\"0\"/>" << '\n';
        if (f->weight != 50)
            buffer << '\t' << '\t' << '\t' << "<Option weight=\"" << f->weight << "\"/>" << '\n';
        if (f->useCustomBuildCommand)
            buffer << '\t' << '\t' << '\t' << "<Option useBuildCommand=\"1\"/>" << '\n';
        if (!f->buildCommand.IsEmpty())
        {
            f->buildCommand.Replace("\n", "\\n");
            buffer << '\t' << '\t' << '\t' << "<Option buildCommand=\"" << FixEntities(f->buildCommand) << "\"/>" << '\n';
        }
        if (!f->autoDeps)
            buffer << '\t' << '\t' << '\t' << "<Option autoDeps=\"0\"/>" << '\n';
        if (!f->customDeps.IsEmpty())
        {
            f->customDeps.Replace("\n", "\\n");
            buffer << '\t' << '\t' << '\t' << "<Option customDeps=\"" << FixEntities(f->customDeps) << "\"/>" << '\n';
        }
        if (!f->GetObjName().IsEmpty())
        {
            wxFileName tmp(f->GetObjName());
            if (tmp.GetExt() != CompilerFactory::Compilers[m_pProject->GetCompilerIndex()]->GetSwitches().objectExtension)
                buffer << '\t' << '\t' << '\t' << "<Option objectName=\"" << FixEntities(f->GetObjName()) << "\"/>" << '\n';
        }
        for (unsigned int x = 0; x < f->buildTargets.GetCount(); ++x)
            buffer << '\t' << '\t' << '\t' << "<Option target=\"" << FixEntities(f->buildTargets[x]) << "\"/>" << '\n';
        for (unsigned int x = 0; x < f->breakpoints.GetCount(); ++x)
		{
			DebuggerBreakpoint* bp = f->breakpoints[x];
            buffer << '\t' << '\t' << '\t' << "<Breakpoint ";
			buffer << "line=\"" << bp->line << "\" ";
			buffer << "enabled=\"" << bp->enabled << "\" ";
			buffer << "pass=\"" << bp->ignoreCount << "\" ";
			buffer << "/>" << '\n';
		}
		buffer << '\t' << '\t' << "</Unit>" << '\n';
    }

    buffer << '\t' << "</Project>" << '\n';
    buffer << "</Code::Blocks_project_file>" << '\n';

    wxFile file(filename, wxFile::write);
    if (file.Write(buffer, buffer.Length()) == buffer.Length())
    {
		m_pProject->SetModified(false);
        return true;
    }
    return false;
}

void ProjectLoader::SaveCompilerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs)
{
    wxString compopts;
    BeginOptionSection(compopts, "Compiler", nrOfTabs);
    bool hasCompOpts = DoOptionSection(compopts, object->GetCompilerOptions(), nrOfTabs + 1, "option");
    bool hasCompDirs = DoOptionSection(compopts, object->GetIncludeDirs(), nrOfTabs + 1, "directory");
    if (hasCompOpts || hasCompDirs)
    {
        EndOptionSection(compopts, "Compiler", nrOfTabs);
        buffer << compopts;
    }
}

void ProjectLoader::SaveResourceCompilerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs)
{
    wxString compopts;
    BeginOptionSection(compopts, "ResourceCompiler", nrOfTabs);
    bool hasCompDirs = DoOptionSection(compopts, object->GetResourceIncludeDirs(), nrOfTabs + 1, "directory");
    if (hasCompDirs)
    {
        EndOptionSection(compopts, "ResourceCompiler", nrOfTabs);
        buffer << compopts;
    }
}

void ProjectLoader::SaveLinkerOptions(wxString& buffer, CompileOptionsBase* object, int nrOfTabs)
{
    wxString linkopts;
    BeginOptionSection(linkopts, "Linker", nrOfTabs);
    bool hasLinkOpts = DoOptionSection(linkopts, object->GetLinkerOptions(), nrOfTabs + 1, "option");
    bool hasLibs = DoOptionSection(linkopts, object->GetLinkLibs(), nrOfTabs + 1, "library");
    bool hasLinkDirs = DoOptionSection(linkopts, object->GetLibDirs(), nrOfTabs + 1, "directory");
    if (hasLinkOpts || hasLibs || hasLinkDirs)
    {
        EndOptionSection(linkopts, "Linker", nrOfTabs);
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
    for (int x = 0; x < nrOfTabs; ++x) buffer << '\t';
    buffer << "<Environment>" << '\n';
    for (unsigned int i = 0; i < v.GetCount(); ++i)
    {
        Var& var = v[i];
        for (int x = 0; x <= nrOfTabs; ++x) buffer << '\t';
        buffer << "<Variable name=\"" << var.name << "\" value=\"" << var.value << "\"/>" << '\n';
    }
    for (int x = 0; x < nrOfTabs; ++x) buffer << '\t';
    buffer << "</Environment>" << '\n';
}

void ProjectLoader::BeginOptionSection(wxString& buffer, const wxString& sectionName, int nrOfTabs)
{
    wxString local;
    for (int i = 0; i < nrOfTabs; ++i)
        local << '\t';
    local << "<" << sectionName << ">" << '\n';
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
            local << '\t';
        local << "<Add " << optionName << "=\"" << FixEntities(array[i]) << "\"/>" << '\n';
    }
    buffer << local;
    return !empty;
}

void ProjectLoader::EndOptionSection(wxString& buffer, const wxString& sectionName, int nrOfTabs)
{
    wxString local;
    for (int i = 0; i < nrOfTabs; ++i)
        local << '\t';
    local << "</" << sectionName << ">" << '\n';
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
            local << '\t';
        local << extra << '\n';
    }
    bool notEmpty = DoOptionSection(local, array, nrOfTabs + 1, optionName);
    if (notEmpty || !extra.IsEmpty())
    {
        EndOptionSection(local, sectionName, nrOfTabs);
        buffer << local;
    }
}
