#include <sdk_precomp.h>
#include <manager.h>
#include <macrosmanager.h>
#include "sc_io.h"
#include <angelscript.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <wx/string.h>
    #include <wx/filedlg.h>
#endif

class IOLib
{
    public:
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
};

static IOLib s_IO;

void Register_IO(asIScriptEngine* engine)
{
    int r;
    r = engine->RegisterObjectType("IOLib", 0, asOBJ_CLASS); //asCHECK_ERROR(r, _T("IO"));

    r = engine->RegisterObjectMethod("IOLib", "bool DirectoryExists(const wxString& in)", asMETHOD(IOLib, DirectoryExists), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
    r = engine->RegisterObjectMethod("IOLib", "wxString SelectDirectory(const wxString& in,const wxString& in,bool)", asMETHOD(IOLib, ChooseDir), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
//    r = engine->RegisterObjectMethod("IOLib", "bool CreateDirectory(const wxString& in, int)", asMETHOD(IOLib, CreateDirRecursively), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
//    r = engine->RegisterObjectMethod("IOLib", "bool RemoveDir(const wxString& in)", asMETHOD(IOLib, RemoveDir), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));

    r = engine->RegisterObjectMethod("IOLib", "bool FileExists(const wxString& in)", asMETHOD(IOLib, FileExists), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
    r = engine->RegisterObjectMethod("IOLib", "wxString SelectFile(const wxString& in,const wxString& in,const wxString& in)", asMETHOD(IOLib, ChooseFile), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
//    r = engine->RegisterObjectMethod("IOLib", "bool CopyFile(const wxString& in,const wxString& in,bool)", asMETHOD(IOLib, CopyFile), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
//    r = engine->RegisterObjectMethod("IOLib", "bool RenameFile(const wxString& in,const wxString& in)", asMETHOD(IOLib, RenameFile), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));
//    r = engine->RegisterObjectMethod("IOLib", "bool RemoveFile(const wxString& in)", asMETHOD(IOLib, RemoveFile), asCALL_THISCALL); //asCHECK_ERROR(r, _T("IO"));

    r = engine->RegisterGlobalProperty("IOLib IO", &s_IO); //asCHECK_ERROR(r, _T("IO"));
}
