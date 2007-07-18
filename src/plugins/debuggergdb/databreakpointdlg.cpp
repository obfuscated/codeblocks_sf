#include "databreakpointdlg.h"

//(*InternalHeaders(DataBreakpointDlg)
#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(DataBreakpointDlg)
const long DataBreakpointDlg::ID_CHECKBOX1 = wxNewId();
const long DataBreakpointDlg::ID_RADIOBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DataBreakpointDlg,wxDialog)
    //(*EventTable(DataBreakpointDlg)
    //*)
END_EVENT_TABLE()

DataBreakpointDlg::DataBreakpointDlg(wxWindow* parent,wxWindowID id,bool enabled,int selection)
{
    //(*Initialize(DataBreakpointDlg)
    Create(parent,id,_("Data breakpoint"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,_T("wxDialog"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    CheckBox1 = new wxCheckBox(this,ID_CHECKBOX1,_("Enabled"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
    CheckBox1->SetValue(false);
    BoxSizer1->Add(CheckBox1,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,8);
    wxString wxRadioBoxChoices_RadioBox1[3] =
    {
        _("Break on read"),
        _("Break on write"),
        _("Break on read or write")
    };
    RadioBox1 = new wxRadioBox(this,ID_RADIOBOX1,_("Condition"),wxDefaultPosition,wxDefaultSize,3,wxRadioBoxChoices_RadioBox1,1,wxRA_SPECIFY_ROWS,wxDefaultValidator,_T("ID_RADIOBOX1"));
    BoxSizer1->Add(RadioBox1,0,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,8);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_OK,wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_CANCEL,wxEmptyString));
    StdDialogButtonSizer1->Realize();
    BoxSizer1->Add(StdDialogButtonSizer1,0,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,8);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();
    //*)

    CheckBox1->SetValue(enabled);
    RadioBox1->SetSelection(selection);
}

DataBreakpointDlg::~DataBreakpointDlg()
{
    //(*Destroy(DataBreakpointDlg)
    //*)
}

bool DataBreakpointDlg::IsEnabled()
{
    return CheckBox1->IsChecked();
}

int DataBreakpointDlg::GetSelection()
{
    return RadioBox1->GetSelection();
}
