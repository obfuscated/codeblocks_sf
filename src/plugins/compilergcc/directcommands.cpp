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
#include "directcommands.h"
#include "compilergcc.h"
#include "makefilegenerator.h"

pfDetails::pfDetails(DirectCommands* cmds, ProjectBuildTarget* target, ProjectFile* pf)
{
    wxString sep = wxFileName::GetPathSeparator();
    wxFileName tmp;

    source_file_native = pf->relativeFilename;
    tmp = source_file_native;
    tmp.SetExt(cmds->m_pCompiler->GetSwitches().objectExtension);
    object_file_native = tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +
                          (target ? target->GetObjectOutput() : "") +
                          sep +
                          tmp.GetFullName();
    wxFileName o_file(object_file_native);
    o_file.MakeAbsolute(cmds->m_pProject->GetBasePath());
    object_dir_native = o_file.GetPath(wxPATH_GET_VOLUME);
    tmp.SetExt("d");
    dep_file_native = tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +
                      (target ? target->GetDepsOutput() : "") +
                      sep +
                      tmp.GetFullName();
    
    source_file = UnixFilename(source_file_native);
    cmds->QuoteStringIfNeeded(source_file);
    
    object_file = UnixFilename(object_file_native);
    cmds->QuoteStringIfNeeded(object_file);
    
    dep_file = UnixFilename(dep_file_native);
    cmds->QuoteStringIfNeeded(dep_file);
    
    object_dir = UnixFilename(object_dir_native);
    cmds->QuoteStringIfNeeded(object_dir);
}

DirectCommands::DirectCommands(CompilerGCC* compilerPlugin, Compiler* compiler, cbProject* project, int logPageIndex)
    : m_PageIndex(logPageIndex),
    m_pCompilerPlugin(compilerPlugin),
    m_pCompiler(compiler),
    m_pProject(project)
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
/* TODO (mandrav#1#): Check for deps timestamp too... */
    
    MakefileGenerator mg(m_pCompilerPlugin, m_pProject, "", 0); // don't worry! we just need a couple of utility funcs from it
    
    // lookup file's type
    FileType ft = FileTypeOf(pf->relativeFilename);
    
    if (ft != ftResource)
    {
        // normal source file
        
        // create output dir
        if (!pfd.object_dir_native.IsEmpty() && !wxDirExists(pfd.object_dir_native))
        {
            if (!wxMkdir(pfd.object_dir_native, 0755))
                wxMessageBox(_("Can't create object output directory ") + pfd.object_dir_native);
        }

        // add actual compile command
        wxString compilerCmd = mg.CreateSingleFileCompileCmd(ctCompileObjectCmd,
                                                             target,
                                                             pf,
                                                             pfd.source_file,
                                                             pfd.object_file,
                                                             pfd.dep_file);
        if (!compilerCmd.IsEmpty())
        {
            switch (m_pCompiler->GetSwitches().logging)
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
//            if (force)
//                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Forced compiling"));
            ret.Add(compilerCmd);
        }
    }
    else
    {
        // win32 resource file
//        Manager::Get()->GetMessageManager()->SwitchTo(m_PageIndex);
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, "win32 resource file: not implemented");
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
        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            wxArrayString targetcompile = GetTargetCompileCommands(bt, force);
            AppendArray(targetcompile, ret);
        }
    }
    return ret;
}

wxArrayString DirectCommands::GetTargetCompileCommands(ProjectBuildTarget* target, bool force)
{
    wxArrayString ret;

    // make sure all project files are saved
    if (!m_pProject->SaveAllFiles())
        wxMessageBox(_("Could not save all files. Build might be incomplete..."));

    // iterate all files of the project/target and add them to the build process
    for (int i = 0; i < m_pProject->GetFilesCount(); ++i)
    {
        ProjectFile* pf = m_pProject->GetFile(i);
        // if the file does not belong in this target (if we have a target), skip it
        if (target && pf->buildTargets.Index(target->GetTitle()) == wxNOT_FOUND)
            continue;
        
        if (pf->compile) // TODO: add check if need deps
        {
//            Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("Calculating dependencies: %s"), pf->relativeFilename.c_str());
//            m_DepFiles.Clear();
//            GetDependenciesOf(pf->relativeFilename);
//            if (!m_DepFiles.IsEmpty())
//                m_DepFiles.Remove(0, 1); // remove the first entry; it's always this file
//            /*for (size_t i = 0; i < m_DepFiles.GetCount(); ++i)
//                Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("    DBG: %s"), m_DepFiles[i].c_str());*/

            pfDetails pfd(this, target, pf);
            wxArrayString filecmd = GetCompileFileCommand(target, pf, force | DependsOnChangedFile(pfd));
            AppendArray(filecmd, ret);
        }
    }
    return ret;
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
            wxArrayString targetlink = GetTargetLinkCommands(bt, force);
            AppendArray(targetlink, ret);
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
    for (int i = 0; i < m_pProject->GetFilesCount(); ++i)
    {
        ProjectFile* pf = m_pProject->GetFile(i);

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
        switch (m_pCompiler->GetSwitches().logging)
        {
            case clogFull:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + compilerCmd);
                break;
            
            case clogSimple:
                ret.Add(wxString(COMPILER_SIMPLE_LOG) + _("Linking ") + kind_of_output + ": " + target->GetOutputFilename());
                break;
            
            default:
                break;
        }
        ret.Add(compilerCmd);
    }
    
    return ret;
}

wxArrayString DirectCommands::GetCleanCommands(ProjectBuildTarget* target)
{
    wxArrayString ret;
    
    if (target)
        ret = GetTargetCleanCommands(target);
    else
    {
        for (int x = 0; x < m_pProject->GetBuildTargetsCount(); ++x)
        {
            ProjectBuildTarget* bt = m_pProject->GetBuildTarget(x);
            wxArrayString targetclear = GetTargetCleanCommands(bt);
            AppendArray(targetclear, ret);
        }
    }
    return ret;
}

wxArrayString DirectCommands::GetTargetCleanCommands(ProjectBuildTarget* target)
{
    wxArrayString ret;

    // add object files
    for (int i = 0; i < m_pProject->GetFilesCount(); ++i)
    {
        ProjectFile* pf = m_pProject->GetFile(i);

        // if the file is not compilable, skip it
        if (!pf->compile)
            continue;

        // if the file does not belong in this target (if we have a target), skip it
        if (target && pf->buildTargets.Index(target->GetTitle()) == wxNOT_FOUND)
            continue;
        
        pfDetails pfd(this, target, pf);
        ret.Add(pfd.object_file_native);
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

/// Creates a list of files this project file depends on, by scanning for #include directives
/// This list of files is m_DepFiles
/// You have to clear m_DepFiles manually before calling this function!
bool DirectCommands::GetDependenciesOf(const wxString& filename)
{
    // check if we already scanned this file (to avoid infinite loop)
    wxFileName fname(filename);
    if (m_DepFiles.Index(fname.GetFullPath()) != wxNOT_FOUND)
        return true; // already scanned
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: %s not scanned yet", fname.GetFullPath().c_str());

    wxFileInputStream file(fname.GetFullPath());
    if (!file.Ok())
        return false; // error opening file???
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: %s exists", fname.GetFullPath().c_str());

    m_DepFiles.Add(fname.GetFullPath());
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
                if (isLocal)
                    GetDependenciesOf(rest);
                else
                {
                    // try scanning the file by prepending all the globals and project include dirs until it's found
                    wxString newfilename;
                    wxString sep = wxFileName::GetPathSeparator();
                    
                    // project include dirs first
                    const wxArrayString& prj_incs = m_pProject->GetIncludeDirs();
                    for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
                    {
                        newfilename = prj_incs[i] + sep + rest;
                        if (wxFileExists(newfilename))
                        {
                            if (GetDependenciesOf(newfilename))
                                break;
                        }
                    }

                    // global include dirs second
//                    const wxArrayString& global_incs = m_pCompiler->GetIncludeDirs();
//                    for (unsigned int i = 0; i < global_incs.GetCount(); ++i)
//                    {
//                        newfilename = global_incs[i] + sep + rest;
//                        if (wxFileExists(newfilename))
//                        {
//                            if (GetDependenciesOf(newfilename))
//                                break;
//                        }
//                    }
                }
            }
        }
    }
    return true;
}

/// Returns true if any one of the files listed in m_DepFiles, has later modification
/// A call to GetDependenciesOf() must have preceded to fill m_DepFiles...
bool DirectCommands::DependsOnChangedFile(const pfDetails& pfd)
{
    wxFileName basefile(pfd.source_file_native);
    wxFileName baseobjfile(pfd.object_file_native);
    wxDateTime basetime = baseobjfile.GetModificationTime();
    for (unsigned int i = 0; i < m_DepFiles.GetCount(); ++i)
    {
        wxFileName otherfile(m_DepFiles[i]);
        if (otherfile.SameAs(basefile))
            continue;
        wxDateTime othertime = wxFileName(m_DepFiles[i]).GetModificationTime();
        if (basetime.IsValid() && othertime.IsValid() && othertime.IsLaterThan(basetime))
        {
            Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: file %s depends on modified %s", pfd.source_file_native.c_str(), m_DepFiles[i].c_str());
            return true; // one match is enough ;)
        }
    }
    return false;
}
