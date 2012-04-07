/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "databreakpointdlg.h"

//(*InternalHeaders(DataBreakpointDlg)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(DataBreakpointDlg)
const long DataBreakpointDlg::ID_CHECKBOX1 = wxNewId();
const long DataBreakpointDlg::ID_TEXTCTRL_DATA_EXPRESION = wxNewId();
const long DataBreakpointDlg::ID_RADIOBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DataBreakpointDlg,wxScrollingDialog)
    //(*EventTable(DataBreakpointDlg)
    //*)
END_EVENT_TABLE()

DataBreakpointDlg::DataBreakpointDlg(wxWindow *parent, const wxString& dataExpression, bool enabled, int selection)
{
    //(*Initialize(DataBreakpointDlg)
    Create(parent, wxID_ANY, _("Data breakpoint"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    CheckBox1 = new wxCheckBox(this, ID_CHECKBOX1, _("Enabled"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBox1->SetValue(false);
    BoxSizer1->Add(CheckBox1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 8);
    m_dataExpressionCtrl = new wxTextCtrl(this, ID_TEXTCTRL_DATA_EXPRESION, _("Text"), wxDefaultPosition, wxSize(265,23), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL_DATA_EXPRESION"));
    m_dataExpressionCtrl->SetFocus();
    BoxSizer1->Add(m_dataExpressionCtrl, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 10);
    wxString __wxRadioBoxChoices_1[3] =
    {
        _("Break on read"),
        _("Break on write"),
        _("Break on read or write")
    };
    RadioBox1 = new wxRadioBox(this, ID_RADIOBOX1, _("Condition"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 1, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
    BoxSizer1->Add(RadioBox1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 8);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
    StdDialogButtonSizer1->Realize();
    BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 8);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();
    //*)

    CheckBox1->SetValue(enabled);
    RadioBox1->SetSelection(selection);
    m_dataExpressionCtrl->SetValue(dataExpression);
}

DataBreakpointDlg::~DataBreakpointDlg()
{
    //(*Destroy(DataBreakpointDlg)
    //*)
}

bool DataBreakpointDlg::IsBreakpointEnabled()
{
    return CheckBox1->IsChecked();
}

int DataBreakpointDlg::GetSelection()
{
    return RadioBox1->GetSelection();
}

wxString DataBreakpointDlg::GetDataExpression() const
{
    return m_dataExpressionCtrl->GetValue();
}
