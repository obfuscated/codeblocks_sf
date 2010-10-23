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
    wxFileDialog* m_SaveFile;
    wxStatusBar* m_StatuBar;
    wxTextCtrl* m_LogCtrl;
    wxFileDialog* m_OpenFile;
    //*)

    size_t               m_LogCount;
    wxString             m_Log;

    wxFindReplaceData    m_FindData;
    wxFindReplaceDialog* m_DlgFind;
    int                  m_LastIndex;
    wxString             m_LastFile;

    ParserTest           m_ParserTest;

    DECLARE_EVENT_TABLE()
};

#endif
