/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk_precomp.h>
#include <manager.h>
#include <macrosmanager.h>
#include <configmanager.h>
#include <annoyingdialog.h>

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "menuitemsmanager.h"
    #include "scriptingmanager.h"
    #include <wx/string.h>
    #include <wx/filedlg.h>
#endif

#include <wx/filename.h>
#include <wx/utils.h>

#include "scripting/bindings/sc_utils.h"
#include "scripting/bindings/sc_typeinfo_all.h"
#include "scriptsecuritywarningdlg.h"

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
            PlaceWindow(&dlg);
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

        SQInteger GetCwd(HSQUIRRELVM v)
        {
            // env table
            ExtractParams1<SkipParam> extractor(v);
            if (!extractor.Process("IO::GetCwd"))
                return extractor.ErrorMessage();
            const wxString result = wxGetCwd();
            return ConstructAndReturnInstance(v, result);
        }

        SQInteger SetCwd(HSQUIRRELVM v)
        {
            // env table, dir
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::SetCwd"))
                return extractor.ErrorMessage();

            wxSetWorkingDirectory(*extractor.p1);
            return 0;
        }

        SQInteger CreateDirRecursively(HSQUIRRELVM v)
        {
            // env table, full_path, perms
            ExtractParams3<SkipParam, const wxString *, SQInteger> extractor(v);
            if (!extractor.Process("IO::CreateDirectory"))
                return extractor.ErrorMessage();

            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);
            if (SecurityAllows(_T("CreateDir"), fname.GetFullPath()))
            {
                const int perms = extractor.p2;
                sq_pushbool(v, ::CreateDirRecursively(fname.GetFullPath(), perms));
            }
            else
                sq_pushbool(v, false);
            return 1;
        }

        SQInteger SelectDirectory(HSQUIRRELVM v)
        {
            // env table, message, initialPath, showCreateDirButton
            ExtractParams4<SkipParam, const wxString *, const wxString *, bool> extractor(v);
            if (!extractor.Process("IO::SelectDirectory"))
                return extractor.ErrorMessage();

            const wxString &path = Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p2);
            const wxString &result = ChooseDirectory(nullptr, *extractor.p1, path, wxString(),
                                                     false, extractor.p3);
            return ConstructAndReturnInstance(v, result);
        }

        SQInteger RemoveDirectory(HSQUIRRELVM v)
        {
            // env table, src
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::RemoveDirectory"))
                return extractor.ErrorMessage();

            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);
            if (SecurityAllows(_T("RemoveDir"), fname.GetFullPath()))
                sq_pushbool(v, wxRmdir(fname.GetFullPath()));
            else
                sq_pushbool(v, false);
            return 1;
        }

        SQInteger DirectoryExists(HSQUIRRELVM v)
        {
            // env table, dir
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::DirectoryExists"))
                return extractor.ErrorMessage();

            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);
            sq_pushbool(v, wxDirExists(fname.GetFullPath()));
            return 1;
        }

        SQInteger CopyFile(HSQUIRRELVM v)
        {
            // env table, src, dst, overwrite
            ExtractParams4<SkipParam, const wxString *, const wxString *, bool> extractor(v);
            if (!extractor.Process("IO::CopyFile"))
                return extractor.ErrorMessage();

            const wxString &src = *extractor.p1;
            const wxString &dst = *extractor.p2;

            MacrosManager *macros = Manager::Get()->GetMacrosManager();

            wxFileName fnameSrc(macros->ReplaceMacros(src));
            wxFileName fnameDst(macros->ReplaceMacros(dst));
            NormalizePath(fnameSrc, wxEmptyString);
            NormalizePath(fnameDst, wxEmptyString);

            bool result = false;

            // FIXME (squirrel) This format differs from the one in RenameFile!
            if (SecurityAllows("CopyFile", wxString::Format("%s -> %s", src, dst)))
            {
                const wxString &srcFullPath = fnameSrc.GetFullPath();
                if (wxFileExists(srcFullPath))
                {
                    // FIXME (squirrel) Redirect the wxWidgets logger to somewhere else.
                    // Currently if the destination file exists there will be a log error message
                    // box shown.
                    result = wxCopyFile(srcFullPath, fnameDst.GetFullPath(), extractor.p3);
                }
            }
            sq_pushbool(v, result);
            return 1;
        }

        SQInteger RenameFile(HSQUIRRELVM v)
        {
            // env table, src, dst
            ExtractParams3<SkipParam, const wxString *, const wxString *> extractor(v);
            if (!extractor.Process("IO::RenameFile"))
                return extractor.ErrorMessage();

            MacrosManager *macros = Manager::Get()->GetMacrosManager();

            wxFileName fnameSrc(macros->ReplaceMacros(*extractor.p1));
            wxFileName fnameDst(macros->ReplaceMacros(*extractor.p2));
            NormalizePath(fnameSrc, wxEmptyString);
            NormalizePath(fnameDst, wxEmptyString);

            const wxString &srcFullPath = fnameSrc.GetFullPath();
            const wxString &dstFullPath = fnameDst.GetFullPath();

            bool result = false;
            if (SecurityAllows("RenameFile", wxString::Format("%s -> %s", srcFullPath, dstFullPath)))
            {
                if (wxFileExists(srcFullPath))
                    result = wxRenameFile(srcFullPath, dstFullPath);
            }
            sq_pushbool(v, result);
            return 1;
        }

        SQInteger RemoveFile(HSQUIRRELVM v)
        {
            // env table, src
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::RemoveFile"))
                return extractor.ErrorMessage();
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);

            bool result = false;
            const wxString &fullPath = fname.GetFullPath();
            if (SecurityAllows("RemoveFile", fullPath))
            {
                if (wxFileExists(fullPath))
                    result = wxRemoveFile(fullPath);
            }
            sq_pushbool(v, result);
            return 1;
        }

        SQInteger FileExists(HSQUIRRELVM v)
        {
            // env table, file
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::FileExists"))
                return extractor.ErrorMessage();
            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);
            sq_pushbool(v, wxFileExists(fname.GetFullPath()));
            return 1;
        }

        SQInteger SelectFile(HSQUIRRELVM v)
        {
            // env table, title, defaultFile, filter
            ExtractParams4<SkipParam, const wxString *, const wxString *, const wxString *> extractor(v);
            if (!extractor.Process("IO::SelectFile"))
                return extractor.ErrorMessage();

            wxFileDialog dlg(nullptr, *extractor.p1, wxString(),
                            Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p2),
                            *extractor.p3, wxFD_OPEN | compatibility::wxHideReadonly);
            PlaceWindow(&dlg);
            wxString result;
            if (dlg.ShowModal() == wxID_OK)
                result = dlg.GetPath();
            return ConstructAndReturnInstance(v, result);
        }

        SQInteger ReadFileContents(HSQUIRRELVM v)
        {
            // env table, filename
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::ReadFileContents"))
                return extractor.ErrorMessage();

            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);
            wxFile f(fname.GetFullPath());
            return ConstructAndReturnInstance(v, cbReadFileContents(f));
        }

        SQInteger WriteFileContents(HSQUIRRELVM v)
        {
            // env table, filename, contents
            ExtractParams3<SkipParam, const wxString *, const wxString *> extractor(v);
            if (!extractor.Process("IO::WriteFileContents"))
                return extractor.ErrorMessage();

            wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1));
            NormalizePath(fname, wxEmptyString);
            const wxString &fullPath = fname.GetFullPath();
            bool result = false;
            if (SecurityAllows("CreateFile", fullPath))
            {
                wxFile f(fullPath, wxFile::write);
                result = cbWrite(f, *extractor.p2);
            }
            sq_pushbool(v, result);
            return 1;
        }

        SQInteger Execute(HSQUIRRELVM v)
        {
            // env table, command
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::Execute"))
                return extractor.ErrorMessage();
            const wxString &command = *extractor.p1;

            SQInteger result = -1;
            if (SecurityAllows("Execute", command))
            {
                wxArrayString output;
                result = wxExecute(command, output, wxEXEC_NODISABLE);
            }
            sq_pushinteger(v, result);
            return 1;
        }

        SQInteger ExecuteAndGetOutput(HSQUIRRELVM v)
        {
            // env table, command
            ExtractParams2<SkipParam, const wxString *> extractor(v);
            if (!extractor.Process("IO::ExecuteAndGetOutput"))
                return extractor.ErrorMessage();
            const wxString &command = *extractor.p1;

            wxString result;
            if (SecurityAllows("Execute", command))
            {
                wxArrayString output;
                wxExecute(command, output, wxEXEC_NODISABLE);
                result = GetStringFromArray(output, "\n");
            }
            return ConstructAndReturnInstance(v, result);
        }

        SQInteger ExecuteAndGetOutputAndError(HSQUIRRELVM v)
        {
            // env table, command, preprend_error
            ExtractParams3<SkipParam, const wxString *, bool> extractor(v);
            if (!extractor.Process("IO::ExecuteAndGetOutputAndError"))
                return extractor.ErrorMessage();
            const wxString &command = *extractor.p1;

            wxString result;
            if (SecurityAllows("Execute", command))
            {
                wxArrayString output;
                wxArrayString error;
                wxExecute(command, output, error, wxEXEC_NODISABLE);

                const bool preprendError = extractor.p2;

                if (preprendError && !error.empty())
                    result += GetStringFromArray(error, "\n");
                if (!output.empty())
                    result += GetStringFromArray(output, "\n");
                if (!preprendError && !error.empty())
                    result += GetStringFromArray(error, "\n");
            }
            return ConstructAndReturnInstance(v, result);
        }

    } // namespace IOLib
} // namespace ScriptBindings

namespace ScriptBindings
{

// Dummy type
struct IONamespace{};

template<>
struct TypeInfo<IONamespace> {
    static const uint32_t typetag = uint32_t(TypeTag::IONamespace);
    static constexpr const SQChar *className = _SC("IO");
    using baseClass = void;
};

void Register_IO(HSQUIRRELVM v, ScriptingManager *manager)
{
    PreserveTop preserve(v);
    sq_pushroottable(v);

    {
        // Register IO
        const SQInteger classDecl = CreateClassDecl<IONamespace>(v);
#ifndef NO_INSECURE_SCRIPTS
        BindStaticMethod(v, _SC("CreateDirectory"), IOLib::CreateDirRecursively,
                         _SC("IO::CreateDirectory"));
        BindStaticMethod(v, _SC("RemoveDirectory"), IOLib::RemoveDirectory,
                         _SC("IO::RemoveDirectory"));
        BindStaticMethod(v, _SC("CopyFile"), IOLib::CopyFile, _SC("IO::CopyFile"));
        BindStaticMethod(v, _SC("RenameFile"), IOLib::RenameFile, _SC("IO::RenameFile"));
        BindStaticMethod(v, _SC("RemoveFile"), IOLib::RemoveFile, _SC("IO::RemoveFile"));
        BindStaticMethod(v, _SC("WriteFileContents"), IOLib::WriteFileContents,
                         _SC("IO::WriteFileContents"));
        BindStaticMethod(v, _SC("Execute"), IOLib::Execute, _SC("IO::Execute"));
        BindStaticMethod(v, _SC("ExecuteAndGetOutput"), IOLib::ExecuteAndGetOutput,
                         _SC("IO::ExecuteAndGetOutput"));
        BindStaticMethod(v, _SC("ExecuteAndGetOutputAndError"), IOLib::ExecuteAndGetOutputAndError,
                         _SC("IO::ExecuteAndGetOutputAndError"));
        // FIXME (squirrel) Introduce an API for listing a directory
#endif // NO_INSECURE_SCRIPTS

        BindStaticMethod(v, _SC("GetCwd"), IOLib::GetCwd, _SC("IO::GetCwd"));
        BindStaticMethod(v, _SC("SetCwd"), IOLib::SetCwd, _SC("IO::SetCwd"));

        BindStaticMethod(v, _SC("DirectoryExists"), IOLib::DirectoryExists,
                         _SC("IO::DirectoryExists"));
        BindStaticMethod(v, _SC("SelectDirectory"), IOLib::SelectDirectory,
                         _SC("IO::SelectDirectory"));
        BindStaticMethod(v, _SC("FileExists"), IOLib::FileExists, _SC("IO::FileExists"));
        BindStaticMethod(v, _SC("SelectFile"), IOLib::SelectFile, _SC("IO::SelectFile"));
        BindStaticMethod(v, _SC("ReadFileContents"), IOLib::ReadFileContents,
                         _SC("IO::ReadFileContents"));

        // Put the class in the root table. This must be last!
        sq_newslot(v, classDecl, SQFalse);

#ifndef NO_INSECURE_SCRIPTS
        manager->BindBoolConstant("allowInsecureScripts", true);
#else
        manager->BindBoolConstant("allowInsecureScripts", false);
#endif // NO_INSECURE_SCRIPTS
    }

    // pop root table.
    sq_pop(v, 1);
}

} // namespace ScriptBinding
