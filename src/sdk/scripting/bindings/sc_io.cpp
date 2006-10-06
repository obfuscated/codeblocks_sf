#include <sdk_precomp.h>
#include <manager.h>
#include <macrosmanager.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <wx/string.h>
#endif

#include "sc_base_types.h"

namespace ScriptBindings
{
    namespace IOLib
    {
        bool CreateDirRecursively(const wxString& full_path, int perms)
        {
            return ::CreateDirRecursively(Manager::Get()->GetMacrosManager()->ReplaceMacros(full_path), perms);
        }

        wxString ChooseDir(const wxString& message, const wxString& initialPath, bool showCreateDirButton)
        {
            return ChooseDirectory(0, message, Manager::Get()->GetMacrosManager()->ReplaceMacros(initialPath), wxEmptyString, false, showCreateDirButton);
        }

        bool RemoveDir(const wxString& src)
        {
            return wxRmdir(Manager::Get()->GetMacrosManager()->ReplaceMacros(src));
        }

        bool DirectoryExists(const wxString& dir)
        {
            return wxDirExists(Manager::Get()->GetMacrosManager()->ReplaceMacros(dir));
        }

        bool CopyFile(const wxString& src, const wxString& dst, bool overwrite)
        {
            if (!wxFileExists(Manager::Get()->GetMacrosManager()->ReplaceMacros(src))) return false;
            return wxCopyFile(Manager::Get()->GetMacrosManager()->ReplaceMacros(src),
                            Manager::Get()->GetMacrosManager()->ReplaceMacros(dst),
                            overwrite);
        }

        bool RenameFile(const wxString& src, const wxString& dst)
        {
            if (!wxFileExists(Manager::Get()->GetMacrosManager()->ReplaceMacros(src))) return false;
            return wxRenameFile(Manager::Get()->GetMacrosManager()->ReplaceMacros(src),
                                Manager::Get()->GetMacrosManager()->ReplaceMacros(dst));
        }

        bool RemoveFile(const wxString& src)
        {
            if (!wxFileExists(Manager::Get()->GetMacrosManager()->ReplaceMacros(src))) return false;
            return wxRemoveFile(Manager::Get()->GetMacrosManager()->ReplaceMacros(src));
        }

        bool FileExists(const wxString& file)
        {
            return wxFileExists(Manager::Get()->GetMacrosManager()->ReplaceMacros(file));
        }

        wxString ChooseFile(const wxString& title, const wxString& defaultFile, const wxString& filter)
        {
            wxFileDialog dlg(0,
                            title,
                            wxEmptyString,
                            Manager::Get()->GetMacrosManager()->ReplaceMacros(defaultFile),
                            filter,
                            wxOPEN);
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
                return dlg.GetPath();
            return wxEmptyString;
        }

        wxString ReadFileContents(const wxString& filename)
        {
            wxFile f(filename);
            return cbReadFileContents(f);
        }
    } // namespace IOLib
} // namespace ScriptBindings

namespace ScriptBindings
{
    struct IONamespace {};

    void Register_IO()
    {
        SqPlus::SQClassDef<IONamespace>("IO").
                staticFunc(&IOLib::DirectoryExists, "DirectoryExists").
                staticFunc(&IOLib::ChooseDir, "SelectDirectory").
                staticFunc(&IOLib::FileExists, "FileExists").
                staticFunc(&IOLib::ChooseFile, "SelectFile").
                staticFunc(&IOLib::ReadFileContents, "ReadFileContents");
    }
} // namespace ScriptBindings
