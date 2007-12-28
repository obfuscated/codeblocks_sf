#include <sdk_precomp.h>
#include <manager.h>
#include <macrosmanager.h>
#include <configmanager.h>
#include <annoyingdialog.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <wx/string.h>
    #include <wx/filedlg.h>
#endif

#include <wx/filename.h>
#include <wx/utils.h>

#include "scriptsecuritywarningdlg.h"
#include "sc_base_types.h"

namespace ScriptBindings
{
    namespace IOLib
    {
        // not exposed
        bool SecurityAllows(const wxString& operation, const wxString& descr)
        {
            if (Manager::Get()->GetScriptingManager()->IsCurrentlyRunningScriptTrusted())
                return true;

            if (Manager::Get()->GetConfigManager(_T("security"))->ReadBool(operation, false))
                return true;

            ScriptSecurityWarningDlg dlg(Manager::Get()->GetAppWindow(), operation, descr);
            if (dlg.ShowModal() != wxID_OK)
                return false;

            ScriptSecurityResponse response = dlg.GetResponse();
            switch (response)
            {
                case ssrAllow:
                    return true;

                case ssrAllowAll:
                    Manager::Get()->GetConfigManager(_T("security"))->Write(operation, true);
                    return true;

                case ssrTrust: // purposely fall through
                case ssrTrustPermanently:
                    Manager::Get()->GetScriptingManager()->TrustCurrentlyRunningScript(response == ssrTrustPermanently);
                    return true;

                default:
                    return false;
            }
            return false;
        }

        wxString GetCwd()
        {
            return wxGetCwd();
        }

        void SetCwd(const wxString& dir)
        {
            wxSetWorkingDirectory(dir);
        }

        bool CreateDirRecursively(const wxString& full_path, int perms)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(full_path));
            NormalizePath(fname, wxEmptyString);
            if (!SecurityAllows(_T("CreateDir"), fname.GetFullPath()))
                return false;
            return ::CreateDirRecursively(fname.GetFullPath(), perms);
        }

        wxString ChooseDir(const wxString& message, const wxString& initialPath, bool showCreateDirButton)
        {
            return ChooseDirectory(0, message, Manager::Get()->GetMacrosManager()->ReplaceMacros(initialPath), wxEmptyString, false, showCreateDirButton);
        }

        bool RemoveDir(const wxString& src)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(src));
            NormalizePath(fname, wxEmptyString);
            if (!SecurityAllows(_T("RemoveDir"), fname.GetFullPath()))
                return false;
            return wxRmdir(fname.GetFullPath());
        }

        bool DirectoryExists(const wxString& dir)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(dir));
            NormalizePath(fname, wxEmptyString);
            return wxDirExists(fname.GetFullPath());
        }

        bool CopyFile(const wxString& src, const wxString& dst, bool overwrite)
        {
            wxFileName fname1(Manager::Get()->GetMacrosManager()->ReplaceMacros(src));
            wxFileName fname2(Manager::Get()->GetMacrosManager()->ReplaceMacros(dst));
            NormalizePath(fname1, wxEmptyString);
            NormalizePath(fname2, wxEmptyString);
            if (!SecurityAllows(_T("CopyFile"), wxString::Format(_T("%s -> %s"), src.c_str(), dst.c_str())))
                return false;
            if (!wxFileExists(fname1.GetFullPath())) return false;
            return wxCopyFile(fname1.GetFullPath(),
                            fname2.GetFullPath(),
                            overwrite);
        }

        bool RenameFile(const wxString& src, const wxString& dst)
        {
            wxFileName fname1(Manager::Get()->GetMacrosManager()->ReplaceMacros(src));
            wxFileName fname2(Manager::Get()->GetMacrosManager()->ReplaceMacros(dst));
            NormalizePath(fname1, wxEmptyString);
            NormalizePath(fname2, wxEmptyString);
            if (!SecurityAllows(_T("RenameFile"), wxString::Format(_T("%s -> %s"),
                                            fname1.GetFullPath().c_str(), fname2.GetFullPath().c_str())))
                return false;
            if (!wxFileExists(fname1.GetFullPath())) return false;
            return wxRenameFile(fname1.GetFullPath(),
                                fname2.GetFullPath());
        }

        bool RemoveFile(const wxString& src)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(src));
            NormalizePath(fname, wxEmptyString);
            if (!SecurityAllows(_T("RemoveFile"), fname.GetFullPath()))
                return false;
            if (!wxFileExists(fname.GetFullPath())) return false;
            return wxRemoveFile(fname.GetFullPath());
        }

        bool FileExists(const wxString& file)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(file));
            NormalizePath(fname, wxEmptyString);
            return wxFileExists(fname.GetFullPath());
        }

        wxString ChooseFile(const wxString& title, const wxString& defaultFile, const wxString& filter)
        {
            wxFileDialog dlg(0,
                            title,
                            wxEmptyString,
                            Manager::Get()->GetMacrosManager()->ReplaceMacros(defaultFile),
                            filter,
                            wxOPEN | compatibility::wxHideReadonly);
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
                return dlg.GetPath();
            return wxEmptyString;
        }

        wxString ReadFileContents(const wxString& filename)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(filename));
            NormalizePath(fname, wxEmptyString);
            wxFile f(fname.GetFullPath());
            return cbReadFileContents(f);
        }

        bool WriteFileContents(const wxString& filename, const wxString& contents)
        {
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(filename));
            NormalizePath(fname, wxEmptyString);
            if (!SecurityAllows(_T("CreateFile"), fname.GetFullPath()))
                return false;
            wxFile f(fname.GetFullPath(), wxFile::write);
            return cbWrite(f, contents);
        }

        int Execute(const wxString& command)
        {
            if (!SecurityAllows(_T("Execute"), command))
                return -1;
            wxArrayString output;
            return wxExecute(command, output, wxEXEC_NODISABLE);
        }

        wxString ExecuteAndGetOutput(const wxString& command)
        {
            if (!SecurityAllows(_T("Execute"), command))
                return wxEmptyString;
            wxArrayString output;
            wxExecute(command, output, wxEXEC_NODISABLE);
            return GetStringFromArray(output, _T("\n"));
        }

    } // namespace IOLib
} // namespace ScriptBindings

namespace ScriptBindings
{
    struct IONamespace {};

    void Register_IO()
    {
        SqPlus::SQClassDef<IONamespace>("IO").

                #ifndef NO_INSECURE_SCRIPTS
                staticFunc(&IOLib::CreateDirRecursively, "CreateDirectory").
                staticFunc(&IOLib::RemoveDir, "RemoveDirectory").
                staticFunc(&IOLib::CopyFile, "CopyFile").
                staticFunc(&IOLib::RenameFile, "RenameFile").
                staticFunc(&IOLib::RemoveFile, "RemoveFile").
                staticFunc(&IOLib::WriteFileContents, "WriteFileContents").
                staticFunc(&IOLib::Execute, "Execute").
                staticFunc(&IOLib::ExecuteAndGetOutput, "ExecuteAndGetOutput").
                #endif // NO_INSECURE_SCRIPTS

                staticFunc(&IOLib::GetCwd, "GetCwd").
                staticFunc(&IOLib::SetCwd, "SetCwd").

                staticFunc(&IOLib::DirectoryExists, "DirectoryExists").
                staticFunc(&IOLib::ChooseDir, "SelectDirectory").
                staticFunc(&IOLib::FileExists, "FileExists").
                staticFunc(&IOLib::ChooseFile, "SelectFile").
                staticFunc(&IOLib::ReadFileContents, "ReadFileContents");

        #ifndef NO_INSECURE_SCRIPTS
        SqPlus::BindConstant(true, "allowInsecureScripts");
        #else
        SqPlus::BindConstant(false, "allowInsecureScripts");
        #endif // NO_INSECURE_SCRIPTS
    }
} // namespace ScriptBindings
