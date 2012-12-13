/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
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
#include <logmanager.h>
#include <macrosmanager.h>
#include "directcommands.h"
#include "compilercommandgenerator.h"
#include "compilergcc.h"
#include "cbexception.h"
#include "filefilters.h"
#include <depslib.h>

DirectCommands::DirectCommands(CompilerGCC* compilerPlugin,
                               Compiler*    compiler,
                               cbProject*   project,
                               int          logPageIndex) :
    m_doYield(false),
    m_PageIndex(logPageIndex),
    m_pCompilerPlugin(compilerPlugin),
    m_pCompiler(compiler),
    m_pProject(project),
    m_pCurrTarget(0),
    m_pGenerator(0)
{
    // even if there is no project, the command generator need to be
    // initialised for single file compilation to work.
    // it can handle a NULL pointer argument... ;-)
    m_pGenerator = m_pCompiler->GetCommandGenerator(m_pProject);

    // ctor
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
    // dtor
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

    Manager::Get()->GetLogManager()->DebugLog(
        F(_("Scanned %ld files for #includes, cache used %ld, cache updated %ld"),
        stats.scanned, stats.cache_used, stats.cache_updated));

    depsDone();

    delete m_pGenerator;
}

void DirectCommands::AddCommandsToArray(const wxString& cmds, wxArrayString& array, bool isWaitCmd, bool isLinkCmd)
{
    wxString cmd = cmds;
    while (!cmd.IsEmpty())
    {
        int idx = cmd.Find(_T("\n"));
        wxString cmdpart = idx != -1 ? cmd.Left(idx) : cmd;
        cmdpart.Trim(false);
        cmdpart.Trim(true);
        if (!cmdpart.IsEmpty())
        {
            if (isWaitCmd)
                array.Add(wxString(COMPILER_WAIT));
            if (isLinkCmd)
                array.Add(wxString(COMPILER_WAIT_LINK));
            array.Add(cmdpart);
        }
        if (idx == -1)
            break;
        cmd.Remove(0, idx + 1);
    }
}

static int MySortProjectFilesByWeight(ProjectFile** one, ProjectFile** two)
{
    int diff = (*one)->weight - (*two)->weight;
    diff = (diff == 0 ? (*one)->relativeFilename.CmpNoCase((*two)->relativeFilename) : diff);
    return (diff == 0 ? (*one)->relativeFilename.Cmp((*two)->relativeFilename) : diff);
}

MyFilesArray DirectCommands::GetProjectFilesSortedByWeight(ProjectBuildTarget* target, bool compile, bool link)
{
    MyFilesArray files;
    for (FilesList::iterator it = m_pProject->GetFilesList().begin(); it != m_pProject->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
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
        wxString err;
        if (!IsObjectOutdated(target, pfd, &err))
        {
            if (!err.IsEmpty())
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + err);
            return ret;
        }
    }

    if (target)
        ret.Add(wxString(COMPILER_TARGET_CHANGE) + target->GetTitle());
    AppendArray(GetCompileFileCommand(target, pf), ret);
    return ret;
}

wxArrayString DirectCommands::GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf)
{
    wxArrayString ret;
    wxArrayString ret_generated;

    // is it compilable?
    if (!pf || !pf->compile)
        return ret;

    if (pf->compilerVar.IsEmpty())
    {
        Manager::Get()->GetLogManager()->DebugLog(_("Cannot resolve compiler var for project file."));
        return ret;
    }

    Compiler* compiler = target
                       ? CompilerFactory::GetCompiler(target->GetCompilerID())
                       : m_pCompiler;
    if (!compiler)
    {
        Manager::Get()->GetLogManager()->DebugLog(_("Can't access compiler for file."));
        return ret;
    }

    const pfDetails& pfd = pf->GetFileDetails(target);
    wxString object      = (compiler->GetSwitches().UseFlatObjects)
                         ? pfd.object_file_flat : pfd.object_file;
    wxString object_dir  = (compiler->GetSwitches().UseFlatObjects)
                         ? pfd.object_dir_flat_native : pfd.object_dir_native;
    // create output dir
    if (!object_dir.IsEmpty() && !CreateDirRecursively(object_dir, 0755))
        Manager::Get()->GetLogManager()->DebugLog(_("Can't create object output directory:\n") + object_dir);

    // lookup file's type
    const FileType ft = FileTypeOf(pf->relativeFilename);

    bool is_resource = ft == ftResource;
    bool is_header   = ft == ftHeader;

    // allowed resources under all platforms: makes sense when cross-compiling for
    // windows under linux.
    // and anyway, if the user is dumb enough to try to compile resources without
    // having a resource compiler, (s)he deserves the upcoming build error ;)

    wxString compiler_cmd;
    if (!is_header || compiler->GetSwitches().supportsPCH)
    {
        const CompilerTool& tool = compiler->GetCompilerTool(is_resource ? ctCompileResourceCmd : ctCompileObjectCmd, pf->file.GetExt());

        // does it generate other files to compile?
        for (size_t i = 0; i < pf->generatedFiles.size(); ++i)
            AppendArray(GetCompileFileCommand(target, pf->generatedFiles[i]), ret_generated); // recurse

        pfCustomBuild& pcfb = pf->customBuild[compiler->GetID()];
        compiler_cmd = pcfb.useCustomBuildCommand
                     ? pcfb.buildCommand : tool.command;

        wxString source_file;
        if (compiler->GetSwitches().UseFullSourcePaths)
            source_file = UnixFilename(pfd.source_file_absolute_native);
        else
            source_file = pfd.source_file;

#ifdef command_line_generation
    Manager::Get()->GetLogManager()->DebugLog(F(_T("GetCompileFileCommand[1]: compiler_cmd='%s', source_file='%s', object='%s', object_dir='%s'."),
                                                compiler_cmd.wx_str(), source_file.wx_str(), object.wx_str(), object_dir.wx_str()));
#endif

        // for resource files, use short from if path because if windres bug with spaces-in-paths
        if (is_resource && compiler->GetSwitches().UseFullSourcePaths)
            source_file = pf->file.GetShortPath();

        QuoteStringIfNeeded(source_file);

#ifdef command_line_generation
        Manager::Get()->GetLogManager()->DebugLog(F(_T("GetCompileFileCommand[2]: source_file='%s'."),
                                                    source_file.wx_str()));
#endif

        m_pGenerator->GenerateCommandLine(compiler_cmd,
                                          target,
                                          pf,
                                          source_file,
                                          object,
                                          pfd.object_file_flat,
                                          pfd.dep_file);
    }

    if (!is_header && compiler_cmd.IsEmpty())
    {
        ret.Add(  wxString(COMPILER_SIMPLE_LOG)
                + _("Skipping file (no compiler program set): ")
                + pfd.source_file_native );
        return ret;
    }

    switch (compiler->GetSwitches().logging)
    {
        case clogFull:
            ret.Add(wxString(COMPILER_SIMPLE_LOG) + compiler_cmd);
            break;

        case clogSimple:
            if (is_header)
                ret.Add(  wxString(COMPILER_SIMPLE_LOG)
                        + _("Pre-compiling header: ")
                        + pfd.source_file_native );
            else
                ret.Add(  wxString(COMPILER_SIMPLE_LOG)
                        + _("Compiling: ")
                        + pfd.source_file_native );
            break;

        default:
            break;
    }

    AddCommandsToArray(compiler_cmd, ret);

    if (is_header)
        ret.Add(wxString(COMPILER_WAIT));

    if (ret_generated.GetCount())
    {
        // not only append commands for (any) generated files to be compiled
        // but also insert a "pause" to allow this file to generate its files first
        if (!is_header) // if is_header, the "pause" has already been added
            ret.Add(wxString(COMPILER_WAIT));
        AppendArray(ret_generated, ret);
    }

    // if it's a PCH, delete the previously generated PCH to avoid problems
    // (it 'll be recreated anyway)
    if ( (ft == ftHeader) && pf->compile )
    {
        wxString object_abs = (compiler->GetSwitches().UseFlatObjects)
                            ? pfd.object_file_flat_absolute_native
                            : pfd.object_file_absolute_native;

        if ( !wxRemoveFile(object_abs) )
            Manager::Get()->GetLogManager()->DebugLog(_("Cannot remove old PCH file:\n") + object_abs);
    }

    return ret;
}

/// This is to be used *only* for files not belonging to a project!!!
wxArrayString DirectCommands::GetCompileSingleFileCommand(const wxString& filename)
{
    wxArrayString ret;

    // lookup file's type
    FileType ft = FileTypeOf(filename);

    // is it compilable?
    if (ft != ftSource)
        return ret;

    wxFileName fname(filename);
    fname.SetExt(m_pCompiler->GetSwitches().objectExtension);
    wxString o_filename = fname.GetFullPath();
    wxString srcExt = fname.GetExt();
    fname.SetExt(FileFilters::EXECUTABLE_EXT);
    wxString exe_filename = fname.GetFullPath();

    wxString s_filename = filename;
    QuoteStringIfNeeded(s_filename);
    QuoteStringIfNeeded(o_filename);

    Compiler* compiler = CompilerFactory::GetDefaultCompiler();
    if (!compiler)
        return ret;

    // please leave this check here for convenience: single file compilation is "special"
    if (!m_pGenerator) cbThrow(_T("Command generator not initialised through ctor!"));

    wxString compilerCmd = compiler->GetCommand(ctCompileObjectCmd, srcExt);
    m_pGenerator->GenerateCommandLine(compilerCmd,
                                      0,
                                      0,
                                      s_filename,
                                      o_filename,
                                      o_filename,
                                      wxEmptyString);
    wxString linkerCmd = compiler->GetCommand(ctLinkConsoleExeCmd, fname.GetExt());
    m_pGenerator->GenerateCommandLine(linkerCmd,
                                      0,
                                      0,
                                      wxEmptyString,
                                      o_filename,
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
    else
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Skipping file (no compiler program set): ") + filename);

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
        AddCommandsToArray(linkerCmd, ret, true);
    }
    else
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Skipping linking (no linker program set): ") + exe_filename);
    return ret;
}

/// This is to be used *only* for files not belonging to a project!!!
wxArrayString DirectCommands::GetCleanSingleFileCommand(const wxString& filename)
{
    wxArrayString ret;

    // lookup file's type
    FileType ft = FileTypeOf(filename);

    // is it compilable?
    if (ft != ftSource)
        return ret;

    wxFileName fname(filename);
    fname.SetExt(m_pCompiler->GetSwitches().objectExtension);
    wxString o_filename = fname.GetFullPath();
    fname.SetExt(FileFilters::EXECUTABLE_EXT);
    wxString exe_filename = fname.GetFullPath();

    ret.Add(o_filename);
    ret.Add(exe_filename);

    return ret;
}

wxArrayString DirectCommands::GetCompileCommands(ProjectBuildTarget* target, bool force)
{
    wxArrayString ret;

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
    return ret;
}

wxArrayString DirectCommands::GetTargetCompileCommands(ProjectBuildTarget* target, bool force)
{
    wxArrayString ret;
    m_pCurrTarget = target;

    // set list of #include directories
    DepsSearchStart(target);

    // iterate all files of the project/target and add them to the build process
    size_t counter = ret.GetCount();
    MyFilesArray files = GetProjectFilesSortedByWeight(target, true, false);
    size_t fcount = files.GetCount();
    bool hasWeight = false;
    unsigned short int lastWeight = 0;
    for (unsigned int i = 0; i < fcount; ++i)
    {
        ProjectFile* pf = files[i];
        // auto-generated files are handled automatically in GetCompileFileCommand()
        if (pf->AutoGeneratedBy())
            continue;

        const pfDetails& pfd = pf->GetFileDetails(target);
        wxString err;
        if (force || IsObjectOutdated(target, pfd, &err))
        {
            // Add a wait command if the weight of the current file is different from the previous one
            // Because GetCompileFileCommand() already adds a wait command if it compiled a PCH we
            // check the last command to prevent two consecutive wait commands
            if (hasWeight && lastWeight != pf->weight && (ret.IsEmpty() || ret.Last() != COMPILER_WAIT))
                ret.Add(wxString(COMPILER_WAIT));

            // compile file
            wxArrayString filecmd = GetCompileFileCommand(target, pf);
            AppendArray(filecmd, ret);

            // Update the weight
            if (!hasWeight)
                hasWeight = true;
            lastWeight = pf->weight;
        }
        else
        {
            if (!err.IsEmpty())
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + err);
        }
        if (m_doYield)
            Manager::Yield();
    }

    // add link command
    wxArrayString link = GetLinkCommands(target, ret.GetCount() != counter);
    AppendArray(link, ret);

    return ret;
}

wxArrayString DirectCommands::GetPreBuildCommands(ProjectBuildTarget* target)
{
    Compiler* compiler = target ? CompilerFactory::GetCompiler(target->GetCompilerID()) : m_pCompiler;
    wxArrayString buildcmds = target ? target->GetCommandsBeforeBuild() : m_pProject->GetCommandsBeforeBuild();
    if (!buildcmds.IsEmpty())
    {
        wxString title = target ? target->GetTitle() : m_pProject->GetTitle();
        wxArrayString tmp;
        for (size_t i = 0; i < buildcmds.GetCount(); ++i)
        {
            if (compiler)
            {
                if (target)
                    m_pGenerator->GenerateCommandLine(buildcmds[i], target, 0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
                else
                    m_pGenerator->GenerateCommandLine(buildcmds[i], m_pProject->GetCurrentlyCompilingTarget(), 0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
            }

            tmp.Add(wxString(COMPILER_WAIT)); // all commands should wait for queue to empty first
            tmp.Add(wxString(COMPILER_SIMPLE_LOG) + buildcmds[i]);
            tmp.Add(buildcmds[i]);
        }
        buildcmds = tmp;
        if (target)
            buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running target pre-build steps"), 0);
        else
            buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running project pre-build steps"), 0);
        if (m_doYield)
            Manager::Yield();
    }
    return buildcmds;
}

wxArrayString DirectCommands::GetPostBuildCommands(ProjectBuildTarget* target)
{
    Compiler* compiler = target ? CompilerFactory::GetCompiler(target->GetCompilerID()) : m_pCompiler;
    wxArrayString buildcmds = target ? target->GetCommandsAfterBuild() : m_pProject->GetCommandsAfterBuild();
    if (!buildcmds.IsEmpty())
    {
        wxString title = target ? target->GetTitle() : m_pProject->GetTitle();
        wxArrayString tmp;
        for (size_t i = 0; i < buildcmds.GetCount(); ++i)
        {
            if (compiler)
            {
                if (target)
                {
                    m_pGenerator->GenerateCommandLine(buildcmds[i], target, 0, wxEmptyString,
                                                      wxEmptyString, wxEmptyString, wxEmptyString);
                }
                else
                {
                    m_pGenerator->GenerateCommandLine(buildcmds[i], m_pProject->GetCurrentlyCompilingTarget(),
                                                      0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
                }
            }

            tmp.Add(wxString(COMPILER_WAIT)); // all commands should wait for queue to empty first
            tmp.Add(wxString(COMPILER_SIMPLE_LOG) + buildcmds[i]);
            tmp.Add(buildcmds[i]);
        }
        buildcmds = tmp;
        if (target)
            buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running target post-build steps"), 0);
        else
            buildcmds.Insert(wxString(COMPILER_SIMPLE_LOG) + _("Running project post-build steps"), 0);
        if (m_doYield)
            Manager::Yield();
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
    wxArrayString ret;

    wxString output = target->GetOutputFilename();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(output, target);

    wxFileName out = UnixFilename(output);
    wxString linkfiles;
    wxString FlatLinkFiles;
    wxString resfiles;
    bool IsOpenWatcom = target->GetCompilerID().IsSameAs(_T("ow"));

    time_t outputtime;
    depsTimeStamp(output.mb_str(), &outputtime);
    if (!outputtime)
        force = true;
    wxArrayString fileMissing;
    if ( AreExternalDepsOutdated(target, out.GetFullPath(), &fileMissing) )
        force = true;

    if (!fileMissing.IsEmpty())
    {
        wxString warn;
        warn.Printf(_("WARNING: Target '%s': Unable to resolve %lu external dependency/ies:"),
                    target->GetFullTitle().wx_str(), static_cast<unsigned long>(fileMissing.Count()));
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + warn);
        for (size_t i=0; i<fileMissing.Count(); i++)
            ret.Add(wxString(COMPILER_SIMPLE_LOG) + fileMissing[i]);
    }

    Compiler* compiler = target ? CompilerFactory::GetCompiler(target->GetCompilerID()) : m_pCompiler;

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
        wxString compilerCmd = compiler ? compiler->GetCommand(ctLinkStaticCmd) : wxString(wxEmptyString);
        wxRegEx re(_T("\\$([-+]+)link_objects"));
        if (re.Matches(compilerCmd))
            prependHack = re.GetMatch(compilerCmd, 1);
    }

    // get all the linkable objects for the target
    MyFilesArray files = GetProjectFilesSortedByWeight(target, false, true);
    if (files.GetCount() == 0)
    {
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Linking stage skipped (build target has no object files to link)"));
        return ret;
    }
    if (IsOpenWatcom && target->GetTargetType() != ttStaticLib)
        linkfiles << _T("file ");

    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];

        // we have to test again for each file if it is to be compiled
        // and we can't check the file for existence because we 're still
        // generating the command lines that will create the files...
        wxString macro = _T("$compiler");
        m_pGenerator->GenerateCommandLine(macro, target, pf, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
        if (macro.IsEmpty())
            continue;

        const pfDetails& pfd = pf->GetFileDetails(target);
        wxString Object = (compiler->GetSwitches().UseFlatObjects)?pfd.object_file_flat:pfd.object_file;

        if (FileTypeOf(pf->relativeFilename) == ftResource)
        {
            // -----------------------------------------
            // Following lines have been modified for OpenWatcom
            if (IsOpenWatcom)
                resfiles << _T("option resource=") << Object << _T(" ");
            else
                resfiles << Object << _T(" ");
            // ------------------------------------------
        }
        else
        {
            // -----------------------------------------
            // Following lines have been modified for OpenWatcom
            if (IsOpenWatcom && target->GetTargetType() != ttStaticLib)
            {
                linkfiles << prependHack << Object << _T(","); // see QUICK HACK above (prependHack)
                FlatLinkFiles << prependHack << pfd.object_file_flat << _T(","); // see QUICK HACK above (prependHack)
            }
            else
            {
                linkfiles << prependHack << Object << _T(" "); // see QUICK HACK above (prependHack)
                FlatLinkFiles << prependHack << pfd.object_file_flat << _T(" "); // see QUICK HACK above (prependHack)
            }
            // -----------------------------------------
        }

        // timestamp check
        if (!force)
        {
            time_t objtime;
            depsTimeStamp(pfd.object_file_native.mb_str(), &objtime);
            if (!objtime)
                force = true;
            if (objtime > outputtime)
                force = true;
        }
    }
    if (IsOpenWatcom)
    {
        linkfiles.Trim();
        if (linkfiles.Right(1).IsSameAs(_T(",")))
            linkfiles = linkfiles.BeforeLast(_T(','));
    }

    if (!force)
        return ret;

    // create output dir
    out.MakeAbsolute(m_pProject->GetBasePath());
    wxString dstname = out.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(dstname, target);
    if (!dstname.IsEmpty() && !CreateDirRecursively(dstname, 0755))
    {
        cbMessageBox(_("Can't create output directory ") + dstname);
    }

    // add actual link command
    wxString kind_of_output;
    CommandType ct = ctCount; // get rid of compiler warning
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

        case ttNative:
            ct = ctLinkNativeCmd;
            kind_of_output = _("native");
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
    wxString compilerCmd = compiler->GetCommand(ct);
    m_pGenerator->GenerateCommandLine(compilerCmd,
                                      target,
                                      0,
                                      _T(""),
                                      linkfiles,
                                      FlatLinkFiles,
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

        // the 'true' will make sure all commands will be prepended by
        // COMPILER_WAIT signal
        AddCommandsToArray(compilerCmd, ret, true, true);
    }
    else
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Skipping linking (no linker program set): ") + output);

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
        Compiler* compiler = target ? CompilerFactory::GetCompiler(target->GetCompilerID()) : m_pCompiler;
        if (compiler)
        {
            wxString ObjectAbs = (compiler->GetSwitches().UseFlatObjects) ? pfd.object_file_flat_absolute_native
                                                                          : pfd.object_file_absolute_native;
            ret.Add(ObjectAbs);
            // if this is an auto-generated file, delete it
            if (pf->AutoGeneratedBy())
                ret.Add(pf->file.GetFullPath());

            if (distclean)
                ret.Add(pfd.dep_file_absolute_native);
        }
    }

    // add target output
    wxString outputfilename = target->GetOutputFilename();

    if (target->GetTargetType() != ttCommandsOnly)
    {
        Manager::Get()->GetMacrosManager()->ReplaceMacros(outputfilename, target);
        ret.Add(outputfilename);
    }

    if (target->GetTargetType() == ttDynamicLib)
    {
        // for dynamic libs, delete static lib
        outputfilename = target->GetStaticLibFilename();
        Manager::Get()->GetMacrosManager()->ReplaceMacros(outputfilename, target);
        ret.Add(outputfilename);
        // .def exports file is not deleted, because it may be user-supplied
//        ret.Add(target->GetDynamicLibDefFilename());
    }

    return ret;
}

/** external deps are manually set by the user
  * e.g. a static library linked to the project is an external dep (if set as such by the user)
  * so that a re-linking is forced if the static lib is updated
  */
bool DirectCommands::AreExternalDepsOutdated(ProjectBuildTarget* target,
                                             const wxString& buildOutput,
                                             wxArrayString*  filesMissing)
{
    Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());

    // if no output, probably a commands-only target; nothing to relink
    // but we have to check other dependencies
    time_t timeOutput = 0;
    if (!buildOutput.IsEmpty())
    {
        wxString output = buildOutput;
        Manager::Get()->GetMacrosManager()->ReplaceMacros(output);
        depsTimeStamp(output.mb_str(), &timeOutput);
        // if build output exists, check for updated static libraries
        if (timeOutput)
        {
            // look for static libraries in target/project library dirs
            wxArrayString libs = target->GetLinkLibs();
            const wxArrayString& prjLibs = target->GetParentProject()->GetLinkLibs();
            const wxArrayString& cmpLibs = compiler->GetLinkLibs();
            AppendArray(prjLibs, libs);
            AppendArray(cmpLibs, libs);

            const wxArrayString& prjLibDirs = target->GetParentProject()->GetLibDirs();
            const wxArrayString& cmpLibDirs = compiler->GetLibDirs();
            wxArrayString libDirs = target->GetLibDirs();
            AppendArray(prjLibDirs, libDirs);
            AppendArray(cmpLibDirs, libDirs);

            for (size_t i = 0; i < libs.GetCount(); ++i)
            {
                wxString lib = libs[i];

                // if user manually pointed to a library, without using the lib dirs,
                // then just check the file directly w/out involving the search dirs...
                if (lib.Contains(_T("/")) || lib.Contains(_T("\\")))
                {
                    Manager::Get()->GetMacrosManager()->ReplaceMacros(lib, target);
                    lib = UnixFilename(lib);
                    time_t timeExtDep;
                    depsTimeStamp(lib.mb_str(), &timeExtDep);
                    if (timeExtDep > timeOutput)
                    {
                        // force re-link
                        Manager::Get()->GetLogManager()->DebugLog(F(_T("Forcing re-link of '%s/%s' because '%s' is newer"),
                                                                        target->GetParentProject()->GetTitle().wx_str(),
                                                                        target->GetTitle().wx_str(),
                                                                        lib.wx_str()));
                        return true;
                    }
                    continue;
                }

                if (!lib.StartsWith(compiler->GetSwitches().libPrefix))
                    lib = compiler->GetSwitches().libPrefix + lib;
                if (!lib.EndsWith(_T(".") + compiler->GetSwitches().libExtension))
                    lib += _T(".") + compiler->GetSwitches().libExtension;

                for (size_t l = 0; l < libDirs.GetCount(); ++l)
                {
                    wxString dir = libDirs[l] + wxFILE_SEP_PATH + lib;
                    Manager::Get()->GetMacrosManager()->ReplaceMacros(dir, target);
                    dir = UnixFilename(dir);
                    time_t timeExtDep;
                    depsTimeStamp(dir.mb_str(), &timeExtDep);
                    if (timeExtDep > timeOutput)
                    {
                        // force re-link
                        Manager::Get()->GetLogManager()->DebugLog(F(_T("Forcing re-link of '%s/%s' because '%s' is newer"),
                                                                        target->GetParentProject()->GetTitle().wx_str(),
                                                                        target->GetTitle().wx_str(),
                                                                        dir.wx_str()));
                        return true;
                    }
                }
            }
        }
    }

    // array is separated by ;
    wxArrayString extDeps  = GetArrayFromString(target->GetExternalDeps(), _T(";"));
    wxArrayString addFiles = GetArrayFromString(target->GetAdditionalOutputFiles(), _T(";"));
    for (size_t i = 0; i < extDeps.GetCount(); ++i)
    {
        if (extDeps[i].IsEmpty())
            continue;

        Manager::Get()->GetMacrosManager()->ReplaceMacros(extDeps[i]);
        time_t timeExtDep;
        depsTimeStamp(extDeps[i].mb_str(), &timeExtDep);
        // if external dep doesn't exist, no need to relink
        // but we have to check other dependencies
        if (!timeExtDep)
        {
            if (filesMissing) filesMissing->Add(extDeps[i]);
            continue;
        }

        // let's check the additional output files
        for (size_t j = 0; j < addFiles.GetCount(); ++j)
        {
            if (addFiles[j].IsEmpty())
                continue;

            Manager::Get()->GetMacrosManager()->ReplaceMacros(addFiles[j]);
            time_t timeAddFile;
            depsTimeStamp(addFiles[j].mb_str(), &timeAddFile);
            // if additional file doesn't exist, we can skip it
            if (!timeAddFile)
            {
                if (filesMissing) filesMissing->Add(addFiles[j]);
                continue;
            }

            // if external dep is newer than additional file, relink
            if (timeExtDep > timeAddFile)
                return true;
        }

        // if no output, probably a commands-only target; nothing to relink
        // but we have to check other dependencies
        if (buildOutput.IsEmpty())
            continue;

        // now check the target's output
        // this is moved last because, for "commands only" targets,
        // it would return before we had a chance to check the
        // additional output files (above)

        // if build output doesn't exist, relink
        if (!timeOutput)
            return true;

        // if external dep is newer than build output, relink
        if (timeExtDep > timeOutput)
            return true;
    }
    return false; // no force relink
}

bool DirectCommands::IsObjectOutdated(ProjectBuildTarget* target, const pfDetails& pfd, wxString* errorStr)
{
    // If the source file does not exist, then do not compile.
    time_t timeSrc;
    depsTimeStamp(pfd.source_file_absolute_native.mb_str(), &timeSrc);
    if (!timeSrc)
    {
        if (errorStr)
            *errorStr = _("WARNING: Can't read file's timestamp: ") + pfd.source_file_absolute_native;

        if (wxFileExists(pfd.source_file_absolute_native))
            return true;

        return false;
    }

    // If the object file does not exist, then it must be built. In this case
    // there is no need to scan the source file for headers.
    time_t timeObj;
    Compiler* compiler = target ? CompilerFactory::GetCompiler(target->GetCompilerID()) : m_pCompiler;
    if (!compiler)
        return false;

    wxString ObjectAbs = (compiler->GetSwitches().UseFlatObjects) ? pfd.object_file_flat_absolute_native
                                                                  : pfd.object_file_absolute_native;
    depsTimeStamp(ObjectAbs.mb_str(), &timeObj);
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

    // object file is up to date with source file
    return false;
}

void DirectCommands::DepsSearchStart(ProjectBuildTarget* target)
{
    depsSearchStart();

    MacrosManager* mm = Manager::Get()->GetMacrosManager();
    wxArrayString incs = m_pGenerator->GetCompilerSearchDirs(target);

    for (unsigned int i = 0; i < incs.GetCount(); ++i)
    {
        // replace custom vars in include dirs
        mm->ReplaceMacros(incs[i], target);
        // actually add search dirs for deps
        depsAddSearchDir(incs[i].mb_str());
    }

    // We could add the "global" compiler directories too, but we normally
    // don't care about the modification times of system include files.
}
