#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "projectloader.h"
#include "compilerfactory.h"
#include "globals.h"

ProjectLoader::ProjectLoader(cbProject* project)
    : m_pProject(project)
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
    DoLinkerOptions(proj);
    DoIncludesOptions(proj);
    DoLibsOptions(proj);
    DoExtraCommands(proj);
    DoUnits(proj);

    return true;
}

void ProjectLoader::DoProjectOptions(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    if (!node)
        return; // no options
    
    wxString title;
    wxString makefile;
    int defaultTarget = 0;
    int compilerIdx = 0;
    
    // loop through all options
    while (node)
    {
        if (node->Attribute("title"))
            title = node->Attribute("title");
        
        if (node->Attribute("makefile"))
            makefile = node->Attribute("makefile");
        
        if (node->Attribute("default_target"))
            defaultTarget = atoi(node->Attribute("default_target"));

        if (node->Attribute("compiler"))
            compilerIdx = atoi(node->Attribute("compiler"));
        
        node = node->NextSiblingElement("Option");
    }
    
    m_pProject->SetTitle(title);
    m_pProject->SetMakefile(makefile);
    m_pProject->SetDefaultExecuteTargetIndex(defaultTarget);
    m_pProject->SetCompilerIndex(compilerIdx);
}

void ProjectLoader::DoBuild(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Build");
    while (node)
    {
        DoBuildTarget(node);
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
            DoLinkerOptions(node, target);
            DoIncludesOptions(node, target);
            DoLibsOptions(node, target);
            DoExtraCommands(node, target);
        }

        node = node->NextSiblingElement("Target");
    }
}

void ProjectLoader::DoBuildTargetOptions(TiXmlElement* parentNode, ProjectBuildTarget* target)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    if (!node)
        return; // no options
    
    wxString output;
    wxString deps;
    int type = -1;
    wxString parameters;
    wxString hostApplication;
    bool includeInTargetAll = true;
    int projectCompilerOptionsRelation = 3;
    int projectLinkerOptionsRelation = 3;
    int projectIncludeDirsRelation = 3;
    int projectLibDirsRelation = 3;
    
    while (node)
    {
        if (node->Attribute("output"))
            output = node->Attribute("output");
            
        if (node->Attribute("external_deps"))
            deps = node->Attribute("external_deps");
            
        if (node->Attribute("type"))
            type = atoi(node->Attribute("type"));
            
        if (node->Attribute("parameters"))
            parameters = node->Attribute("parameters");

        if (node->Attribute("host_application"))
            hostApplication = node->Attribute("host_application");
            
        if (node->Attribute("includeInTargetAll"))
            includeInTargetAll = atoi(node->Attribute("includeInTargetAll")) != 0;

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
        if (!output.IsEmpty())
            target->SetOutputFilename(output);
        target->SetExternalDeps(deps);
        target->SetTargetType((TargetType)type);
        target->SetExecutionParameters(parameters);
        target->SetHostApplication(hostApplication);
        target->SetIncludeInTargetAll(includeInTargetAll);
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
        if (!option.IsEmpty())
        {
            if (target)
                target->AddCompilerOption(option);
            else
                m_pProject->AddCompilerOption(option);
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
        if (!option.IsEmpty())
        {
            if (target)
                target->AddLinkerOption(option);
            else
                m_pProject->AddLinkerOption(option);
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
    if (!node)
        return; // no options

    TiXmlElement* child = node->FirstChildElement("Add");
    while (child)
    {
        wxString before;
        wxString after;
        
        if (child->Attribute("before"))
            before = child->Attribute("before");
        if (child->Attribute("after"))
            after = child->Attribute("after");

        if (!before.IsEmpty())
        {
            if (target)
                target->AddCommandsBeforeBuild(before);
            else
                m_pProject->AddCommandsBeforeBuild(before);
        }
        if (!after.IsEmpty())
        {
            if (target)
                target->AddCommandsAfterBuild(after);
            else
                m_pProject->AddCommandsAfterBuild(after);
        }

        child = child->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoUnits(TiXmlElement* parentNode)
{
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
            if (objName.GetExt() != CompilerFactory::Compilers[m_pProject->GetCompilerIndex()]->GetSwitches().objectExtension)
                file->SetObjName(objName.GetFullName());
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

    buffer << "<?xml version=\"1.0\"?>" << '\n';
    buffer << "<!DOCTYPE Code::Blocks_project_file>" << '\n';
    buffer << "<Code::Blocks_project_file>" << '\n';
    buffer << '\t' << "<Project>" << '\n';
    buffer << '\t' << '\t' << "<Option title=\"" << m_pProject->GetTitle() << "\"/>" << '\n';
    buffer << '\t' << '\t' << "<Option makefile=\"" << m_pProject->GetMakefile() << "\"/>" << '\n';
    if (m_pProject->GetDefaultExecuteTargetIndex() != 0)
        buffer << '\t' << '\t' << "<Option default_target=\"" << m_pProject->GetDefaultExecuteTargetIndex() << "\"/>" << '\n';
    if (m_pProject->GetCompilerIndex() != 0)
        buffer << '\t' << '\t' << "<Option compiler=\"" << m_pProject->GetCompilerIndex() << "\"/>" << '\n';

    buffer << '\t' << '\t' << "<Build>" << '\n';
    for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
        if (!target)
            break;
            
        buffer << '\t' << '\t' << '\t' << "<Target title=\"" << target->GetTitle() << "\">" << '\n';
        buffer << '\t' << '\t' << '\t' << '\t' << "<Option output=\"" << target->GetOutputFilename() << "\"/>" << '\n';
        buffer << '\t' << '\t' << '\t' << '\t' << "<Option external_deps=\"" << target->GetExternalDeps() << "\"/>" << '\n';
        buffer << '\t' << '\t' << '\t' << '\t' << "<Option type=\"" << target->GetTargetType() << "\"/>" << '\n';
        if (!target->GetExecutionParameters().IsEmpty())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option parameters=\"" << target->GetExecutionParameters() << "\"/>" << '\n';
        if (!target->GetIncludeInTargetAll())
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option includeInTargetAll=\"0\"/>" << '\n';
        if (target->GetOptionRelation(ortCompilerOptions) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectCompilerOptionsRelation=\"" << target->GetOptionRelation(ortCompilerOptions) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortLinkerOptions) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectLinkerOptionsRelation=\"" << target->GetOptionRelation(ortLinkerOptions) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortIncludeDirs) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectIncludeDirsRelation=\"" << target->GetOptionRelation(ortIncludeDirs) << "\"/>" << '\n';
        if (target->GetOptionRelation(ortLibDirs) != 3) // 3 is the default
            buffer << '\t' << '\t' << '\t' << '\t' << "<Option projectLibDirsRelation=\"" << target->GetOptionRelation(ortLibDirs) << "\"/>" << '\n';
        SaveOptions(buffer, target->GetCompilerOptions(), "Compiler", 4);
        SaveOptions(buffer, target->GetLinkerOptions(), "Linker", 4);
        SaveOptions(buffer, target->GetIncludeDirs(), "IncludeDirs", 4);
        SaveOptions(buffer, target->GetLibDirs(), "LibDirs", 4);
        SaveOptions(buffer, target->GetCommandsBeforeBuild(), "ExtraCommands", 4, "before");
        SaveOptions(buffer, target->GetCommandsAfterBuild(), "ExtraCommands", 4, "after");
        buffer << '\t' << '\t' << '\t' << "</Target>" << '\n';
    }
    buffer << '\t' << '\t' << "</Build>" << '\n';

    SaveOptions(buffer, m_pProject->GetCompilerOptions(), "Compiler", 2);
    SaveOptions(buffer, m_pProject->GetLinkerOptions(), "Linker", 2);
    SaveOptions(buffer, m_pProject->GetIncludeDirs(), "IncludeDirs", 2);
    SaveOptions(buffer, m_pProject->GetLibDirs(), "LibDirs", 2);
    SaveOptions(buffer, m_pProject->GetCommandsBeforeBuild(), "ExtraCommands", 2, "before");
    SaveOptions(buffer, m_pProject->GetCommandsAfterBuild(), "ExtraCommands", 2, "after");

    int count = m_pProject->GetFilesCount();
    for (int i = 0; i < count; ++i)
    {
        ProjectFile* f = m_pProject->GetFile(i);
        buffer << '\t' << '\t' << "<Unit filename=\"" << f->relativeFilename << "\">" << '\n';
        buffer << '\t' << '\t' << '\t' << "<Option compilerVar=\"" << f->compilerVar << "\"/>" << '\n';
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
            buffer << '\t' << '\t' << '\t' << "<Option buildCommand=\"" << f->buildCommand << "\"/>" << '\n';
        }
        if (!f->autoDeps)
            buffer << '\t' << '\t' << '\t' << "<Option autoDeps=\"0\"/>" << '\n';
        if (!f->customDeps.IsEmpty())
        {
            f->customDeps.Replace("\n", "\\n");
            buffer << '\t' << '\t' << '\t' << "<Option customDeps=\"" << f->customDeps << "\"/>" << '\n';
        }
        if (!f->GetObjName().IsEmpty())
        {
            wxFileName tmp(f->GetObjName());
            if (tmp.GetExt() != CompilerFactory::Compilers[m_pProject->GetCompilerIndex()]->GetSwitches().objectExtension)
                buffer << '\t' << '\t' << '\t' << "<Option objectName=\"" << f->GetObjName() << "\"/>" << '\n';
        }
        for (unsigned int x = 0; x < f->buildTargets.GetCount(); ++x)
            buffer << '\t' << '\t' << '\t' << "<Option target=\"" << f->buildTargets[x] << "\"/>" << '\n';
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

bool ProjectLoader::ImportDevCpp(const wxString& filename)
{
    m_pProject->ClearAllProperties();
        
    wxFileConfig* dev = new wxFileConfig("", "", filename, "", wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    dev->SetPath("/Project");
    int unitCount;
    dev->Read("UnitCount", &unitCount, 0);
        
    wxString path, tmp, title, output; 
    wxArrayString array;
    int typ;
        
    // read project options
    dev->Read("Name", &title, "");
    m_pProject->SetTitle(title);

    dev->Read("Type", &typ, 0);
    m_pProject->SetTargetType(TargetType(typ));

    dev->Read("OverrideOutputName", &output, "");
    m_pProject->SetOutputFilename(output);

    dev->Read("CppCompiler", &tmp, "");
    if (tmp.IsEmpty())
        dev->Read("Compiler", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, "_@@@@_");
    m_pProject->SetCompilerOptions(array);

    dev->Read("Linker", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, "_@@@@_");
    m_pProject->SetLinkerOptions(array);

    dev->Read("Includes", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, ";");
    m_pProject->SetIncludeDirs(array);

    dev->Read("Libs", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, ";");
    m_pProject->SetLibDirs(array);

    dev->Read("Resources", &tmp, "");
    array = GetArrayFromString(tmp, ","); // make sure that this is comma-separated
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        tmp = array[i];
        m_pProject->AddFile(0, tmp, true, true);
    }

    // read project units
    for (int x = 0; x < unitCount; ++x)
    {
        path.Printf("/Unit%d", x + 1);
        dev->SetPath(path);
        tmp.Clear();
        dev->Read("FileName", &tmp, "");

        bool compile, compileCpp, link;
        dev->Read("Compile", &compile, false);
        dev->Read("CompileCpp", &compileCpp, true);
        dev->Read("Link", &link, true);
        m_pProject->AddFile(0, tmp, compile || compileCpp, link);
    }

    delete dev;

    m_pProject->SetModified(true);
    return true;
}

void ProjectLoader::SaveOptions(wxString& buffer, const wxArrayString& array, const wxString& sectionName, int nrOfTabs, const wxString& optionName)
{
    if (!array.GetCount())
        return;
    
    wxString local;
    bool empty = true;
    
    for (int i = 0; i < nrOfTabs; ++i)
        local << '\t';
    local << "<" << sectionName << ">" << '\n';
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        if (array[i].IsEmpty())
            continue;
        
        empty = false;
        for (int x = 0; x <= nrOfTabs; ++x)
            local << '\t';
        local << "<Add " << optionName << "=\"" << array[i] << "\"/>" << '\n';
    }
    for (int i = 0; i < nrOfTabs; ++i)
        local << '\t';
    local << "</" << sectionName << ">" << '\n';
    
    if (!empty)
        buffer << local;
}
