/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/confbase.h>
    #include <wx/fileconf.h>
    #include <wx/intl.h>
    #include <wx/filename.h>
    #include <wx/msgdlg.h>
    #include <wx/log.h>
    #include <wx/stopwatch.h>
    #include "manager.h"
    #include "configmanager.h"
    #include "projectmanager.h"
    #include "messagemanager.h"
    #include "macrosmanager.h"
    #include "cbproject.h"
    #include "compilerfactory.h"
    #include "globals.h"
#endif

#include "filefilters.h"
#include "projectloader.h"
#include "projectloader_hooks.h"
#include "annoyingdialog.h"
#include "configmanager.h"

ProjectLoader::ProjectLoader(cbProject* project)
    : m_pProject(project),
    m_Upgraded(false),
    m_OpenDirty(false),
    m_1_4_to_1_5_deftarget(-1),
    m_IsPre_1_6(false)
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
    pMsg->DebugLog(_T("Loading project file..."));
    TiXmlDocument doc(filename.mb_str());
    if (!doc.LoadFile())
        return false;

    pMsg->DebugLog(_T("Parsing project file..."));
    TiXmlElement* root;
    TiXmlElement* proj;

    root = doc.FirstChildElement("CodeBlocks_project_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_project_file");
        if (!root)
        {
            pMsg->DebugLog(_T("Not a valid Code::Blocks project file..."));
            return false;
        }
    }
    proj = root->FirstChildElement("Project");
    if (!proj)
    {
        pMsg->DebugLog(_T("No 'Project' element in file..."));
        return false;
    }

    m_IsPre_1_2 = false; // flag for some changed defaults in version 1.2
    TiXmlElement* version = root->FirstChildElement("FileVersion");
    // don't show messages if we 're running a batch build (i.e. no gui)
    if (!Manager::IsBatchBuild() && version)
    {
        int major = PROJECT_FILE_VERSION_MAJOR;
        int minor = PROJECT_FILE_VERSION_MINOR;
        version->QueryIntAttribute("major", &major);
        version->QueryIntAttribute("minor", &minor);

        m_IsPre_1_6 = major < 1 || (major == 1 && minor < 6);

        if (major < 1 ||
            (major == 1 && minor < 2))
        {
            // pre-1.2
            pMsg->DebugLog(_T("Project version is %d.%d. Defaults have changed since then..."), major, minor);
            m_IsPre_1_2 = true;
        }
        else if (major >= PROJECT_FILE_VERSION_MAJOR && minor > PROJECT_FILE_VERSION_MINOR)
        {
            pMsg->DebugLog(_T("Project version is > %d.%d. Trying to load..."), PROJECT_FILE_VERSION_MAJOR, PROJECT_FILE_VERSION_MINOR);
            AnnoyingDialog dlg(_("Project file format is newer/unknown"),
                                _("This project file was saved with a newer version of Code::Blocks.\n"
                                "Will try to load, but you should make sure all the settings were loaded correctly..."),
                                wxART_WARNING,
                                AnnoyingDialog::OK,
                                wxID_OK);
            dlg.ShowModal();
        }
        else
        {
            // use one message for all changes
            wxString msg;
            wxString warn_msg;

            // 1.5 -> 1.6: values matching defaults are not written to <Unit> sections
            if (major == 1 && minor == 5)
            {
                msg << _("1.5 to 1.6:\n");
                msg << _("  * only saves values that differ from defaults (i.e. project files are smaller now).\n");
                msg << _("  * added object names generation mode setting (normal/extended).\n");
                msg << _("  * added project notes.\n");
                msg << _("\n");

                warn_msg << _("* Project file updated to version 1.6:\n");
                warn_msg << _("   When a project file is saved as version 1.6, it will NO LONGER be read correctly\n");
                warn_msg << _("   by earlier Code::Blocks versions!\n");
                warn_msg << _("   So, if you plan on using this project with an earlier Code::Blocks version, you\n");
                warn_msg << _("   should probably NOT save this project as version 1.6...\n");
                warn_msg << _("\n");
            }

            // 1.4 -> 1.5: updated custom build command per-project file
            if (major == 1 && minor == 4)
            {
                msg << _("1.4 to 1.5:\n");
                msg << _("  * added virtual build targets.\n");
                msg << _("\n");
            }

            // 1.3 -> 1.4: updated custom build command per-project file
            if (major == 1 && minor == 3)
            {
                msg << _("1.3 to 1.4:\n");
                msg << _("  * changed the way custom file build commands are stored (no auto-conversion).\n");
                msg << _("\n");
            }

            if (!msg.IsEmpty())
            {
                m_Upgraded = true;
                msg.Prepend(wxString::Format(_("Project file format is older (%d.%d) than the current format (%d.%d).\n"
                                                "The file will automatically be upgraded on save.\n"
                                                "But please read the following list of changes, as some of them "
                                                "might not automatically convert existing (old) settings.\n"
                                                "If you don't understand what a change means, you probably don't "
                                                "use that feature so you don't have to worry about it.\n\n"
                                                "List of changes:\n"),
                                            major,
                                            minor,
                                            PROJECT_FILE_VERSION_MAJOR,
                                            PROJECT_FILE_VERSION_MINOR));
                AnnoyingDialog dlg(_("Project file format changed"),
                                    msg,
                                    wxART_INFORMATION,
                                    AnnoyingDialog::OK,
                                    wxID_OK);
                dlg.ShowModal();
            }

            if (!warn_msg.IsEmpty())
            {
                warn_msg.Prepend(_("!!! WARNING !!!\n\n"));
                AnnoyingDialog dlg(_("Project file upgrade warning"),
                                    warn_msg,
                                    wxART_WARNING,
                                    AnnoyingDialog::OK,
                                    wxID_OK);
                dlg.ShowModal();
            }
        }
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

    // if targets still use the "build with all" flag,
    // it's time for conversion
    if (!m_pProject->HasVirtualBuildTarget(_T("All")))
    {
        wxArrayString all;
        for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(i);
            if (bt && bt->GetIncludeInTargetAll())
                all.Add(bt->GetTitle());
        }
        if (all.GetCount())
        {
            m_pProject->DefineVirtualBuildTarget(_T("All"), all);
            m_Upgraded = true;
        }
    }

    // convert old deftarget int to string
    if (m_1_4_to_1_5_deftarget != -1)
    {
        ProjectBuildTarget* bt = m_pProject->GetBuildTarget(m_1_4_to_1_5_deftarget);
        if (bt)
            m_pProject->SetDefaultExecuteTarget(bt->GetTitle());
    }

    // as a last step, run all hooked callbacks
    TiXmlElement* node = proj->FirstChildElement("Extensions");
    if (node)
    {
        ProjectLoaderHooks::CallHooks(m_pProject, node, true);
    }

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

    pMsg->DebugLog(wxString(_T("Done loading project in ")) << wxString::Format(_T("%d"), (int) sw.Time()) << _T("ms"));
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
    if (cbMessageBox(msg, _("Question"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
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

    wxString compilerId = object->GetCompilerID();
    Compiler* compiler = CompilerFactory::GetCompiler(compilerId);
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

void ProjectLoader::DoMakeCommands(TiXmlElement* parentNode, CompileTargetBase* target)
{
    if (!parentNode)
        return; // no options

    TiXmlElement* node;

    node = parentNode->FirstChildElement("Build");
    if (node && node->Attribute("command"))
        target->SetMakeCommandFor(mcBuild, cbC2U(node->Attribute("command")));

    node = parentNode->FirstChildElement("CompileFile");
    if (node && node->Attribute("command"))
        target->SetMakeCommandFor(mcCompileFile, cbC2U(node->Attribute("command")));

    node = parentNode->FirstChildElement("Clean");
    if (node && node->Attribute("command"))
        target->SetMakeCommandFor(mcClean, cbC2U(node->Attribute("command")));

    node = parentNode->FirstChildElement("DistClean");
    if (node && node->Attribute("command"))
        target->SetMakeCommandFor(mcDistClean, cbC2U(node->Attribute("command")));
}

void ProjectLoader::DoVirtualTargets(TiXmlElement* parentNode)
{
    if (!parentNode)
        return;

    TiXmlElement* node = parentNode->FirstChildElement("Add");
    if (!node)
        return; // no virtual targets

    while (node)
    {
        if (node->Attribute("alias") && node->Attribute("targets"))
        {
            wxString alias = cbC2U(node->Attribute("alias"));
            wxString targets = cbC2U(node->Attribute("targets"));
            wxArrayString arr = GetArrayFromString(targets, _T(";"), true);

            m_pProject->DefineVirtualBuildTarget(alias, arr);
        }

        node = node->NextSiblingElement("Add");
    }
}

void ProjectLoader::DoProjectOptions(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    if (!node)
        return; // no options

    wxString title;
    wxString makefile;
    bool makefile_custom = false;
    wxString defaultTarget;
    wxString compilerId = _T("gcc");
    bool extendedObjectNames = false;
    wxArrayString vfolders;
    int platformsFinal = spAll;
    PCHMode pch_mode = m_IsPre_1_2 ? pchSourceDir : pchObjectDir;
    bool showNotes = false;
    wxString notes;

    // loop through all options
    while (node)
    {
        if (node->Attribute("title"))
            title = cbC2U(node->Attribute("title"));

        else if (node->Attribute("platforms"))
        	platformsFinal = GetPlatformsFromString(cbC2U(node->Attribute("platforms")));

        else if (node->Attribute("makefile")) // there is only one attribute per option, so "else" is a safe optimisation
            makefile = cbC2U(node->Attribute("makefile"));

        else if (node->Attribute("makefile_is_custom"))
            makefile_custom = strncmp(node->Attribute("makefile_is_custom"), "1", 1) == 0;

        // old default_target (int) node
        else if (node->QueryIntAttribute("default_target", &m_1_4_to_1_5_deftarget) == TIXML_SUCCESS)
        {
            // we read the value
        }

        else if (node->Attribute("default_target"))
            defaultTarget = cbC2U(node->Attribute("default_target"));

        else if (node->Attribute("compiler"))
            compilerId = GetValidCompilerID(cbC2U(node->Attribute("compiler")), _T("the project"));

        else if (node->Attribute("extended_obj_names"))
            extendedObjectNames = strncmp(node->Attribute("extended_obj_names"), "1", 1) == 0;

        else if (node->Attribute("pch_mode"))
            pch_mode = (PCHMode)atoi(node->Attribute("pch_mode"));

        else if (node->Attribute("virtualFolders"))
            vfolders = GetArrayFromString(cbC2U(node->Attribute("virtualFolders")), _T(";"));

        else if (node->Attribute("show_notes"))
        {
            TiXmlHandle parentHandle(node);
            TiXmlText* t = (TiXmlText *) parentHandle.FirstChild("notes").FirstChild().Node();
            if (t)
                notes = cbC2U(t->Value());
            showNotes = !notes.IsEmpty() && strncmp(node->Attribute("show_notes"), "1", 1) == 0;
        }

        node = node->NextSiblingElement("Option");
    }

    m_pProject->SetTitle(title);
    m_pProject->SetPlatforms(platformsFinal);
    m_pProject->SetMakefile(makefile);
    m_pProject->SetMakefileCustom(makefile_custom);
    m_pProject->SetDefaultExecuteTarget(defaultTarget);
    m_pProject->SetCompilerID(compilerId);
    m_pProject->SetExtendedObjectNamesGeneration(extendedObjectNames);
    m_pProject->SetModeForPCH(pch_mode);
    m_pProject->SetVirtualFolders(vfolders);
    m_pProject->SetNotes(notes);
    m_pProject->SetShowNotesOnLoad(showNotes);

    DoMakeCommands(parentNode->FirstChildElement("MakeCommands"), m_pProject);
    DoVirtualTargets(parentNode->FirstChildElement("VirtualTargets"));
}

void ProjectLoader::DoBuild(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Build");
    while (node)
    {
        TiXmlElement* opt = node->FirstChildElement("Script");
        while (opt)
        {
            if (opt->Attribute("file"))
                m_pProject->AddBuildScript(cbC2U(opt->Attribute("file")));

            opt = opt->NextSiblingElement("Script");
        }

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
        wxString title = cbC2U(node->Attribute("title"));
        if (!title.IsEmpty())
            target = m_pProject->AddBuildTarget(title);

        if (target)
        {
            Manager::Get()->GetMessageManager()->DebugLog(_T("Loading target %s"), title.c_str());
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
    int platformsFinal = spAll;
    wxString compilerId = m_pProject->GetCompilerID();
    wxString parameters;
    wxString hostApplication;
    bool includeInTargetAll = m_IsPre_1_2 ? true : false;
    bool createStaticLib = false;
    bool createDefFile = false;
    int projectCompilerOptionsRelation = 3;
    int projectLinkerOptionsRelation = 3;
    int projectIncludeDirsRelation = 3;
    int projectLibDirsRelation = 3;
    int projectResIncludeDirsRelation = 3;
    TargetFilenameGenerationPolicy prefixPolicy = tgfpNone; // tgfpNone for compat. with older projects
    TargetFilenameGenerationPolicy extensionPolicy = tgfpNone;

    while (node)
    {
        if (node->Attribute("platforms"))
        	platformsFinal = GetPlatformsFromString(cbC2U(node->Attribute("platforms")));
        
        if (node->Attribute("use_console_runner"))
            use_console_runner = strncmp(node->Attribute("use_console_runner"), "0", 1) != 0;

        if (node->Attribute("output"))
            output = UnixFilename(cbC2U(node->Attribute("output")));

        if (node->Attribute("prefix_auto"))
            prefixPolicy = atoi(node->Attribute("prefix_auto")) == 1 ? tgfpPlatformDefault : tgfpNone;

        if (node->Attribute("extension_auto"))
            extensionPolicy = atoi(node->Attribute("extension_auto")) == 1 ? tgfpPlatformDefault : tgfpNone;

        if (node->Attribute("working_dir"))
            working_dir = UnixFilename(cbC2U(node->Attribute("working_dir")));

        if (node->Attribute("object_output"))
            obj_output = UnixFilename(cbC2U(node->Attribute("object_output")));

        if (node->Attribute("deps_output"))
            deps_output = UnixFilename(cbC2U(node->Attribute("deps_output")));

        if (node->Attribute("external_deps"))
            deps = UnixFilename(cbC2U(node->Attribute("external_deps")));

        if (node->Attribute("additional_output"))
            added = UnixFilename(cbC2U(node->Attribute("additional_output")));

        if (node->Attribute("type"))
            type = atoi(node->Attribute("type"));

        if (node->Attribute("compiler"))
            compilerId = GetValidCompilerID(cbC2U(node->Attribute("compiler")), target->GetTitle());

        if (node->Attribute("parameters"))
            parameters = cbC2U(node->Attribute("parameters"));

        if (node->Attribute("host_application"))
            hostApplication = UnixFilename(cbC2U(node->Attribute("host_application")));

        // used in versions prior to 1.5
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

        if (node->Attribute("projectResourceIncludeDirsRelation"))
        {
            projectResIncludeDirsRelation = atoi(node->Attribute("projectResourceIncludeDirsRelation"));
            // there used to be a bug in this setting and it might have a negative or very big number
            // detect this case and set as default
            if (projectResIncludeDirsRelation < 0 || projectResIncludeDirsRelation >= ortLast)
                projectResIncludeDirsRelation = 3;
        }

        node = node->NextSiblingElement("Option");
    }

    node = parentNode->FirstChildElement("Script");
    while (node)
    {
        if (node->Attribute("file"))
            target->AddBuildScript(cbC2U(node->Attribute("file")));

        node = node->NextSiblingElement("Script");
    }

    if (type != -1)
    {
        target->SetPlatforms(platformsFinal);
        target->SetCompilerID(compilerId);
        target->SetTargetFilenameGenerationPolicy(prefixPolicy, extensionPolicy);
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
        target->SetExecutionParameters(parameters);
        target->SetHostApplication(hostApplication);
        target->SetIncludeInTargetAll(includeInTargetAll); // used in versions prior to 1.5
        target->SetCreateDefFile(createDefFile);
        target->SetCreateStaticLib(createStaticLib);
        target->SetOptionRelation(ortCompilerOptions, (OptionsRelation)projectCompilerOptionsRelation);
        target->SetOptionRelation(ortLinkerOptions, (OptionsRelation)projectLinkerOptionsRelation);
        target->SetOptionRelation(ortIncludeDirs, (OptionsRelation)projectIncludeDirsRelation);
        target->SetOptionRelation(ortLibDirs, (OptionsRelation)projectLibDirsRelation);
        target->SetOptionRelation(ortResDirs, (OptionsRelation)projectResIncludeDirsRelation);

        DoMakeCommands(parentNode->FirstChildElement("MakeCommands"), target);
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
        wxString option = cbC2U(child->Attribute("option"));
        wxString dir = cbC2U(child->Attribute("directory"));
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
        wxString dir = cbC2U(child->Attribute("directory"));
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
        wxString option = cbC2U(child->Attribute("option"));
        wxString dir = UnixFilename(cbC2U(child->Attribute("directory")));
        wxString lib = UnixFilename(cbC2U(child->Attribute("library")));
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
        wxString option = UnixFilename(cbC2U(child->Attribute("option")));
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
        wxString option = cbC2U(child->Attribute("option"));
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
            wxString mode = cbC2U(child->Attribute("after"));
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
                before = cbC2U(child->Attribute("before"));
            if (child->Attribute("after"))
                after = cbC2U(child->Attribute("after"));

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
    TiXmlElement* node = parentNode->FirstChildElement("Environment");
    while (node)
    {
        TiXmlElement* child = node->FirstChildElement("Variable");
        while (child)
        {
            wxString name = cbC2U(child->Attribute("name"));
            wxString value = cbC2U(child->Attribute("value"));
            if (!name.IsEmpty())
            	base->SetVar(name, UnixFilename(value));

            child = child->NextSiblingElement("Variable");
        }
        node = node->NextSiblingElement("Environment");
    }
}

void ProjectLoader::DoUnits(TiXmlElement* parentNode)
{
    Manager::Get()->GetMessageManager()->DebugLog(_T("Loading project files..."));
    int count = 0;
    TiXmlElement* unit = parentNode->FirstChildElement("Unit");
    while (unit)
    {
        wxString filename = cbC2U(unit->Attribute("filename"));
        if (!filename.IsEmpty())
        {
            ProjectFile* file = m_pProject->AddFile(-1, UnixFilename(filename));
            if (!file)
                Manager::Get()->GetMessageManager()->DebugLog(_T("Can't load file '%s'"), filename.c_str());
            else
            {
                ++count;
                DoUnitOptions(unit, file);
            }
        }

        unit = unit->NextSiblingElement("Unit");
    }
    Manager::Get()->GetMessageManager()->DebugLog(_T("%d files loaded"), count);
}

void ProjectLoader::DoUnitOptions(TiXmlElement* parentNode, ProjectFile* file)
{
    int tempval = 0;
    bool foundCompile = false;
    bool foundLink = false;
    bool foundCompilerVar = false;
    bool foundTarget = false;

//    Compiler* compiler = CompilerFactory::GetCompiler(m_pProject->GetCompilerID());

    TiXmlElement* node = parentNode->FirstChildElement("Option");
    while (node)
    {
        if (node->Attribute("compilerVar"))
        {
            file->compilerVar = cbC2U(node->Attribute("compilerVar"));
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
        if (node->Attribute("virtualFolder"))
        {
            file->virtual_path = UnixFilename(cbC2U(node->Attribute("virtualFolder")));
        }
        //
        if (node->Attribute("buildCommand") && node->Attribute("compiler"))
        {
            wxString cmp = cbC2U(node->Attribute("compiler"));
            wxString tmp = cbC2U(node->Attribute("buildCommand"));
            if (!cmp.IsEmpty() && !tmp.IsEmpty())
            {
                tmp.Replace(_T("\\n"), _T("\n"));
                file->SetCustomBuildCommand(cmp, tmp);
                if (node->QueryIntAttribute("use", &tempval) == TIXML_SUCCESS)
                    file->SetUseCustomBuildCommand(cmp, tempval != 0);
            }
        }
        //
        if (node->Attribute("target"))
        {
            file->AddBuildTarget(cbC2U(node->Attribute("target")));
            foundTarget = true;
        }

        node = node->NextSiblingElement("Option");
    }

    // pre 1.6 versions upgrade
    if (m_IsPre_1_6)
    {
        // make sure the "compile" and "link" flags are honored
        if (!foundCompile)
            file->compile = true;
        if (!foundLink)
            file->link = true;
        if (!foundCompilerVar)
            file->compilerVar = _T("CPP");
    }

    if (!foundTarget)
    {
        // add to all targets
        for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
        {
            file->AddBuildTarget(m_pProject->GetBuildTarget(i)->GetTitle());
        }
    }
}

// convenience function, used in Save()
TiXmlElement* ProjectLoader::AddElement(TiXmlElement* parent, const char* name, const char* attr, const wxString& attribute)
{
    TiXmlElement elem(name);

    if (attr)
    {
        elem.SetAttribute(attr, cbU2C(attribute));
    }

    return parent->InsertEndChild(elem)->ToElement();
}

// convenience function, used in Save()
TiXmlElement* ProjectLoader::AddElement(TiXmlElement* parent, const char* name, const char* attr, int attribute)
{
    TiXmlElement elem(name);

    if (attr)
    {
        elem.SetAttribute(attr, attribute);
    }

    return parent->InsertEndChild(elem)->ToElement();
}

// convenience function, used in Save()
void ProjectLoader::AddArrayOfElements(TiXmlElement* parent, const char* name, const char* attr, const wxArrayString& array)
{
    if (!array.GetCount())
        return;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        if (array[i].IsEmpty())
            continue;
        AddElement(parent, name, attr, array[i]);
    }
}

// convenience function, used in Save()
void ProjectLoader::SaveEnvironment(TiXmlElement* parent, CompileOptionsBase* base)
{
    if (!base)
        return;
    const StringHash& v = base->GetAllVars();
    if (v.empty())
        return;
    TiXmlElement* node = AddElement(parent, "Environment");
    for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
    {
        TiXmlElement* elem = AddElement(node, "Variable", "name", it->first);
        elem->SetAttribute("value", cbU2C(it->second));
    }
}

bool ProjectLoader::Save(const wxString& filename)
{
    if (ExportTargetAsProject(filename, wxEmptyString))
    {
        m_pProject->SetModified(false);
        return true;
    }
    return false;
}

bool ProjectLoader::ExportTargetAsProject(const wxString& filename, const wxString& onlyTarget)
{
    const char* ROOT_TAG = "CodeBlocks_project_file";

    TiXmlDocument doc;
    doc.SetCondenseWhiteSpace(false);
    doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
    TiXmlElement* rootnode = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement(ROOT_TAG)));
    if (!rootnode)
        return false;

//    Compiler* compiler = CompilerFactory::GetCompiler(m_pProject->GetCompilerID());

    rootnode->InsertEndChild(TiXmlElement("FileVersion"));
    rootnode->FirstChildElement("FileVersion")->SetAttribute("major", PROJECT_FILE_VERSION_MAJOR);
    rootnode->FirstChildElement("FileVersion")->SetAttribute("minor", PROJECT_FILE_VERSION_MINOR);

    rootnode->InsertEndChild(TiXmlElement("Project"));
    TiXmlElement* prjnode = rootnode->FirstChildElement("Project");

    AddElement(prjnode, "Option", "title", m_pProject->GetTitle());
	if (m_pProject->GetPlatforms() != spAll)
	{
		wxString platforms = GetStringFromPlatforms(m_pProject->GetPlatforms());
		AddElement(prjnode, "Option", "platforms", platforms);
	}
    if (m_pProject->GetMakefile() != _T("Makefile"))
        AddElement(prjnode, "Option", "makefile", m_pProject->GetMakefile());
    if (m_pProject->IsMakefileCustom())
        AddElement(prjnode, "Option", "makefile_is_custom", 1);
    if (m_pProject->GetModeForPCH() != pchObjectDir)
        AddElement(prjnode, "Option", "pch_mode", (int)m_pProject->GetModeForPCH());
    if (!m_pProject->GetDefaultExecuteTarget().IsEmpty() && m_pProject->GetDefaultExecuteTarget() != m_pProject->GetFirstValidBuildTargetName())
        AddElement(prjnode, "Option", "default_target", m_pProject->GetDefaultExecuteTarget());
    AddElement(prjnode, "Option", "compiler", m_pProject->GetCompilerID());
    if (m_pProject->GetVirtualFolders().GetCount() > 0)
        AddElement(prjnode, "Option", "virtualFolders", GetStringFromArray(m_pProject->GetVirtualFolders(), _T(";")));
    if (m_pProject->GetExtendedObjectNamesGeneration())
        AddElement(prjnode, "Option", "extended_obj_names", 1);
    if (m_pProject->GetShowNotesOnLoad() || !m_pProject->GetNotes().IsEmpty())
    {
        TiXmlElement* notesBase = AddElement(prjnode, "Option", "show_notes", m_pProject->GetShowNotesOnLoad() ? 1 : 0);
        if (!m_pProject->GetNotes().IsEmpty())
        {
            TiXmlElement* notes = AddElement(notesBase, "notes");
            TiXmlText t(m_pProject->GetNotes().mb_str(wxConvUTF8));
            t.SetCDATA(true);
            notes->InsertEndChild(t);
        }
    }

    if (m_pProject->MakeCommandsModified())
    {
        TiXmlElement* makenode = AddElement(prjnode, "MakeCommands");
        AddElement(makenode, "Build", "command", m_pProject->GetMakeCommandFor(mcBuild));
        AddElement(makenode, "CompileFile", "command", m_pProject->GetMakeCommandFor(mcCompileFile));
        AddElement(makenode, "Clean", "command", m_pProject->GetMakeCommandFor(mcClean));
        AddElement(makenode, "DistClean", "command", m_pProject->GetMakeCommandFor(mcDistClean));
    }

    prjnode->InsertEndChild(TiXmlElement("Build"));
    TiXmlElement* buildnode = prjnode->FirstChildElement("Build");

    for (size_t x = 0; x < m_pProject->GetBuildScripts().GetCount(); ++x)
    {
        AddElement(buildnode, "Script", "file", m_pProject->GetBuildScripts().Item(x));
    }

    // now decide which target we 're exporting.
    // remember that if onlyTarget is empty, we export all targets (i.e. normal save).
    ProjectBuildTarget* onlytgt = m_pProject->GetBuildTarget(onlyTarget);

    for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = m_pProject->GetBuildTarget(i);
        if (!target)
            break;

        // skip every target except the desired one
        if (onlytgt && onlytgt != target)
            continue;

        TiXmlElement* tgtnode = AddElement(buildnode, "Target", "title", target->GetTitle());
        if (target->GetPlatforms() != spAll)
        {
        	wxString platforms = GetStringFromPlatforms(target->GetPlatforms());
            AddElement(tgtnode, "Option", "platforms", platforms);
        }
        if (target->GetTargetType() != ttCommandsOnly)
        {
            TiXmlElement* outnode = AddElement(tgtnode, "Option", "output", target->GetOutputFilename());
            TargetFilenameGenerationPolicy prefixPolicy;
            TargetFilenameGenerationPolicy extensionPolicy;
            target->GetTargetFilenameGenerationPolicy(&prefixPolicy, &extensionPolicy);
            outnode->SetAttribute("prefix_auto", prefixPolicy == tgfpPlatformDefault ? "1" : "0");
            outnode->SetAttribute("extension_auto", extensionPolicy == tgfpPlatformDefault ? "1" : "0");

            if (target->GetWorkingDir() != _T("."))
                AddElement(tgtnode, "Option", "working_dir", target->GetWorkingDir());
            if (target->GetObjectOutput() != _T(".objs"))
                AddElement(tgtnode, "Option", "object_output", target->GetObjectOutput());
            if (target->GetDepsOutput() != _T(".deps"))
                AddElement(tgtnode, "Option", "deps_output", target->GetDepsOutput());
        }
        if (!target->GetExternalDeps().IsEmpty())
            AddElement(tgtnode, "Option", "external_deps", target->GetExternalDeps());
        if (!target->GetAdditionalOutputFiles().IsEmpty())
            AddElement(tgtnode, "Option", "additional_output", target->GetAdditionalOutputFiles());
        AddElement(tgtnode, "Option", "type", target->GetTargetType());
        AddElement(tgtnode, "Option", "compiler", target->GetCompilerID());
        if (target->GetTargetType() == ttConsoleOnly && !target->GetUseConsoleRunner())
            AddElement(tgtnode, "Option", "use_console_runner", 0);
        if (!target->GetExecutionParameters().IsEmpty())
            AddElement(tgtnode, "Option", "parameters", target->GetExecutionParameters());
        if (!target->GetHostApplication().IsEmpty())
            AddElement(tgtnode, "Option", "host_application", target->GetHostApplication());
        // used in versions prior to 1.5
//        if (target->GetIncludeInTargetAll())
//            AddElement(tgtnode, "Option", "includeInTargetAll", 1);
        if ((target->GetTargetType() == ttStaticLib || target->GetTargetType() == ttDynamicLib) && target->GetCreateDefFile())
            AddElement(tgtnode, "Option", "createDefFile", 1);
        if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
            AddElement(tgtnode, "Option", "createStaticLib", 1);
        if (target->GetOptionRelation(ortCompilerOptions) != 3) // 3 is the default
            AddElement(tgtnode, "Option", "projectCompilerOptionsRelation", target->GetOptionRelation(ortCompilerOptions));
        if (target->GetOptionRelation(ortLinkerOptions) != 3) // 3 is the default
            AddElement(tgtnode, "Option", "projectLinkerOptionsRelation", target->GetOptionRelation(ortLinkerOptions));
        if (target->GetOptionRelation(ortIncludeDirs) != 3) // 3 is the default
            AddElement(tgtnode, "Option", "projectIncludeDirsRelation", target->GetOptionRelation(ortIncludeDirs));
        if (target->GetOptionRelation(ortResDirs) != 3) // 3 is the default
            AddElement(tgtnode, "Option", "projectResourceIncludeDirsRelation", target->GetOptionRelation(ortResDirs));
        if (target->GetOptionRelation(ortLibDirs) != 3) // 3 is the default
            AddElement(tgtnode, "Option", "projectLibDirsRelation", target->GetOptionRelation(ortLibDirs));

        for (size_t x = 0; x < target->GetBuildScripts().GetCount(); ++x)
        {
            AddElement(tgtnode, "Script", "file", target->GetBuildScripts().Item(x));
        }

        TiXmlElement* node = AddElement(tgtnode, "Compiler");
        AddArrayOfElements(node, "Add", "option", target->GetCompilerOptions());
        AddArrayOfElements(node, "Add", "directory", target->GetIncludeDirs());
        if (node->NoChildren())
            tgtnode->RemoveChild(node);

        node = AddElement(tgtnode, "ResourceCompiler");
        AddArrayOfElements(node, "Add", "directory", target->GetResourceIncludeDirs());
        if (node->NoChildren())
            tgtnode->RemoveChild(node);

        node = AddElement(tgtnode, "Linker");
        AddArrayOfElements(node, "Add", "option", target->GetLinkerOptions());
        AddArrayOfElements(node, "Add", "library", target->GetLinkLibs());
        AddArrayOfElements(node, "Add", "directory", target->GetLibDirs());
        if (node->NoChildren())
            tgtnode->RemoveChild(node);

        node = AddElement(tgtnode, "ExtraCommands");
        AddArrayOfElements(node, "Add", "before", target->GetCommandsBeforeBuild());
        AddArrayOfElements(node, "Add", "after", target->GetCommandsAfterBuild());
        if (node->NoChildren())
            tgtnode->RemoveChild(node);
        else
        {
            if (target->GetAlwaysRunPostBuildSteps())
                AddElement(node, "Mode", "after", wxString(_T("always")));
        }

        SaveEnvironment(tgtnode, target);

        if (target->MakeCommandsModified())
        {
            TiXmlElement* makenode = AddElement(tgtnode, "MakeCommands");
            AddElement(makenode, "Build", "command", target->GetMakeCommandFor(mcBuild));
            AddElement(makenode, "CompileFile", "command", target->GetMakeCommandFor(mcCompileFile));
            AddElement(makenode, "Clean", "command", target->GetMakeCommandFor(mcClean));
            AddElement(makenode, "DistClean", "command", target->GetMakeCommandFor(mcDistClean));
        }
    }

    // virtuals only for whole project
    if (onlyTarget.IsEmpty())
    {
        TiXmlElement* virtnode = AddElement(prjnode, "VirtualTargets");
        wxArrayString virtuals = m_pProject->GetVirtualBuildTargets();
        for (size_t i = 0; i < virtuals.GetCount(); ++i)
        {
            const wxArrayString& group = m_pProject->GetVirtualBuildTargetGroup(virtuals[i]);
            wxString groupStr = GetStringFromArray(group, _T(";"));
            if (!groupStr.IsEmpty())
            {
                TiXmlElement* elem = AddElement(virtnode, "Add", "alias", virtuals[i]);
                elem->SetAttribute("targets", cbU2C(groupStr));
            }
        }
        if (virtnode->NoChildren())
            prjnode->RemoveChild(virtnode);
    }

    SaveEnvironment(buildnode, m_pProject);

    TiXmlElement* node = AddElement(prjnode, "Compiler");
    AddArrayOfElements(node, "Add", "option", m_pProject->GetCompilerOptions());
    AddArrayOfElements(node, "Add", "directory", m_pProject->GetIncludeDirs());
    if (node->NoChildren())
        prjnode->RemoveChild(node);

    node = AddElement(prjnode, "ResourceCompiler");
    AddArrayOfElements(node, "Add", "directory", m_pProject->GetResourceIncludeDirs());
    if (node->NoChildren())
        prjnode->RemoveChild(node);

    node = AddElement(prjnode, "Linker");
    AddArrayOfElements(node, "Add", "option", m_pProject->GetLinkerOptions());
    AddArrayOfElements(node, "Add", "library", m_pProject->GetLinkLibs());
    AddArrayOfElements(node, "Add", "directory", m_pProject->GetLibDirs());
    if (node->NoChildren())
        prjnode->RemoveChild(node);

    node = AddElement(prjnode, "ExtraCommands");
    AddArrayOfElements(node, "Add", "before", m_pProject->GetCommandsBeforeBuild());
    AddArrayOfElements(node, "Add", "after", m_pProject->GetCommandsAfterBuild());
    if (node->NoChildren())
        prjnode->RemoveChild(node);
    else
    {
        if (m_pProject->GetAlwaysRunPostBuildSteps())
            AddElement(node, "Mode", "after", wxString(_T("always")));
    }

    int count = m_pProject->GetFilesCount();
    for (int i = 0; i < count; ++i)
    {
        ProjectFile* f = m_pProject->GetFile(i);

        // do not save project files that do not belong in the target we 're exporting
        if (onlytgt && !onlytgt->GetFilesList().Find(f))
            continue;

        FileType ft = FileTypeOf(f->relativeFilename);

        TiXmlElement* unitnode = AddElement(prjnode, "Unit", "filename", f->relativeFilename);
        if (!f->compilerVar.IsEmpty())
        {
            wxString ext = f->relativeFilename.AfterLast(_T('.')).Lower();
            if (f->compilerVar != _T("CC") && (ext.IsSameAs(FileFilters::C_EXT) || ext.IsSameAs(FileFilters::CC_EXT)))
                AddElement(unitnode, "Option", "compilerVar", f->compilerVar);
#ifdef __WXMSW__
            else if (f->compilerVar != _T("WINDRES") && ext.IsSameAs(FileFilters::RESOURCE_EXT))
                AddElement(unitnode, "Option", "compilerVar", f->compilerVar);
#endif
            else if (f->compilerVar != _T("CPP")) // default
                AddElement(unitnode, "Option", "compilerVar", f->compilerVar);
        }

        if (f->compile != (ft == ftSource || ft == ftResource))
        {
            AddElement(unitnode, "Option", "compile", f->compile ? 1 : 0);
        }
        if (f->link !=
                (ft == ftSource ||
                ft == ftResource ||
                ft == ftObject ||
                ft == ftResourceBin ||
                ft == ftStaticLib))
        {
            AddElement(unitnode, "Option", "link", f->link ? 1 : 0);
        }
        if (f->weight != 50)
            AddElement(unitnode, "Option", "weight", f->weight);
        if (!f->virtual_path.IsEmpty())
            AddElement(unitnode, "Option", "virtualFolder", f->virtual_path);

        // loop and save custom build commands
        for (pfCustomBuildMap::iterator it = f->customBuild.begin(); it != f->customBuild.end(); ++it)
        {
            pfCustomBuild& pfcb = it->second;
            if (!pfcb.buildCommand.IsEmpty())
            {
                wxString tmp = pfcb.buildCommand;
                tmp.Replace(_T("\n"), _T("\\n"));
                TiXmlElement* elem = AddElement(unitnode, "Option", "compiler", it->first);
                elem->SetAttribute("use", pfcb.useCustomBuildCommand ? "1" : "0");
                elem->SetAttribute("buildCommand", cbU2C(tmp));
            }
        }

        if ((int)f->buildTargets.GetCount() != m_pProject->GetBuildTargetsCount())
        {
            for (unsigned int x = 0; x < f->buildTargets.GetCount(); ++x)
                AddElement(unitnode, "Option", "target", f->buildTargets[x]);
        }
    }

    // as a last step, run all hooked callbacks
    if (ProjectLoaderHooks::HasRegisteredHooks())
    {
        TiXmlElement* node = AddElement(prjnode, "Extensions");
        if (node)
        {
            ProjectLoaderHooks::CallHooks(m_pProject, node, false);
        }
    }

    return cbSaveTinyXMLDocument(&doc, filename);
}

wxString ProjectLoader::GetValidCompilerID(const wxString& proposal, const wxString& scope)
{
    if (CompilerFactory::GetCompiler(proposal))
        return proposal;

    // check the map; maybe we asked the user before
    CompilerSubstitutes::iterator it = m_CompilerSubstitutes.find(proposal);
    if (it != m_CompilerSubstitutes.end())
        return it->second;

    Compiler* compiler = 0;

    // if compiler is a number, then this is an older version of the project file
    // propose the same compiler by index
    if (!proposal.IsEmpty())
    {
        long int idx = -1;
        if (proposal.ToLong(&idx))
			compiler = CompilerFactory::GetCompiler(idx);
    }

    if (!compiler)
    {
		if(!(Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/ignore_invalid_targets"), true)))
		{
			wxString msg;
			msg.Printf(_("The defined compiler for %s cannot be located (ID: %s).\n"
						"Please choose the compiler you want to use instead and click \"OK\".\n"
						"If you click \"Cancel\", the project/target will be excluded from the build."), scope.c_str(),
						proposal.c_str());
			compiler = CompilerFactory::SelectCompilerUI(msg);
		}
    }

    if (!compiler)
    {
		// allow for invalid compiler IDs to be preserved...
		m_CompilerSubstitutes[proposal] = proposal;
		return proposal;
    }

    m_OpenDirty = true;

    // finally, keep the user selection in the map so we don't ask him again
    m_CompilerSubstitutes[proposal] = compiler->GetID();
    return compiler->GetID();
}
