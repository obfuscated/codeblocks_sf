/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef FRAME_H
#define FRAME_H

#include <wx/fdrepdlg.h>
#include "parsertest.h"

//(*Headers(Frame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class Frame : public wxFrame
{
public:
    Frame();
    virtual ~Frame();

    void Log(const wxString& log);
    void Start(const wxString& file);


private:
    void DoStart();
    void ShowLog();

protected:
    //(*Identifiers(Frame)
    static const long ID_LOGMAIN;
    static const long ID_OPEN;
    static const long ID_RELOAD;
    static const long ID_SAVE;
    static const long ID_QUIT;
    static const long ID_FIND;
    static const long ID_ABOUT;
    static const long ID_STATUSBAR;
    //*)

private:
    //(*Handlers(Frame)
    void OnMenuQuitSelected(wxCommandEvent& event);
    void OnMenuAboutSelected(wxCommandEvent& event);
    void OnMenuSaveSelected(wxCommandEvent& event);
    void OnMenuOpenSelected(wxCommandEvent& event);
    void OnMenuFindSelected(wxCommandEvent& event);
    void OnMenuReloadSelected(wxCommandEvent& event);
    //*)
    void OnFindDialog(wxFindDialogEvent& event);

private:
    //(*Declarations(Frame)
    wxFileDialog* m_openFile;
    wxFileDialog* m_saveFile;
    wxStatusBar* m_statuBar;
    wxTextCtrl* m_logCtrl;
    //*)

    unsigned int m_logCnt;
    wxString m_log;

    wxFindReplaceData m_findData;
    wxFindReplaceDialog* m_dlgFind;
    int m_lastIndex;
    wxString m_lastFile;

    ParserTest m_parserTest;

    DECLARE_EVENT_TABLE()
};

#endif
