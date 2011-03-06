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
                               Compiler* compiler,
                               cbProject* project,
                               int logPageIndex)
    : m_doYield(false),
    m_PageIndex(logPageIndex),
    m_pCompilerPlugin(compilerPlugin),
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
    Manager::Get()->GetLogManager()->DebugLog(
        F(_("Scanned %d files for #includes, cache used %d, cache updated %d"),
        stats.scanned, stats.cache_used, stats.cache_updated));

    depsDone();
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
    wxArrayString retGenerated;

    // is it compilable?
    if (!pf->compile || pf->compilerVar.IsEmpty())
        return ret;

    const pfDetails& pfd = pf->GetFileDetails(target);
    Compiler* compiler = target ? CompilerFactory::GetCompiler(target->GetCompilerID()) : m_pCompiler;
    if (!compiler)
        return ret;
    wxString Object = (compiler->GetSwitches().UseFlatObjects)?pfd.object_file_flat:pfd.object_file;
    wxString ObjectDir = (compiler->GetSwitches().UseFlatObjects)?pfd.object_dir_flat_native:pfd.object_dir_native;

    // lookup file's type
    FileType ft = FileTypeOf(pf->relativeFilename);

    // create output dir
    if (!ObjectDir.IsEmpty() && !CreateDirRecursively(ObjectDir, 0755))
        cbMessageBox(_("Can't create object output directory ") + ObjectDir);

    bool isResource = ft == ftResource;
    bool isHeader = ft == ftHeader;

    // allowed resources under all platforms: makes sense when cross-compiling for
    // windows under linux.
    // and anyway, if the user is dumb enough to try to compile resources without
    // having a resource compiler, (s)he deserves the upcoming build error ;)

    wxString compilerCmd;
    if (!isHeader || compiler->GetSwitches().supportsPCH)
    {
        const CompilerTool& tool = compiler->GetCompilerTool(isResource ? ctCompileResourceCmd : ctCompileObjectCmd, pf->file.GetExt());

        // does it generate other files to compile?
        for (size_t i = 0; i < pf->generatedFiles.size(); ++i)
        {
            AppendArray(GetCompileFileCommand(target, pf->generatedFiles[i]), retGenerated); // recurse
        }

        pfCustomBuild& pcfb = pf->customBuild[compiler->GetID()];
        compilerCmd = pcfb.useCustomBuildCommand
                        ? pcfb.buildCommand
                        : tool.command;
        wxString source_file;
        if (compiler->GetSwitches().UseFullSourcePaths)
        {
            source_file = UnixFilename(pfd.source_file_absolute_native);
            // for resource files, use short from if path because if windres bug with spaces-in-paths
            if (isResource)
                source_file = pf->file.GetShortPath();
        }
        else
            source_file = pfd.source_file;
        QuoteStringIfNeeded(source_file);
        compiler->GenerateCommandLine(compilerCmd,
                                         target,
                                         pf,
                                         source_file,
                                         Object,
                                         pfd.object_file_flat,
                                         pfd.dep_file);
    }

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
        if (isHeader)
            ret.Add(wxString(COMPILER_WAIT));
        if (retGenerated.GetCount())
        {
            // not only append commands for (any) generated files to be compiled
            // but also insert a "pause" to allow this file to generate its files first
            if (!isHeader) // if isHeader, the "pause" has already been added
                ret.Add(wxString(COMPILER_WAIT));
            AppendArray(retGenerated, ret);
        }

        // if it's a PCH, delete the previously generated PCH to avoid problems
        // (it 'll be recreated anyway)
        if (FileTypeOf(pf->relativeFilename) == ftHeader && pf->compile)
        {
            wxString ObjectAbs = (compiler->GetSwitches().UseFlatObjects)?pfd.object_file_flat_absolute_native:pfd.object_file_absolute_native;
            wxRemoveFile(ObjectAbs);
        }
    }
    else
        ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Skipping file (no compiler program set): ") + pfd.source_file_native);
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

    wxString compilerCmd = compiler->GetCommand(ctCompileObjectCmd, srcExt);
    compiler->GenerateCommandLine(compilerCmd,
                                     0,
                                     0,
                                     s_filename,
                                     o_filename,
                                     o_filename,
                                     wxEmptyString);
    wxString linkerCmd = compiler->GetCommand(ctLinkConsoleExeCmd, fname.GetExt());
    compiler->GenerateCommandLine(linkerCmd,
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
//    Manager::Get()->GetLogManager()->DebugLog(wxString("-----GetTargetCompileCommands-----"));
    wxArrayString ret;
    m_pCurrTarget = target;

    // set list of #include directories
    DepsSearchStart(target);

    // iterate all files of the project/target and add them to the build process
    size_t counter = ret.GetCount();
    MyFilesArray files = GetProjectFilesSortedByWeight(target, true, false);
    size_t fcount = files.GetCount();
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
            // compile file
            wxArrayString filecmd = GetCompileFileCommand(target, pf);
            AppendArray(filecmd, ret);
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
                    compiler->GenerateCommandLine(buildcmds[i], target, 0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
                else
                    compiler->GenerateCommandLine(buildcmds[i], m_pProject->GetCurrentlyCompilingTarget(), 0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
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
                    compiler->GenerateCommandLine(buildcmds[i], target, 0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
                else
                    compiler->GenerateCommandLine(buildcmds[i], m_pProject->GetCurrentlyCompilingTarget(), 0, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
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
    if ( AreExternalDepsOutdated(out.GetFullPath(), target->GetAdditionalOutputFiles(),
                                 target->GetExternalDeps(), &fileMissing) )
    {
        force = true;
    }
    if (!fileMissing.IsEmpty())
    {
        wxString warn;
        warn.Printf(_("WARNING: Target '%s': Unable to resolve %d external dependencies:"),
                    target->GetFullTitle().wx_str(), fileMissing.Count());
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
        compiler->GenerateCommandLine(macro, target, pf, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString);
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
    compiler->GenerateCommandLine(compilerCmd,
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
bool DirectCommands::AreExternalDepsOutdated(const wxString& buildOutput,
                                             const wxString& additionalFiles,
                                             const wxString& externalDeps,
                                             wxArrayString*  filesMissing)
{
    // array is separated by ;
    wxArrayString extDeps  = GetArrayFromString(externalDeps, _T(";"));
    wxArrayString addFiles = GetArrayFromString(additionalFiles, _T(";"));
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
        wxString output = buildOutput;
        Manager::Get()->GetMacrosManager()->ReplaceMacros(output);
        time_t timeOutput;
        depsTimeStamp(output.mb_str(), &timeOutput);
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
    wxArrayString incs = m_pCompiler->GetCompilerSearchDirs(target);

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
