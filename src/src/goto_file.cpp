/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#include "goto_file.h"

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(GotoFile)
    #include <wx/sizer.h>
    #include <wx/listctrl.h>
    #include <wx/string.h>
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    //*)
#endif
//(*InternalHeaders(GotoFile)
//*)

//(*IdInit(GotoFile)
const long GotoFile::ID_TEXTCTRL1 = wxNewId();
const long GotoFile::ID_RESULT_LIST = wxNewId();
//*)

BEGIN_EVENT_TABLE(GotoFile,wxDialog)
    //(*EventTable(GotoFile)
    //*)
END_EVENT_TABLE()

GotoFile::GotoFile(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                   const wxString &message) :
    m_handler(this, iterator)
{
    BuildContent(parent, iterator, title, message);

    m_handler.Init(m_ResultList, m_Text);
}

void GotoFile::BuildContent(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                            const wxString &message)
{
    //(*Initialize(GotoFile)
    wxStaticText* labelCtrl;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("wxID_ANY"));
    m_sizer = new wxBoxSizer(wxVERTICAL);
    labelCtrl = new wxStaticText(this, wxID_ANY, _("Some text"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    m_sizer->Add(labelCtrl, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_Text = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_Text->SetFocus();
    m_sizer->Add(m_Text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_ResultList = new IncrementalListCtrl(this, ID_RESULT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_RESULT_LIST"));
    m_ResultList->SetMinSize(wxSize(500,300));
    m_sizer->Add(m_ResultList, 1, wxALL|wxEXPAND, 5);
    SetSizer(m_sizer);
    m_sizer->Fit(this);
    m_sizer->SetSizeHints(this);
    //*)

    // Add first column
    wxListItem column;
    column.SetId(0);
    column.SetText( _("Column") );
    column.SetWidth(300);
    m_ResultList->InsertColumn(0, column);
    m_ResultList->SetIterator(iterator);

    SetTitle(title);
    labelCtrl->SetLabel(message);
}

GotoFile::~GotoFile()
{
    m_handler.DeInit(this);

    //(*Destroy(GotoFile)
    //*)
}

int GotoFile::GetSelection()
{
    return m_handler.GetSelection();
}

void GotoFile::AddControlBelowList(wxControl *control)
{
    wxSize sz = GetSize();
    m_sizer->Add(control, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT, 8);
    Fit();
    SetMinSize(GetSize());
    SetSize(sz);
}
