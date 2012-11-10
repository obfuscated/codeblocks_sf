/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#if (__WXMSW__)
#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/filefn.h>
    #include <wx/filename.h>
    #include <wx/string.h>
    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "globals.h"
#endif //CB_PRECOMP
#include "cbstyledtextctrl.h"

#include "crashhandler.h"
#include <shlobj.h>

void CrashHandlerSaveEditorFiles(wxString& buf)
{
    wxString path;
    //get the "My Files" folder
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, wxStringBuffer(path, MAX_PATH));
    if (FAILED(result))
    {   //get at least the profiles folder
        path = ConfigManager::GetHomeFolder();
    }
    path << _T("\\cb-crash-recover");
    if (!wxDirExists(path)) wxMkdir(path);

    //make a sub-directory of the current date & time
    wxDateTime now = wxDateTime::Now();
    path << now.Format(_T("\\%Y%m%d-%H%M%S"));

    EditorManager* em = Manager::Get()->GetEditorManager();
    if (em)
    {
        bool AnyFileSaved = false;
        if (wxMkdir(path) && wxDirExists(path))
        {
            for (int i = 0; i < em->GetEditorsCount(); ++i)
            {
                cbEditor* ed = em->GetBuiltinEditor(em->GetEditor(i));
                if (ed)
                {
                    wxFileName fn(ed->GetFilename());
                    wxString fnpath = path + _T("/") + fn.GetFullName();
                    wxString newfnpath = fnpath;
                    // add number if filename already exists e.g. main.cpp.001, main.cpp.002, ...
                    int j = 1;
                    while (wxFileExists(newfnpath))
                        newfnpath = fnpath + wxString::Format(wxT(".%03d"),j);

                    if (cbSaveToFile(newfnpath,
                                    ed->GetControl()->GetText(),
                                    ed->GetEncoding(),
                                    ed->GetUseBom() ) )
                    {
                        AnyFileSaved = true;
                    }
                }
            }

            if (AnyFileSaved)
            {
                buf << _("The currently opened files have been saved to the directory\n");
                buf << path;
                buf << _("\nHopefully, this will prevent you from losing recent modifications.\n\n");
            }
            else
                wxRmdir(path);
        }
    }
}

LONG WINAPI CrashHandlerFunc(PEXCEPTION_POINTERS ExceptionInfo)
{
    static bool EditorFilesNotSaved = true;
    DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;

    if (code != EXCEPTION_ACCESS_VIOLATION && code != EXCEPTION_ILLEGAL_INSTRUCTION)
        return EXCEPTION_CONTINUE_SEARCH;

    wxString buf;
    buf.Printf(_("The application encountered a crash at address %u.\n\n"),
               (unsigned int) ExceptionInfo->ContextRecord->Eip);

    if (EditorFilesNotSaved)
    {
        CrashHandlerSaveEditorFiles(buf);
        EditorFilesNotSaved = false;
    }

    buf << _("Now you have three options:\n"
              "1. Press 'Abort' to pass control back to the system. This will normally display the standard 'application error' message and kill the program.\n"
              "2. Press 'Ignore' to step over the offending instruction. You may run into another access violation, but if you are lucky enough, you might get to save your work and close the program gracefully.\n"
              "3. Press 'Retry' to return to the offending instruction (this is almost certain to fail again, but might nevertheless work in rare cases).");


    switch(MessageBox(0, buf.c_str(), _T("Woah!"), MB_ABORTRETRYIGNORE))
    {
        case IDABORT:
        return EXCEPTION_CONTINUE_SEARCH;
        break;

        case IDIGNORE:
        ExceptionInfo->ContextRecord->Eip += 2;
        return EXCEPTION_CONTINUE_EXECUTION;
        break;

        case IDRETRY:
        return EXCEPTION_CONTINUE_EXECUTION;
        break;
    }
    return EXCEPTION_CONTINUE_SEARCH;
};


CrashHandler::CrashHandler(bool bDisabled) : handler(0)
{
    if (!bDisabled)
    {
        AddHandler_t AddHandler = (AddHandler_t) GetProcAddress(GetModuleHandle(_T("kernel32")), "AddVectoredExceptionHandler");

        if (AddHandler)
            handler = AddHandler(1, CrashHandlerFunc);
    }
}

CrashHandler::~CrashHandler()
{
    if (handler)
    {
        RemoveHandler_t RemoveHandler = (RemoveHandler_t) GetProcAddress(GetModuleHandle(_T("kernel32")), "RemoveVectoredExceptionHandler");
        RemoveHandler(handler);
    }
}

#endif //(__WXMSW__)
