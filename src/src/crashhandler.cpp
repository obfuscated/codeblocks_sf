#include <sdk.h>
#include "crashhandler.h"


#if (__WXMSW__)

LONG WINAPI CrashHandlerFunc(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    wxLogNull nl;
    wxString path(ConfigManager::GetHomeFolder() + _T("cb-crash-recover"));
    wxMkdir(ConfigManager::GetHomeFolder() + _T("cb-crash-recover"));
    path << _T("/");

    EditorManager* em = Manager::Get()->GetEditorManager();

    if(em)
    {
    for(int i = 0; i < em->GetEditorsCount(); ++i)
        {
            cbEditor* ed = em->GetBuiltinEditor(em->GetEditor(i));
            if(ed)
            {
            wxFileName fn(ed->GetFilename());
            cbSaveToFile(path + fn.GetFullName(), ed->GetControl()->GetText(), ed->GetEncoding(), ed->GetUseBom());
            }

        }
    }

    wxString buf;
    buf.Printf(_T("The application caused an access violation from address %u.\n\n"
                  "A snapshot of the present state of Code::Blocks has been saved to the directory cb-crash-recover inside your 'My Files' folder. Hopefully, this will prevent you from losing recent modifications.\n\n"
                  "You now have three options:\n"
                  "1. Press 'Abort' to pass control back to the system. This will normally display the standard 'application error' message and kill the program.\n"
                  "2. Press 'Ignore' to step over the offending instruction. You may run into another access violation, but if you are lucky enough, you might get to save your work and close the program gracefully.\n"
                  "3. Press 'Retry' to return to the offending instruction (this is almost certain to fail again, but might nevertheless work in rare cases)."),
               (unsigned int) ExceptionInfo->ContextRecord->Eip);


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


CrashHandler::CrashHandler() : handler(0)
{
        AddHandler_t AddHandler = (AddHandler_t) GetProcAddress(GetModuleHandle(_T("kernel32")), "AddVectoredExceptionHandler");

        if (AddHandler)
            handler = AddHandler(1, CrashHandlerFunc);
}

CrashHandler::~CrashHandler()
{
    if(handler)
    {
        RemoveHandler_t RemoveHandler = (RemoveHandler_t) GetProcAddress(GetModuleHandle(_T("kernel32")), "RemoveVectoredExceptionHandler");
        RemoveHandler(handler);
    }
}


#endif

