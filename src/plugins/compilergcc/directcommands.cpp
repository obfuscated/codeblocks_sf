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

pfDetails::pfDetails(DirectCommands* cmds, ProjectBuildTarget* target, ProjectFile* pf)
{
    wxString sep = wxFileName::GetPathSeparator();
    wxFileName tmp;

    source_file_native = pf->relativeFilename;
    tmp = pf->GetObjName();
//    tmp = source_file_native;
//    bool isResource = FileTypeOf(source_file_native) == ftResource;
//    tmp.SetExt(isResource ? RESOURCEBIN_EXT : cmds->m_pCompiler->GetSwitches().objectExtension);
    object_file_native = tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +
                          (target ? target->GetObjectOutput() : "") +
                          sep +
                          tmp.GetFullName();
    wxFileName o_file(object_file_native);
    o_file.MakeAbsolute(cmds->m_pProject->GetBasePath());
    object_dir_native = o_file.GetPath(wxPATH_GET_VOLUME);
    tmp.SetExt("depend");
    dep_file_native = tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +
                      (target ? target->GetDepsOutput() : "") +
                      sep +
                      tmp.GetFullName();
    wxFileName d_file(dep_file_native);
    d_file.MakeAbsolute(cmds->m_pProject->GetBasePath());
    dep_dir_native = d_file.GetPath(wxPATH_GET_VOLUME);
    
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
}

DirectCommands::~DirectCommands()
{
	//dtor
}

void DirectCommands::QuoteStringIfNeeded(wxString& str)
{
    if (!str.IsEmpty() && str.GetChar(0) != '"')
        str = "\"" + str + "\"";
}

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
    Manager::Get()->GetMacrosManager()->ReplaceMacros(cmd);
    
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

MyFilesArray DirectCommands::GetProjectFilesSortedByWeight()
{
    MyFilesArray files;
    for (int i = 0; i < m_pProject->GetFilesCount(); ++i)
    {
        ProjectFile* pf = m_pProject->GetFile(i);
        files.Add(pf);
    }
    files.Sort(MySortProjectFilesByWeight);
    return files;
}

wxArrayString DirectCommands::GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf, bool force)
{
    wxLogNull ln;
    wxArrayString ret;

    // is it compilable?
    if (!pf->compile || pf->compilerVar.IsEmpty())
        return ret;

    pfDetails pfd(this, target, pf);

    // timestamp check
    if (!force)
    {
        wxDateTime srclast = wxFileName(pfd.source_file_native).GetModificationTime();
        wxDateTime dstlast = wxFileName(pfd.object_file_native).GetModificationTime();
        if (srclast.IsValid() && dstlast.IsValid() && !srclast.IsLaterThan(dstlast))
            return ret;
    }
    
    MakefileGenerator mg(m_pCompilerPlugin, m_pProject, "", 0); // don't worry! we just need a couple of utility funcs from it
    
    // lookup file's type
    FileType ft = FileTypeOf(pf->relativeFilename);

    // create output dir
    if (!pfd.object_dir_native.IsEmpty() && !wxDirExists(pfd.object_dir_native))
    {
        if (!wxMkdir(pfd.object_dir_native, 0755))
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
        msg.Printf(_("File %s has custom custom build command set."
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

        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            if (bt->GetIncludeInTargetAll()) // only if target gets build with "all"
            {
                wxArrayString targetcompile = GetTargetCompileCommands(bt, force);
                AppendArray(targetcompile, ret);
            }
        }

        // add post-build commands
        if (!ret.IsEmpty())
            AppendArray(GetPostBuildCommands(0L), ret);
    }
    return ret;
}

wxArrayString DirectCommands::GetTargetCompileCommands(ProjectBuildTarget* target, bool force)
{
    wxArrayString ret;
    ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Switching to target: ") + target->GetTitle());

    m_pCurrTarget = target;

    // make sure all project files are saved
    if (!m_pProject->SaveAllFiles())
        wxMessageBox(_("Could not save all files. Build might be incomplete..."));

    // add pre-build commands
    AppendArray(GetPreBuildCommands(target), ret);

    if (target->GetTargetType() == 4)
    {
        // commands-only target
        AppendArray(GetPostBuildCommands(target), ret);
        return ret;
    }

    // iterate all files of the project/target and add them to the build process
    MyFilesArray files = GetProjectFilesSortedByWeight();
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];
        // if the file does not belong in this target (if we have a target), skip it
        if (target && pf->buildTargets.Index(target->GetTitle()) == wxNOT_FOUND)
            continue;
        
        if (pf->compile)
        {
            // check for deps
            bool forceByDeps = false;
            if (m_pCompiler->GetSwitches().needDependencies)
            {
                pfDetails pfd(this, target, pf);
                if (pf->autoDeps)
                    forceByDeps = ForceCompileByDependencies(pfd);
                else
                {
                    wxString msg;
                    msg.Printf(_("File %s has custom dependencies set."
                                "This feature only works when using GNU \"make\""
                                "for the build process..."), pfd.source_file_native.c_str());
                    ret.Add(wxString(COMPILER_SIMPLE_LOG) + msg);
                }
            }

            // compile file
            wxArrayString filecmd = GetCompileFileCommand(target, pf, force | forceByDeps);
            AppendArray(filecmd, ret);
        }
    }

    if (ret.GetCount() == 1)
        ret.Clear(); // it's just the "Switching to..." message

    // add link command
    wxArrayString link = GetLinkCommands(target, ret.GetCount() > 1);
    AppendArray(link, ret);

    // add post-build commands
    if (!ret.IsEmpty())
        AppendArray(GetPostBuildCommands(target), ret);

    return ret;
}

wxArrayString DirectCommands::GetPreBuildCommands(ProjectBuildTarget* target)
{
    wxArrayString buildcmds = target ? target->GetCommandsBeforeBuild() : m_pProject->GetCommandsBeforeBuild();
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

    wxString output = target->GetOutputFilename();
    wxString linkfiles;

    wxDateTime latestobjecttime;

    // get all the linkable objects for the target
    MyFilesArray files = GetProjectFilesSortedByWeight();
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];

        // if the file is not linkable, skip it
        if (!pf->link)
            continue;

        // if the file does not belong in this target (if we have a target), skip it
        if (target && pf->buildTargets.Index(target->GetTitle()) == wxNOT_FOUND)
            continue;

        pfDetails pfd(this, target, pf);
        linkfiles << pfd.object_file << " ";

        // timestamp check
        if (!force)
        {
            wxDateTime objtime = wxFileName(pfd.object_file_native).GetModificationTime();
            if (!latestobjecttime.IsValid() ||
                (objtime.IsValid() && objtime.IsLaterThan(latestobjecttime)))
            {
                latestobjecttime = objtime;
            }
        }
    }

    if (!force)
    {
        wxDateTime outputtime = wxFileName(target->GetOutputFilename()).GetModificationTime();
        if (outputtime.IsValid() && outputtime.IsLaterThan(latestobjecttime))
            return ret; // no object file more recent than output file
    }

    // create output dir
    wxFileName out = UnixFilename(target->GetOutputFilename());
    wxString dstname = out.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!dstname.IsEmpty() && !wxDirExists(dstname))
    {
        if (!wxMkdir(dstname, 0755))
            wxMessageBox(_("Can't create output directory ") + dstname);
    }

    // add actual link command
    wxString kind_of_output;
    CommandType ct;
    switch (target->GetTargetType())
    {
        case ttConsoleOnly:
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
    }
    wxString compilerCmd = mg.CreateSingleFileCompileCmd(ct, target, 0, "", linkfiles, "");
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
    MyFilesArray files = GetProjectFilesSortedByWeight();
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        ProjectFile* pf = files[i];

        // if the file is not compilable, skip it
        if (!pf->compile)
            continue;

        // if the file does not belong in this target (if we have a target), skip it
        if (target && pf->buildTargets.Index(target->GetTitle()) == wxNOT_FOUND)
            continue;
        
        pfDetails pfd(this, target, pf);
        ret.Add(pfd.object_file_native);
        if (distclean)
            ret.Add(pfd.dep_file_native);
    }

    // add target output
    ret.Add(target->GetOutputFilename());
    if (target->GetTargetType() == ttDynamicLib)
    {
        // for dynamic libs, delete static lib and def too
        ret.Add(target->GetStaticLibFilename());
        ret.Add(target->GetDynamicLibDefFilename());
    }

    return ret;
}

bool DirectCommands::ForceCompileByDependencies(const pfDetails& pfd)
{
    wxArrayString deps;
    bool done = false;

    if (wxFileExists(pfd.dep_file_native))
    {
        // check source file time with deps file time
        // if newer, force a scan of dependencies again
        wxDateTime src = wxFileName(pfd.source_file_native).GetModificationTime();
        wxDateTime dep = wxFileName(pfd.dep_file_native).GetModificationTime();
        if (src.IsValid() && dep.IsValid() && !src.IsLaterThan(dep))
        {
            // just read existing deps file
            done = ReadDependencies(pfd.dep_file_native, deps);
        }
    }
    
    if (!done)
    {
        // scan for dependencies
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Calculating dependencies: %s"), pfd.source_file_native.c_str());
        GetDependenciesOf(pfd.source_file_native, deps);
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("    DBG: %s has %d deps"), pfd.source_file_native.c_str(), deps.GetCount());
        if (!deps.IsEmpty())
            deps.Remove(0, 1); // remove the first entry; it's always this file

        // create deps dir
        if (!pfd.dep_dir_native.IsEmpty() && !wxDirExists(pfd.dep_dir_native))
        {
            if (!wxMkdir(pfd.dep_dir_native, 0755))
                wxMessageBox(_("Can't create dependencies output directory ") + pfd.dep_dir_native);
        }

        // save them to file
        wxFile file;
        file.Create(pfd.dep_file_native, true);
        if (file.IsOpened())
        {
            for (size_t i = 0; i < deps.GetCount(); ++i)
            {
                file.Write(deps[i] + "\n");
            }
        }
    }
    /*for (size_t i = 0; i < deps.GetCount(); ++i)
        Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("    DBG: %s"), deps[i].c_str());*/
    return DependsOnChangedFile(pfd, deps);
}

bool DirectCommands::ReadDependencies(const wxString& filename, wxArrayString& deps)
{
    wxFileInputStream file(filename);
    if (!file.Ok())
        return false; // error opening file???

    wxTextInputStream input(file);
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(false);
        line.Trim(true);
        if (line.GetChar(line.Length() - 1) == '\\')
        {
            // GNU "make" style dependencies file detected
            deps.Clear();
            return false;
        }
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Line: '%s'", line.c_str());
        if (!line.IsEmpty())
            deps.Add(line);
    }
    return true;
}

/// Creates a list of files this project file depends on, by scanning for #include directives
/// This list of files is deps
bool DirectCommands::GetDependenciesOf(const wxString& filename, wxArrayString& deps)
{
    wxLogNull ln;

    wxFileName fname(filename);
//    if (fname.IsRelative())
//        fname.MakeAbsolute(m_pProject->GetBasePath());

    // check if we already scanned this file (to avoid infinite loop)
    if (deps.Index(fname.GetFullPath()) != wxNOT_FOUND)
        return true; // already scanned
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: %s not scanned yet", fname.GetFullPath().c_str());

    wxFileInputStream file(fname.GetFullPath());
    if (!file.Ok())
        return false; // error opening file???
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: %s exists", fname.GetFullPath().c_str());

    deps.Add(fname.GetFullPath());

    wxTextInputStream input(file);
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(false);
        
        // only lines containing preprocessor directives
        if (line.GetChar(0) != '#')
            continue;
        
        // remove #, so that we can trim the rest up to the directive
        line.Remove(0, 1);
        line.Trim(false);

        // check if it is an #include directive
        wxString rest;
        if (line.StartsWith("include", &rest))
        {
            rest.Trim(false);
            if (rest.GetChar(0) != '"' && rest.GetChar(0) != '<')
                continue; // invalid token?

            bool isLocal = rest.GetChar(0) == '"';

            // now "rest" must hold either "some/file.name" or <some/file.name>
            rest.Remove(0, 1);
            
            size_t idx = 0;
            while (true)
            {
                if (idx >= rest.Length())
                {
                    // we reached the end of line and didn't find the string :(
                    rest.Clear();
                    break;
                }
                if (rest.GetChar(idx) == '"' || rest.GetChar(idx) == '>')
                {
                    rest.Remove(idx);
                    break;
                }
                ++idx;
            }

            // if rest is not empty, we got an included filename :)
            if (!rest.IsEmpty())
            {
//                Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: found included file: %s", rest.c_str());
                // if #include uses quotes (is local relative filename), scan it directly
//                if (!GetDependenciesOf(rest, deps))
                if (isLocal)
                {
//                    wxFileName tmp(rest);
//                    if (tmp.IsRelative())
//                        tmp.MakeAbsolute(fname.GetPath());
//                    GetDependenciesOf(tmp.GetFullPath(), deps);
                    wxFileName tmp(fname.GetPath(wxPATH_GET_SEPARATOR) + rest);
                    tmp.Normalize(wxPATH_NORM_ALL, m_pProject->GetBasePath());
                    tmp.MakeRelativeTo(m_pProject->GetBasePath());
                    
                    GetDependenciesOf(tmp.GetFullPath(), deps);
                }
                else
                {
                    // try scanning the file by prepending all the globals and project include dirs until it's found
                    wxString newfilename;
                    wxString sep = wxFileName::GetPathSeparator();
                    
                    // target include dirs first
                    if (m_pCurrTarget)
                    {
                        const wxArrayString& tgt_incs = m_pCurrTarget->GetIncludeDirs();
                        for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
                        {
                            newfilename = tgt_incs[i] + sep + rest;
                            if (wxFileExists(newfilename))
                            {
                                if (GetDependenciesOf(newfilename, deps))
                                    break;
                            }
                        }
                    }

                    // project include dirs second
                    const wxArrayString& prj_incs = m_pProject->GetIncludeDirs();
                    for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                    {
                        newfilename = prj_incs[i] + sep + rest;
                        if (wxFileExists(newfilename))
                        {
                            if (GetDependenciesOf(newfilename, deps))
                                break;
                        }
                    }

                    // global include dirs last
//                    const wxArrayString& global_incs = m_pCompiler->GetIncludeDirs();
//                    for (unsigned int i = 0; i < global_incs.GetCount(); ++i)
//                    {
//                        newfilename = global_incs[i] + sep + rest;
//                        if (wxFileExists(newfilename))
//                        {
//                            if (GetDependenciesOf(newfilename, deps))
//                                break;
//                        }
//                    }
                }
            }
        }
    }
    return true;
}

/// Returns true if any one of the files listed in deps, has later modification
/// A call to GetDependenciesOf() must have preceded to fill deps...
bool DirectCommands::DependsOnChangedFile(const pfDetails& pfd, const wxArrayString& deps)
{
    wxFileName basefile(pfd.source_file_native);
    wxFileName baseobjfile(pfd.object_file_native);
    wxDateTime basetime = baseobjfile.GetModificationTime();
    for (unsigned int i = 0; i < deps.GetCount(); ++i)
    {
        wxFileName otherfile(deps[i]);
        if (otherfile.SameAs(basefile))
            continue;
        wxDateTime othertime = wxFileName(deps[i]).GetModificationTime();
        if (basetime.IsValid() && othertime.IsValid() && othertime.IsLaterThan(basetime))
        {
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: file %s depends on modified %s", pfd.source_file_native.c_str(), deps[i].c_str());
            return true; // one match is enough ;)
        }
    }
    return false;
}
