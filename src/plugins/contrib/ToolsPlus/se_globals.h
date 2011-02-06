#ifndef IL_GLOBALS_H
#define IL_GLOBALS_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/filename.h>
 
    #include <editorbase.h>
    #include <editormanager.h>
    #include <logmanager.h>
    #include <manager.h>
    #include <macrosmanager.h>
    #include <configmanager.h>
    #include <cbproject.h>
#endif

wxString GetParentDir(const wxString &path);

bool DirIsChildOf(const wxString &path, const wxString &child);

bool WildCardListMatch(wxString list, wxString name, bool strip=true);

bool PromptSaveOpenFile(wxString message, wxFileName path);

inline void LogMessage(const wxString &msg)
{ Manager::Get()->GetLogManager()->Log(msg); }

inline void LogErrorMessage(const wxString &msg)
{ Manager::Get()->GetLogManager()->LogError(msg); }


inline int MessageBox(wxWindow *parent, const wxString& message, const wxString& caption = wxEmptyString, int style = wxOK, int x = -1, int y = -1) { return cbMessageBox(message, caption, style, parent, x, y);}

#endif //IL_GLOBALS_H
