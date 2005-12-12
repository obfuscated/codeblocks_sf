#include <sdk.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/regex.h> // used in QUICK hack at line 574
#include <compiler.h>
#include <cbproject.h>
#include <projectbuildtarget.h>
#include <globals.h>
#include <manager.h>
#include <messagemanager.h>
#include <macrosmanager.h>
#include "directcommands.h"
#include "cmdlinegenerator.h"
#include "compilergcc.h"
#include "customvars.h"
#include "cbexception.h"
#include <depslib.h>

DirectCommands::DirectCommands(CompilerGCC* compilerPlugin,
                                CmdLineGenerator* generator,
                                Compiler* compiler,
                                cbProject* project,
                                int logPageIndex)
    : m_PageIndex(logPageIndex),
    m_pCompilerPlugin(compilerPlugin),
    m_pGenerator(generator),
    m_pCompiler(compiler),
    m_pProject(project),
    m_pCurrTarget(0)
{
    //ctor
    if (!m_pProject)
        return; // probably a compile file cmd without a project

    depsStart();
    wxFileName cwd;
    cwd.Assign(m_pProject->GetBasePath());
    depsSetCWD(cwd.GetPath(wxPATH_GET_VOLUME).mb_str());

    wxFileName fname(m_pProject->GetFilename());
    fname.SetExt(_T("depend"));
    depsCacheRead(fname.GetFullPath().mb_str());
}

DirectCommands::~DirectCommands()
{
    //dtor
    if (!m_pProject)
        return; // probably a compile file cmd without a project

    struct depsStats stats;
    depsGetStats(&stats);
    if (stats.cache_updated)
    {
        wxFileName fname(m_pProject->GetFilename());
        fname.SetExt(_T("depend"));
        depsCacheWrite(fname.GetFullPath().mb_str());
    }
    Manager::Get()->GetMessageManager()->DebugLog(
        _("Scanned %d files for #includes, cache used %d, cache updated %d"),
        stats.scanned, stats.cache_used, stats.cache_updated);

    depsDone();
}

void DirectCommands::AddCommandsToArray(const wxString& cmds, wxArrayString& array)
{
    wxString cmd = cmds;
    while (!cmd.IsEmpty())
    {
        int idx = cmd.Find(_T("\n"));
        wxString cmdpart = idx != -1 ? cmd.Left(idx) : cmd;
        cmdpart.Trim(false);
        cmdpart.Trim(true);
        if (!cmdpart.IsEmpty())
            array.Add(cmdpart);
        if (idx == -1)
            break;
        cmd.Remove(0, idx + 1);
    }
}

int MySortProjectFilesByWeight(ProjectFile** one, ProjectFile** two)
{
    return (*one)->weight - (*two)->weight;
}

MyFilesArray DirectCommands::GetProjectFilesSortedByWeight(ProjectBuildTarget* target, bool compile, bool link)
{
    MyFilesArray files;
    for (int i = 0; i < m_pProject->GetFilesCount(); ++i)
    {
        ProjectFile* pf = m_pProject->GetFile(i);
        // require compile
        if (compile && !pf->compile)
            continue;
        // require link
        if (link && !pf->link)
            continue;
        // if the file does not belong in this target (if we have a target), skip it
        if (target && (pf->buildTargets.Index(target->GetTitle()) == wxNOT_FOUND))
            continue;
        files.Add(pf);
    }
    files.Sort(MySortProjectFilesByWeight);
    return files;
}

wxArrayString DirectCommands::CompileFile(ProjectBuildTarget* target, ProjectFile* pf, bool force)
{
    wxArrayString ret;

    // is it compilable?
    if (!pf->compile || pf->compilerVar.IsEmpty())
        return ret;

    if (!force)
    {
        DepsSearchStart(target);

        const pfDetails& pfd = pf->GetFileDetails(target);
        if (!IsObjectOutdated(pfd))
            return ret;
    }

    if (target)
        ret.Add(wxString(COMPILER_TARGET_CHANGE) + target->GetTitle());
    AppendArray(GetCompileFileCommand(target, pf), ret);
    return ret;
}

wxArrayString DirectCommands::GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf)
{
    wxLogNull ln;
    wxArrayString ret;

    // is it compilable?
    if (!pf->compile || pf->compilerVar.IsEmpty())
        return ret;

    const pfDetails& pfd = pf->GetFileDetails(target);

    // lookup file's type
    FileType ft = FileTypeOf(pf->relativeFilename);

    // create output dir
    if (!pfd.object_dir_native.IsEmpty() && !wxDirExists(pfd.object_dir_native))
    {
        if (!CreateDirRecursively(pfd.object_dir_native, 0755))
            wxMessageBox(_("Can't create object output directory ") + pfd.object_dir_native);
    }

    bool isResource = ft == ftResource;
    bool isHeader = ft == ftHeader;
#ifndef __WXMSW__
    // not supported under non-win32 platforms
    if (isResource)
        return ret;
#endif
    Compiler* compiler = target ? CompilerFactory::Compilers[target->GetCompilerIndex()] : m_pCompiler;
    wxString compilerCmd = pf->useCustomBuildCommand
                            ? pf->buildCommand
                            : compiler->GetCommand(isResource ? ctCompileResourceCmd : ctCompileObjectCmd);
    m_pGenerator->CreateSingleFileCompileCmd(compilerCmd,
                                             target,
                                             pf,
                                             pfd.source_file,
                                             pfd.object_file,
                                             pfd.dep_file);

    if (!compilerCmd.IsEmpty())
    {
        switch (compiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + compilerCmd);
                break;

            case clogSimple:
                if (isHeader)
                    ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Precompiling header: ") + pfd.source_file_native);
                else
                    ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Compiling: ") + pfd.source_file_native);
                break;

            default:
                break;
        }
        AddCommandsToArray(compilerCmd, ret);

        // if it's a PCH, delete the previously generated PCH to avoid problems
        // (it 'll be recreated anyway)
        if (FileTypeOf(pf->relativeFilename) == ftHeader && pf->compile)
        {
            wxRemoveFile(pfd.object_file_absolute_native);
        }
    }
    return ret;
}

/// This is to be used *only* for files not belonging to a project!!!
wxArrayString DirectCommands::GetCompileSingleFileCommand(const wxString& filename)
{
    wxLogNull ln;
    wxArrayString ret;

    // lookup file's type
    FileType ft = FileTypeOf(filename);

    // is it compilable?
    if (ft != ftSource)
        return ret;

    wxFileName fname(filename);
    fname.SetExt(m_pCompiler->GetSwitches().objectExtension);
    wxString o_filename = fname.GetFullPath();
    fname.SetExt(EXECUTABLE_EXT);
    wxString exe_filename = fname.GetFullPath();

    wxString s_filename = filename;
    QuoteStringIfNeeded(s_filename);
    QuoteStringIfNeeded(o_filename);

    Compiler* compiler = CompilerFactory::GetDefaultCompiler();
    wxString compilerCmd = compiler->GetCommand(ctCompileObjectCmd);
    m_pGenerator->CreateSingleFileCompileCmd(compilerCmd,
                                             0,
                                             0,
                                             s_filename,
                                             o_filename,
                                             wxEmptyString);
    wxString linkerCmd = compiler->GetCommand(ctLinkConsoleExeCmd);
    m_pGenerator->CreateSingleFileCompileCmd(linkerCmd,
                                             0,
                                             0,
                                             wxEmptyString,
                                             o_filename,
                                             wxEmptyString);

    if (!compilerCmd.IsEmpty())
    {
        switch (m_pCompiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + compilerCmd);
                break;

            case clogSimple:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Compiling: ") + filename);
                break;

            default:
                break;
        }
        AddCommandsToArray(compilerCmd, ret);
    }

    if (!linkerCmd.IsEmpty())
    {
        switch (m_pCompiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + linkerCmd);
                break;

            default:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Linking console executable: ") + exe_filename);
                break;
        }
        AddCommandsToArray(linkerCmd, ret);
    }
    return ret;
}

wxArrayString DirectCommands::GetCompileCommands(ProjectBuildTarget* target, bool force)
{
    wxArrayString ret;

    m_pProject->SetCurrentlyCompilingTarget(0);

    // add project pre-build commands
    AppendArray(GetPreBuildCommands(0L), ret);
    size_t counter = ret.GetCount();

    if (target)
        ret = GetTargetCompileCommands(target, force);
    else
    {
        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            if (bt->GetIncludeInTargetAll()) // only if target gets build with "all"
            {
                wxArrayString targetcompile = GetTargetCompileCommands(bt, force);
                AppendArray(targetcompile, ret);
            }
        }
	}

    // remove pre-build commands if no compile needed and not always run pre-build commands
    bool needPost = ret.GetCount() != counter;
    if (!needPost  && !m_pProject->GetAlwaysRunPreBuildSteps())
        ret.Clear();

    m_pProject->SetCurrentlyCompilingTarget(0);
    return ret;
}

wxArrayString DirectCommands::GetTargetCompileCommands(ProjectBuildTarget* target, bool force)
{
    m_pProject->SetCurrentlyCompilingTarget(target);

//    target->GetCustomVars().ApplyVarsToEnvironment();

    wxArrayString ret;
    ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Switching to target: ") + target->GetTitle());
    // NOTE: added this to notify compiler about the active target.
    // this is needed when targets use different compiler each
    // and C::B tries to parse the compiler's output.
    // previous behaviour, used the project's compiler for parsing
    // all targets output, which failed when a target's compiler
    // was different than the project's...
    ret.Add(wxString(COMPILER_TARGET_CHANGE) + target->GetTitle());

    m_pCurrTarget = target;

    // make sure all project files are saved
    if (!m_pProject->SaveAllFiles())
        wxMessageBox(_("Could not save all files. Build might be incomplete..."));

    // add target pre-build commands
    AppendArray(GetPreBuildCommands(target), ret);

    // set list of #include directories
    DepsSearchStart(target);

    // iterate all files of the project/target and add them to the build process
    size_t counter = ret.GetCount();
    MyFilesArray files = GetProjectFilesSortedByWeight(target, true, false);
    size_t fcount = files.GetCount();
    for (unsigned int i = 0; i < fcount; ++i)
    {
        ProjectFile* pf = files[i];
        const pfDetails& pfd = pf->GetFileDetails(target);
        bool doBuild = false;

        if (pf->autoDeps)
            doBuild = force || IsObjectOutdated(pfd);
        else
        {
            wxString msg;
            msg.Printf(_("File %s has custom dependencies set."
                        "This feature only works when using GNU \"make\""
                        "for the build process..."), pfd.source_file_native.c_str());
            ret.Add(wxString(COMPILER_SIMPLE_LOG) + msg);
        }
        if (doBuild)
        {
            // compile file
            wxArrayString filecmd = GetCompileFileCommand(target, pf);
            AppendArray(filecmd, ret);
        }
    }

    // add link command
    wxArrayString link = GetLinkCommands(target, ret.GetCount() != counter);
    AppendArray(link, ret);

    // remove pre-build commands if no compile needed and not always run pre-build commands
    bool needPost = ret.GetCount() != counter;
    if (!needPost && !target->GetAlwaysRunPreBuildSteps())
        ret.Clear();

    // force post-build step, if needed
    if (!ret.GetCount() && target->GetAlwaysRunPostBuildSteps())
        AppendArray(GetPostBuildCommands(target), ret);

    m_pProject->SetCurrentlyCompilingTarget(0);
    return ret;
}

wxArrayString DirectCommands::GetPreBuildCommands(ProjectBuildTarget* target)
{
    m_pProject->SetCurrentlyCompilingTarget(target);
//    m_pProject->GetCustomVars().ApplyVarsToEnvironment();
//    if (target)
//        target->GetCustomVars().ApplyVarsToEnvironment();

    wxArrayString buildcmds = target ? target->GetCommandsBeforeBuild() : m_pProject->GetCommandsBeforeBuild();
    if (!buildcmds.IsEmpty())
    {
        wxString title = target ? target->GetTitle() : m_pProject->GetTitle();
        wxArrayString tmp;
        for (size_t i = 0; i < buildcmds.GetCount(); ++i)
        {
            m_pGenerator->CreateSingleFileCompileCmd(buildcmds[i], target, 0, wxEmptyString, wxEmptyString, wxEmptyString);
            tmp.Add(wxString(COMPILER_SIMPLE_LOG) + buildcmds[i]);
            tmp.Add(buildcmds[i]);
        }
        buildcmds = tmp;
        buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running pre-build step: ") + title, 0);
    }
    m_pProject->SetCurrentlyCompilingTarget(0);
    return buildcmds;
}

wxArrayString DirectCommands::GetPostBuildCommands(ProjectBuildTarget* target)
{
    m_pProject->SetCurrentlyCompilingTarget(target);
//    m_pProject->GetCustomVars().ApplyVarsToEnvironment();
//    if (target)
//        target->GetCustomVars().ApplyVarsToEnvironment();

    wxArrayString buildcmds = target ? target->GetCommandsAfterBuild() : m_pProject->GetCommandsAfterBuild();
    if (!buildcmds.IsEmpty())
    {
        wxString title = target ? target->GetTitle() : m_pProject->GetTitle();
        wxArrayString tmp;
        for (size_t i = 0; i < buildcmds.GetCount(); ++i)
        {
            m_pGenerator->CreateSingleFileCompileCmd(buildcmds[i], target, 0, wxEmptyString, wxEmptyString, wxEmptyString);
            tmp.Add(wxString(COMPILER_SIMPLE_LOG) + buildcmds[i]);
            tmp.Add(buildcmds[i]);
        }
        buildcmds = tmp;
        buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running post-build step: ") + title, 0);
    }
    m_pProject->SetCurrentlyCompilingTarget(0);
    return buildcmds;
}

wxArrayString DirectCommands::GetLinkCommands(ProjectBuildTarget* target, bool force)
{
    m_pProject->SetCurrentlyCompilingTarget(0);
    wxArrayString ret;

    if (target)
        ret = GetTargetLinkCommands(target, force);
    else
    {
        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            if (bt->GetIncludeInTargetAll()) // only if target gets build with "all"
            {
                wxArrayString targetlink = GetTargetLinkCommands(bt, force);
                AppendArray(targetlink, ret);
            }
        }
    }

    // add project post-build commands
    if (ret.GetCount() != 0  || m_pProject->GetAlwaysRunPostBuildSteps())
        AppendArray(GetPostBuildCommands(0L), ret);

    m_pProject->SetCurrentlyCompilingTarget(0);
    return ret;
}

wxArrayString DirectCommands::GetTargetLinkCommands(ProjectBuildTarget* target, bool force)
{
    m_pProject->SetCurrentlyCompilingTarget(target);
//    target->GetCustomVars().ApplyVarsToEnvironment();

    wxLogNull ln;
    wxArrayString ret;

    wxString output = target->GetOutputFilename();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(output);

    wxFileName out = UnixFilename(output);
    wxString linkfiles;
    wxString resfiles;

    time_t outputtime;
    depsTimeStamp(output.mb_str(), &outputtime);
    if (!outputtime)
        force = true;
    if (AreExternalDepsOutdated(out.GetFullPath(), target->GetAdditionalOutputFiles(), target->GetExternalDeps()))
        force = true;

    wxString prependHack; // part of the following hack
    if (target->GetTargetType() == ttStaticLib)
    {
        // QUICK HACK: some linkers (e.g. bcc, dmc) require a - or + in front of
        // object files for static library. What we 'll do here until we redesign
        // the thing, is to accept this symbol as part of the $link_objects macro
        // like this:
        // $+link_objects
        // $-link_objects
        // $-+link_objects
        // $+-link_objects
        //
        // So, we first scan the command for this special case and, if found,
        // set a flag so that the linkfiles array is filled with the correct options
        Compiler* compiler = target ? CompilerFactory::Compilers[target->GetCompilerIndex()] : m_pCompiler;
        wxString compilerCmd = compiler->GetCommand(ctLinkStaticCmd);
        wxRegEx re(_T("\\$([-+]+)link_objects"));
        if (re.Matches(compilerCmd))
            prependHack = re.GetMatch(compilerCmd, 1);
    }

    // get all the linkable objects for the target
    MyFilesArray files = GetProjectFilesSortedByWeight(target, false, true);
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];
        const pfDetails& pfd = pf->GetFileDetails(target);

        if (FileTypeOf(pf->relativeFilename) == ftResource)
            resfiles << pfd.object_file << _T(" ");
        else
            linkfiles << prependHack << pfd.object_file << _T(" "); // see QUICK HACK above (prependHack)

        // timestamp check
        if (!force)
        {
            time_t objtime;
            depsTimeStamp(pfd.object_file_native.mb_str(), &objtime);
            if (objtime > outputtime)
                force = true;
        }
    }

    if (!force)
        return ret;

    // create output dir
    out.MakeAbsolute(m_pProject->GetBasePath());
    wxString dstname = out.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dstname);
    if (!dstname.IsEmpty() && !wxDirExists(dstname))
    {
        if (!CreateDirRecursively(dstname, 0755))
            wxMessageBox(_("Can't create output directory ") + dstname);
    }

    // add actual link command
    wxString kind_of_output;
    CommandType ct = ctInvalid; // get rid of compiler warning
    switch (target->GetTargetType())
    {
        case ttConsoleOnly:
            ct = ctLinkConsoleExeCmd;
            kind_of_output = _("console executable");
            break;

        case ttExecutable:
            ct = ctLinkExeCmd;
            kind_of_output = _("executable");
            break;

        case ttDynamicLib:
            ct = ctLinkDynamicCmd;
            kind_of_output = _("dynamic library");
            break;

        case ttStaticLib:
            ct = ctLinkStaticCmd;
            kind_of_output = _("static library");
            break;
		case ttCommandsOnly:
            // add target post-build commands
            ret.Clear();
            AppendArray(GetPostBuildCommands(target), ret);
			return ret;
			break;
        default:
			wxString ex;
			ex.Printf(_T("Encountered invalid TargetType (value = %d)"), target->GetTargetType());
			cbThrow(ex);
        break;
    }
    Compiler* compiler = target ? CompilerFactory::Compilers[target->GetCompilerIndex()] : m_pCompiler;
    wxString compilerCmd = compiler->GetCommand(ct);
    m_pGenerator->CreateSingleFileCompileCmd(compilerCmd,
                                             target,
                                             0,
                                             _T(""),
                                             linkfiles,
                                             resfiles);
    if (!compilerCmd.IsEmpty())
    {
        switch (compiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + compilerCmd);
                break;

            default: // linker always simple log (if not full)
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Linking ") + kind_of_output + _T(": ") + output);
                break;
        }

        // for an explanation of the following, see GetTargetCompileCommands()
        if (target && ret.GetCount() != 0)
            ret.Add(wxString(COMPILER_TARGET_CHANGE) + target->GetTitle());

        AddCommandsToArray(compilerCmd, ret);
    }

    // add target post-build commands
    if (ret.GetCount() != 0  || target->GetAlwaysRunPostBuildSteps())
        AppendArray(GetPostBuildCommands(target), ret);

    m_pProject->SetCurrentlyCompilingTarget(0);
    return ret;
}

wxArrayString DirectCommands::GetCleanCommands(ProjectBuildTarget* target, bool distclean)
{
    wxArrayString ret;

    if (target)
        ret = GetTargetCleanCommands(target);
    else
    {
        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            wxArrayString targetclear = GetTargetCleanCommands(bt, distclean);
            AppendArray(targetclear, ret);
        }
    }
    return ret;
}

wxArrayString DirectCommands::GetTargetCleanCommands(ProjectBuildTarget* target, bool distclean)
{
    wxArrayString ret;

    // add object files
    MyFilesArray files = GetProjectFilesSortedByWeight(target, true, false);
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];
        const pfDetails& pfd = pf->GetFileDetails(target);
        ret.Add(pfd.object_file_absolute_native);
        if (distclean)
            ret.Add(pfd.dep_file_absolute_native);
    }

    // add target output
    wxString outputfilename = target->GetOutputFilename();

    if (target->GetTargetType() != ttCommandsOnly)
    {
        ret.Add(outputfilename);
    }

    if (target->GetTargetType() == ttDynamicLib)
    {
        // for dynamic libs, delete static lib
        ret.Add(target->GetStaticLibFilename());
        // .def exports file is not deleted, because it may be user-supplied
//        ret.Add(target->GetDynamicLibDefFilename());
    }

    return ret;
}

/** external deps are manualy set by the user
  * e.g. a static library linked to the project is an external dep (if set as such by the user)
  * so that a re-linking is forced if the static lib is updated
  */
bool DirectCommands::AreExternalDepsOutdated(const wxString& buildOutput, const wxString& additionalFiles, const wxString& externalDeps)
{
    // array is separated by ;
    wxArrayString deps = GetArrayFromString(externalDeps, _T(";"));
    wxArrayString files = GetArrayFromString(additionalFiles, _T(";"));
    for (size_t i = 0; i < deps.GetCount(); ++i)
    {
        if (deps[i].IsEmpty())
            continue;

        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(deps[i]);
        time_t timeSrc;
        depsTimeStamp(deps[i].mb_str(), &timeSrc);
        // if external dep doesn't exist, no need to relink
        if (!timeSrc)
            return false;

        // let's check the additional output files
        for (size_t x = 0; x < files.GetCount(); ++x)
        {
        	if (files[i].IsEmpty())
                continue;

            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(files[i]);
            time_t addT;
            depsTimeStamp(files[i].mb_str(), &addT);
            // if additional file doesn't exist, we can skip it
            if (!addT)
                continue;

            // if external dep is newer than additional file, relink
            if (timeSrc > addT)
                return true;
        }

        // if no output, probably a commands-only target; nothing to relink
        if (buildOutput.IsEmpty())
            return false;

        // now check the target's output
        // this is moved last because, for "commands only" targets,
        // it would return before we had a chance to check the
        // additional output files (above)
        wxString output = buildOutput;
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(output);
        time_t timeExe;
        depsTimeStamp(output.mb_str(), &timeExe);
        // if build output doesn't exist, relink
        if (!timeExe)
            return true;

        // if external dep is newer than build output, relink
        if (timeSrc > timeExe)
            return true;
    }
    return false; // no force relink
}

bool DirectCommands::IsObjectOutdated(const pfDetails& pfd)
{
    // If the source file does not exist, then do not compile.
    time_t timeSrc;
    depsTimeStamp(pfd.source_file_absolute_native.mb_str(), &timeSrc);
    if (!timeSrc)
        return false;

    // If the object file does not exist, then it must be built. In this case
    // there is no need to scan the source file for headers.
    time_t timeObj;
    depsTimeStamp(pfd.object_file_absolute_native.mb_str(), &timeObj);
    if (!timeObj)
        return true;

    // If the source file is newer than the object file, then the object file
    // must be built. In this case there is no need to scan the source file
    // for headers.
    if (timeSrc > timeObj)
        return true;

    // Scan the source file for headers. Result is NULL if the file does
    // not exist. If one of the descendent header files is newer than the
    // object file, then the object file must be built.
    depsRef ref = depsScanForHeaders(pfd.source_file_absolute_native.mb_str());
    if (ref)
    {
        time_t timeNewest;
        (void) depsGetNewest(ref, &timeNewest);
        return (timeNewest > timeObj);
    }

    // Source file doesn't exist.
    return false;
}

void DirectCommands::DepsSearchStart(ProjectBuildTarget* target)
{
    depsSearchStart();

    wxArrayString prj_incs = m_pProject->GetIncludeDirs();
    wxArrayString tgt_incs = target->GetIncludeDirs();

    // replace custom vars in include dirs
    for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(prj_incs[i]);
    for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(tgt_incs[i]);

    OptionsRelation relation = target->GetOptionRelation(ortIncludeDirs);
    switch (relation)
    {
        case orUseParentOptionsOnly:
            for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                depsAddSearchDir(prj_incs[i].mb_str());
            break;
        case orUseTargetOptionsOnly:
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                depsAddSearchDir(tgt_incs[i].mb_str());
            break;
        case orPrependToParentOptions:
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                depsAddSearchDir(tgt_incs[i].mb_str());
            for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                depsAddSearchDir(prj_incs[i].mb_str());
            break;
        case orAppendToParentOptions:
            for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                depsAddSearchDir(prj_incs[i].mb_str());
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                depsAddSearchDir(tgt_incs[i].mb_str());
            break;
    }

    // We could add the "global" compiler directories too, but we normally
    // don't care about the modification times of system include files.
}
