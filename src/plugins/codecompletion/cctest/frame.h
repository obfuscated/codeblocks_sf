/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef FRAME_H
#define FRAME_H

#include "cc_test.h"

//(*Headers(Frame)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include <wx/fdrepdlg.h>

class wxProgressDialog;

class Frame : public wxFrame
{
public:
    Frame(const wxString& main_file);
    virtual ~Frame();

    void AppendToLog(const wxString& log);
    void Start();

protected:
    //(*Identifiers(Frame)
    static const long wxID_TOKEN;
    //*)

private:
    //(*Handlers(Frame)
    void OnMenuQuitSelected(wxCommandEvent& event);
    void OnMenuAboutSelected(wxCommandEvent& event);
    void OnMenuSaveSelected(wxCommandEvent& event);
    void OnMenuOpenSelected(wxCommandEvent& event);
    void OnMenuFindSelected(wxCommandEvent& event);
    void OnMenuReloadSelected(wxCommandEvent& event);
    void OnDoHeadersClick(wxCommandEvent& event);
    void OnMenuTokenSelected(wxCommandEvent& event);
    //*)

    void OnFindDialog(wxFindDialogEvent& event);
    void OnCCLogger(wxCommandEvent& event);
    void OnCCAddToken(wxCommandEvent& event);

    //(*Declarations(Frame)
    wxTextCtrl* m_TreeCtrl;
    wxCheckBox* m_DoTreeCtrl;
    wxFileDialog* m_SaveFile;
    wxStatusBar* m_StatuBar;
    wxTextCtrl* m_LogCtrl;
    wxCheckBox* m_DoHeadersCtrl;
    wxTextCtrl* m_HeadersCtrl;
    wxTextCtrl* m_IncludeCtrl;
    wxFileDialog* m_OpenFile;
    //*)

    wxProgressDialog*    m_ProgDlg;

    wxString             m_MainFile;
    wxString             m_CurrentFile;
    size_t               m_LogCount;

    wxFindReplaceData    m_FRData;
    wxFindReplaceDialog* m_FRDlg;
    int                  m_LastIndex;

    DECLARE_EVENT_TABLE()
};

#endif
