#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <compiler.h>
#include <cbproject.h>
#include <projectbuildtarget.h>
#include <globals.h>
#include <manager.h>
#include <messagemanager.h>
#include <macrosmanager.h>
#include "directcommands.h"
#include "compilergcc.h"
#include "makefilegenerator.h"
#include "customvars.h"
#include <depslib.h>

pfDetails::pfDetails(DirectCommands* cmds, ProjectBuildTarget* target, ProjectFile* pf)
{
    wxString sep = wxFileName::GetPathSeparator();
    wxFileName tmp;

    wxFileName prjbase(cmds->m_pProject->GetBasePath());

    source_file_native = pf->relativeFilename;
    source_file_absolute_native = pf->file.GetFullPath();

    tmp = pf->GetObjName();
    object_file_native = (target ? target->GetObjectOutput() : ".") +
                          sep +
                          tmp.GetFullPath();
    wxFileName o_file(object_file_native);
    o_file.MakeAbsolute(prjbase.GetFullPath());
    object_dir_native = o_file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    object_file_absolute_native = o_file.GetFullPath();
    tmp.SetExt("depend");
    dep_file_native = (target ? target->GetDepsOutput() : ".") +
                      sep +
                      tmp.GetFullPath();
    wxFileName d_file(dep_file_native);
    d_file.MakeAbsolute(prjbase.GetFullPath());
    dep_dir_native = d_file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    dep_file_absolute_native = o_file.GetFullPath();
    
    source_file = UnixFilename(source_file_native);
    cmds->QuoteStringIfNeeded(source_file);
    
    object_file = UnixFilename(object_file_native);
    cmds->QuoteStringIfNeeded(object_file);
    
    dep_file = UnixFilename(dep_file_native);
    cmds->QuoteStringIfNeeded(dep_file);
    
    object_dir = UnixFilename(object_dir_native);
    cmds->QuoteStringIfNeeded(object_dir);

    dep_dir = UnixFilename(dep_dir_native);
    cmds->QuoteStringIfNeeded(dep_dir);
}

DirectCommands::DirectCommands(CompilerGCC* compilerPlugin, Compiler* compiler, cbProject* project, int logPageIndex)
    : m_PageIndex(logPageIndex),
    m_pCompilerPlugin(compilerPlugin),
    m_pCompiler(compiler),
    m_pProject(project),
    m_pCurrTarget(0)
{
    //ctor
    depsStart();
    wxFileName cwd;
    cwd.Assign(m_pProject->GetBasePath());
    depsSetCWD(cwd.GetPath(wxPATH_GET_VOLUME).c_str());

    wxFileName fname(m_pProject->GetFilename());
    fname.SetExt("depend");
    depsCacheRead(fname.GetFullPath().c_str());
}

DirectCommands::~DirectCommands()
{
    //dtor
    struct depsStats stats;
    depsGetStats(&stats);
    if (stats.cache_updated)
    {
        wxFileName fname(m_pProject->GetFilename());
        fname.SetExt("depend");
        depsCacheWrite(fname.GetFullPath().c_str());
    }
    Manager::Get()->GetMessageManager()->DebugLog(
        _("Scanned %d files for #includes, cache used %d, cache updated %d"),
        stats.scanned, stats.cache_used, stats.cache_updated);

    depsDone();
}

// static
void DirectCommands::QuoteStringIfNeeded(wxString& str)
{
    if (!str.IsEmpty() && str.Find(' ') != -1 && str.GetChar(0) != '"')
        str = "\"" + str + "\"";
}

// static
void DirectCommands::AppendArray(const wxArrayString& from, wxArrayString& to)
{
    for (unsigned int i = 0; i < from.GetCount(); ++i)
    {
        to.Add(from[i]);
    }
}

void DirectCommands::AddCommandsToArray(const wxString& cmds, wxArrayString& array)
{
    wxString cmd = cmds;

    // macros and custom vars substitution
    CustomVars customvars(m_pCompilerPlugin);
    const VarsArray& vars = customvars.GetVars();
    for (unsigned int i = 0; i < vars.GetCount(); ++i)
    {
        Var& var = vars[i];
        cmd.Replace("$(" + var.name + ")", var.value);
    }
    Manager::Get()->GetMacrosManager()->ReplaceMacros(cmd, true);
    
    while (!cmd.IsEmpty())
    {
        int idx = cmd.Find("\n");
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

        pfDetails pfd(this, target, pf);
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

    pfDetails pfd(this, target, pf);
    
    MakefileGenerator mg(m_pCompilerPlugin, m_pProject, "", 0); // don't worry! we just need a couple of utility funcs from it
    
    // lookup file's type
    FileType ft = FileTypeOf(pf->relativeFilename);

    // create output dir
    if (!pfd.object_dir_native.IsEmpty() && !wxDirExists(pfd.object_dir_native))
    {
        if (!CreateDirRecursively(pfd.object_dir_native, 0755))
            wxMessageBox(_("Can't create object output directory ") + pfd.object_dir_native);
    }

    bool isResource = ft == ftResource;
#ifndef __WXMSW__
    // not supported under non-win32 platforms
    if (isResource)
        return ret;
#endif
    if (pf->useCustomBuildCommand)
    {
        wxString msg;
        msg.Printf(_("File %s has a custom build command set."
                    "This feature only works when using GNU \"make\""
                    "for the build process..."), pfd.source_file_native.c_str());
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + msg);
        return ret;
    }
    wxString compilerCmd;
    compilerCmd = mg.CreateSingleFileCompileCmd(isResource ? ctCompileResourceCmd : ctCompileObjectCmd,
                                                 target,
                                                 pf,
                                                 pfd.source_file,
                                                 pfd.object_file,
                                                 pfd.dep_file);

    if (!compilerCmd.IsEmpty())
    {
        Compiler* compiler = target ? CompilerFactory::Compilers[target->GetCompilerIndex()] : m_pCompiler;
        switch (compiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + compilerCmd);
                break;
            
            case clogSimple:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Compiling: ") + pfd.source_file_native);
                break;
            
            default:
                break;
        }
        AddCommandsToArray(compilerCmd, ret);
    }
    return ret;
}

wxArrayString DirectCommands::GetCompileCommands(ProjectBuildTarget* target, bool force)
{
    wxArrayString ret;

    if (target)
        ret = GetTargetCompileCommands(target, force);
    else
    {
        // add pre-build commands
        AppendArray(GetPreBuildCommands(0L), ret);

        size_t counter = ret.GetCount();
        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            if (bt->GetIncludeInTargetAll()) // only if target gets build with "all"
            {
                wxArrayString targetcompile = GetTargetCompileCommands(bt, force);
                AppendArray(targetcompile, ret);
            }
        }

        if (ret.GetCount() == counter  && !m_pProject->GetAlwaysRunPreBuildSteps())
            ret.Clear();

        // add post-build commands
        if (ret.GetCount() != 0 || m_pProject->GetAlwaysRunPostBuildSteps())
            AppendArray(GetPostBuildCommands(0L), ret);
    }
    return ret;
}

wxArrayString DirectCommands::GetTargetCompileCommands(ProjectBuildTarget* target, bool force)
{
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

    // add pre-build commands
    AppendArray(GetPreBuildCommands(target), ret);

    if (target->GetTargetType() == ttCommandsOnly)
    {
        // commands-only target
        AppendArray(GetPostBuildCommands(target), ret);
        return ret;
    }

    // set list of #include directories
    DepsSearchStart(target);

    // iterate all files of the project/target and add them to the build process
    size_t counter = ret.GetCount();
    MyFilesArray files = GetProjectFilesSortedByWeight(target, true, false);
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];
        pfDetails pfd(this, target, pf);
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

    if (ret.GetCount() == counter && !target->GetAlwaysRunPreBuildSteps())
        ret.Clear();

    // add post-build commands
    if (ret.GetCount() != counter || target->GetAlwaysRunPostBuildSteps())
        AppendArray(GetPostBuildCommands(target), ret);

    return ret;
}

wxArrayString DirectCommands::GetPreBuildCommands(ProjectBuildTarget* target)
{
    wxArrayString buildcmds = target ? target->GetCommandsBeforeBuild() : m_pProject->GetCommandsBeforeBuild();
    if (!buildcmds.IsEmpty())
    {
        Compiler* compiler = target ? CompilerFactory::Compilers[target->GetCompilerIndex()] : m_pCompiler;
        wxString title = target ? target->GetTitle() : m_pProject->GetTitle();
        switch (compiler->GetSwitches().logging)
        {
            case clogFull:
                {
                    wxArrayString tmp;
                    for (size_t i = 0; i < buildcmds.GetCount(); ++i)
                    {
                        wxArrayString tmp2;
                        AddCommandsToArray(buildcmds[i], tmp2);
                        for (size_t n = 0; n < tmp2.GetCount(); ++n)
                        {
                            tmp.Add(wxString(COMPILER_SIMPLE_LOG) + tmp2[n]);
                            tmp.Add(tmp2[n]);
                        }
                    }
                    buildcmds = tmp;
                }
                break;
            
            case clogSimple:
                buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running pre-build step: ") + title, 0);

            default:
                break;
        }
    }
    return buildcmds;
}

wxArrayString DirectCommands::GetPostBuildCommands(ProjectBuildTarget* target)
{
    wxArrayString buildcmds = target ? target->GetCommandsAfterBuild() : m_pProject->GetCommandsAfterBuild();
    if (!buildcmds.IsEmpty())
    {
        wxString title = target ? target->GetTitle() : m_pProject->GetTitle();
        switch (m_pCompiler->GetSwitches().logging)
        {
            case clogFull:
                {
                    wxArrayString tmp;
                    for (size_t i = 0; i < buildcmds.GetCount(); ++i)
                    {
                        wxArrayString tmp2;
                        AddCommandsToArray(buildcmds[i], tmp2);
                        for (size_t n = 0; n < tmp2.GetCount(); ++n)
                        {
                            tmp.Add(wxString(COMPILER_SIMPLE_LOG) + tmp2[n]);
                            tmp.Add(tmp2[n]);
                        }
                    }
                    buildcmds = tmp;
                }
                break;
            
            case clogSimple:
                buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running post-build step: ") + title, 0);

            default:
                break;
        }
    }
    return buildcmds;
}

wxArrayString DirectCommands::GetLinkCommands(ProjectBuildTarget* target, bool force)
{
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
    return ret;
}

wxArrayString DirectCommands::GetTargetLinkCommands(ProjectBuildTarget* target, bool force)
{
    wxLogNull ln;
    wxArrayString ret;

    MakefileGenerator mg(m_pCompilerPlugin, m_pProject, "", 0); // don't worry! we just need a couple of utility funcs from it
    wxFileName out = UnixFilename(target->GetOutputFilename());

    wxString output = target->GetOutputFilename();
    wxString linkfiles;
    wxString resfiles;

    time_t outputtime;
    depsTimeStamp(output.c_str(), &outputtime);
    if (!outputtime)
        force = true;
    if (AreExternalDepsOutdated(out.GetFullPath(), target->GetExternalDeps()))
        force = true;

    // get all the linkable objects for the target
    MyFilesArray files = GetProjectFilesSortedByWeight(target, false, true);
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];
        pfDetails pfd(this, target, pf);

        if (FileTypeOf(pf->relativeFilename) == ftResource)
            resfiles << pfd.object_file << " ";
        else
            linkfiles << pfd.object_file << " ";

        // timestamp check
        if (!force)
        {
            time_t objtime;
            depsTimeStamp(pfd.object_file_native.c_str(), &objtime);
            if (objtime > outputtime)
                force = true;
        }
    }

    if (!force)
        return ret;

    // create output dir
    out.MakeAbsolute(m_pProject->GetBasePath());
    wxString dstname = out.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!dstname.IsEmpty() && !wxDirExists(dstname))
    {
        if (!CreateDirRecursively(dstname, 0755))
            wxMessageBox(_("Can't create output directory ") + dstname);
    }

    // add actual link command
    wxString kind_of_output;
    CommandType ct;
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
        default: break;
    }
    wxString compilerCmd = mg.CreateSingleFileCompileCmd(ct, target, 0, "", linkfiles, resfiles);
    if (!compilerCmd.IsEmpty())
    {
        Compiler* compiler = target ? CompilerFactory::Compilers[target->GetCompilerIndex()] : m_pCompiler;
        switch (compiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + compilerCmd);
                break;
            
            default: // linker always simple log (if not full)
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Linking ") + kind_of_output + ": " + target->GetOutputFilename());
                break;
        }

        // for an explanation of the following, see GetTargetCompileCommands()
        if (target && ret.GetCount() != 0)
            ret.Add(wxString(COMPILER_TARGET_CHANGE) + target->GetTitle());

        AddCommandsToArray(compilerCmd, ret);
    }
    
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
        pfDetails pfd(this, target, pf);
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
bool DirectCommands::AreExternalDepsOutdated(const wxString& buildOutput, const wxString& externalDeps)
{
    // array is separated by ;
    wxArrayString deps = GetArrayFromString(externalDeps, ";");
    for (size_t i = 0; i < deps.GetCount(); ++i)
    {
        if (deps[i].IsEmpty())
            continue;

        time_t timeSrc;
        depsTimeStamp(deps[i].c_str(), &timeSrc);
        // if external dep doesn't exist, no need to relink
        if (!timeSrc)
            return false;

        time_t timeExe;
        depsTimeStamp(buildOutput.c_str(), &timeExe);
        // if build output doesn't exist, relink
        if (!timeExe)
            return false;
        
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
    depsTimeStamp(pfd.source_file_native.c_str(), &timeSrc);
    if (!timeSrc)
        return false;

    // If the object file does not exist, then it must be built. In this case
    // there is no need to scan the source file for headers.
    time_t timeObj;
    depsTimeStamp(pfd.object_file_native.c_str(), &timeObj);
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
    depsRef ref = depsScanForHeaders(pfd.source_file_native.c_str());
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

    const wxArrayString& prj_incs = m_pProject->GetIncludeDirs();
    const wxArrayString& tgt_incs = target->GetIncludeDirs();

    OptionsRelation relation = target->GetOptionRelation(ortIncludeDirs);
    switch (relation)
    {
        case orUseParentOptionsOnly:
            for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                depsAddSearchDir(prj_incs[i].c_str());
            break;
        case orUseTargetOptionsOnly:
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                depsAddSearchDir(tgt_incs[i].c_str());
            break;
        case orPrependToParentOptions:
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                depsAddSearchDir(tgt_incs[i].c_str());
            for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                depsAddSearchDir(prj_incs[i].c_str());
            break;
        case orAppendToParentOptions:
            for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                depsAddSearchDir(prj_incs[i].c_str());
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                depsAddSearchDir(tgt_incs[i].c_str());
            break;
    }

    // We could add the "global" compiler directories too, but we normally
    // don't care about the modification times of system include files.
}
