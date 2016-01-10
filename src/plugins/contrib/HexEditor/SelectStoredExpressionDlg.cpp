/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "SelectStoredExpressionDlg.h"
#include <configmanager.h>

//(*InternalHeaders(SelectStoredExpressionDlg)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(SelectStoredExpressionDlg)
const long SelectStoredExpressionDlg::ID_LISTBOX1 = wxNewId();
const long SelectStoredExpressionDlg::ID_STATICTEXT1 = wxNewId();
const long SelectStoredExpressionDlg::ID_TEXTCTRL1 = wxNewId();
const long SelectStoredExpressionDlg::ID_BUTTON1 = wxNewId();
const long SelectStoredExpressionDlg::ID_BUTTON2 = wxNewId();
const long SelectStoredExpressionDlg::ID_BUTTON3 = wxNewId();
const long SelectStoredExpressionDlg::ID_BUTTON4 = wxNewId();
const long SelectStoredExpressionDlg::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SelectStoredExpressionDlg,wxScrollingDialog)
	//(*EventTable(SelectStoredExpressionDlg)
	//*)
END_EVENT_TABLE()

SelectStoredExpressionDlg::SelectStoredExpressionDlg(wxWindow* parent,const wxString& startingExpression)
{
    m_Expression = startingExpression;
    m_CacheChanged = false;
	BuildContent(parent);
	ReadExpressions();
	RecreateExpressionsList();
}

void SelectStoredExpressionDlg::BuildContent(wxWindow* parent)
{
	//(*Initialize(SelectStoredExpressionDlg)
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Stored Expressions"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Stored expressions"));
	m_Expressions = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(292,119), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(m_Expressions, 1, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	m_Filter = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer2->Add(m_Filter, 1, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxBOTTOM|wxEXPAND, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this, ID_BUTTON1, _("New"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer3->Add(Button1, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("Clone"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer3->Add(Button2, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	Button3 = new wxButton(this, ID_BUTTON3, _("Modify"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer3->Add(Button3, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	Button4 = new wxButton(this, ID_BUTTON4, _("Delete"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON4"));
	BoxSizer3->Add(Button4, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer3, 0, wxBOTTOM|wxRIGHT|wxEXPAND, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&SelectStoredExpressionDlg::Onm_ExpressionsSelect);
	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::Onm_ExpressionsDClick);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&SelectStoredExpressionDlg::Onm_FilterText);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&SelectStoredExpressionDlg::Onm_FilterTextEnter);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnButton1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnButton2Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnButton3Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnButton4Click);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnTimer1Trigger);
	//*)

	Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnOkClick);
	Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectStoredExpressionDlg::OnCancelClick);
}

SelectStoredExpressionDlg::~SelectStoredExpressionDlg()
{
	//(*Destroy(SelectStoredExpressionDlg)
	//*)
}

void SelectStoredExpressionDlg::ReadExpressions()
{
    ConfigManager* mgr = Manager::Get()->GetConfigManager( _T("lib_finder") );

    const wxString basePath = _T("/storedexpressions");

    wxArrayString keys = mgr->EnumerateSubPaths( basePath );
    for ( size_t i=0; i<keys.Count(); ++i )
    {
        wxString path = basePath + _T("/") + keys[i] + _T("/");
        wxString name = mgr->Read( path + _T("name") );
        wxString value = mgr->Read( path + _T("value") );

        if ( !name.IsEmpty() && !value.IsEmpty() )
        {
            m_Cache[ name ] = value;
        }
    }
}

void SelectStoredExpressionDlg::StoreExpressions()
{
    ConfigManager* mgr = Manager::Get()->GetConfigManager( _T("lib_finder") );

    const wxString basePath = _T("/storedexpressions");

    mgr->DeleteSubPath( _T("/storedexpressions") );

    int index = 0;
    for ( CacheT::iterator i = m_Cache.begin(); i != m_Cache.end(); ++i )
    {
        wxString path = basePath + _T("/") + wxString::Format( _T("expr_%d"), index++ ) + _T("/");
        mgr->Write( path + _T("name"),  i->first );
        mgr->Write( path + _T("value"), i->second );
    }
}

void SelectStoredExpressionDlg::RecreateExpressionsList( const wxString& selectionHint )
{
    m_Expressions->Clear();
    wxString filter = m_Filter->GetValue();

    for ( CacheT::iterator i = m_Cache.begin(); i != m_Cache.end(); ++i )
    {
        const wxString& name = i->first;
        const wxString& value = i->second;

        if ( filter.IsEmpty() || name.Find( filter ) != wxNOT_FOUND || value.Find( filter ) != wxNOT_FOUND )
        {
            wxString final = wxString::Format( _T("%s: %s"), name.c_str(), value.c_str() );
            int thisIndex = m_Expressions->Append( final, new ListData( i ) );

            if ( !selectionHint.IsEmpty() && name == selectionHint )
            {
                m_Expressions->Select( thisIndex );
            }
        }
    }

    if ( m_Expressions->GetCount() > 0 && m_Expressions->GetSelection() == wxNOT_FOUND )
    {
        m_Expressions->SetSelection( 0 );
    }
}

void SelectStoredExpressionDlg::OnButton1Click(wxCommandEvent& /*event*/)
{
    AddingExpression( wxEmptyString, m_Expression );
}

void SelectStoredExpressionDlg::AddingExpression( const wxString& defaultName, const wxString& defaultExpression )
{
    wxString name = defaultName;
    for (;;)
    {
        name = cbGetTextFromUser( _("Enter name for this expression"), _("New stored expression"), name );

        if ( name.IsEmpty() )
        {
            return;
        }

        if ( m_Cache.find( name ) != m_Cache.end() )
        {
            int ret = cbMessageBox( _("Expression with such name already exists."
                                      "Do you want to replace it ?"),
                                    _("Duplicated expression"), wxYES_NO|wxCANCEL, this );

            if ( ret == wxID_CANCEL )
            {
                return;
            }

            if ( ret == wxID_YES )
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    wxString value = cbGetTextFromUser(
        _("Enter expression"),
        _("New stored expression"),
        defaultExpression );

    if ( value.IsEmpty() )
    {
        return;
    }

    // Delete filter if it won't show new expression
    wxString filter = m_Filter->GetValue();
    if ( !filter.IsEmpty() && name.Find( filter ) == wxNOT_FOUND && value.Find( filter ) == wxNOT_FOUND )
    {
        m_Filter->ChangeValue( wxEmptyString );
    }

    m_Cache[ name ] = value;
    m_CacheChanged = true;

	RecreateExpressionsList( name );
}

void SelectStoredExpressionDlg::Onm_ExpressionsSelect(wxCommandEvent& /*event*/)
{
}

void SelectStoredExpressionDlg::OnOkClick(wxCommandEvent& event)
{
    if ( m_Expressions->GetSelection() == wxNOT_FOUND )
    {
        return;
    }

    StoreExpressionsQuery();
    m_Expression = ( (ListData*)m_Expressions->GetClientObject( m_Expressions->GetSelection() ) )->GetValue();

    // Just to close the dialog properly
    event.Skip();
}

void SelectStoredExpressionDlg::OnCancelClick(wxCommandEvent& event)
{
    StoreExpressionsQuery();

    // Just to close the dialog properly
    event.Skip();
}

void SelectStoredExpressionDlg::StoreExpressionsQuery()
{
    if ( m_CacheChanged )
    {
       int ret = cbMessageBox( _("Expressions has changed.\nDo you want to save them?\n"), _("Changed expressions"), wxYES_NO );
       if ( ret == wxID_YES )
       {
           StoreExpressions();
       }
    }
    m_CacheChanged = false;
}

void SelectStoredExpressionDlg::OnButton2Click(wxCommandEvent& /*event*/)
{
    ListData* data = GetSelection();
    if ( !data ) return;

    AddingExpression( wxString::Format( _("Copy of %s"), data->GetKey().c_str() ), data->GetValue() );
}

SelectStoredExpressionDlg::ListData * SelectStoredExpressionDlg::GetSelection()
{
    if ( m_Expressions->GetSelection() == wxNOT_FOUND ) return 0;
    return (ListData*)m_Expressions->GetClientObject( m_Expressions->GetSelection() );
}


void SelectStoredExpressionDlg::OnButton3Click(wxCommandEvent& /*event*/)
{
    ListData* data = GetSelection();
    if ( !data ) return;

    wxString value = cbGetTextFromUser(
        _("Enter new expression"),
        _("Modifying expression"),
        data->GetValue() );

    wxString name = data->GetKey();

    if ( value.IsEmpty() )
    {
        return;
    }

    // Delete filter if it won't show new expression
    wxString filter = m_Filter->GetValue();
    if ( !filter.IsEmpty() && name.Find( filter ) == wxNOT_FOUND && value.Find( filter ) == wxNOT_FOUND )
    {
        m_Filter->ChangeValue( wxEmptyString );
    }

    m_Cache[ name ] = value;
    m_CacheChanged = true;

	RecreateExpressionsList( name );
}


void SelectStoredExpressionDlg::Onm_FilterText(wxCommandEvent& /*event*/)
{
    Timer1.Start( 1000, true );
}

void SelectStoredExpressionDlg::Onm_FilterTextEnter(wxCommandEvent& /*event*/)
{
    FilterUpdated();
}

void SelectStoredExpressionDlg::OnTimer1Trigger(wxTimerEvent& /*event*/)
{
    FilterUpdated();
}

void SelectStoredExpressionDlg::FilterUpdated()
{
    Timer1.Stop();
    ListData* data = GetSelection();
    wxString name;
    if ( data ) name = data->GetKey();
    RecreateExpressionsList( name );
}

void SelectStoredExpressionDlg::OnButton4Click(wxCommandEvent& /*event*/)
{
    ListData* data = GetSelection();
    if ( !data ) return;
    m_Cache.erase( data->GetKey() );
    m_CacheChanged = true;
    RecreateExpressionsList();
}

void SelectStoredExpressionDlg::Onm_ExpressionsDClick(wxCommandEvent& event)
{
    OnOkClick( event );
    EndDialog( wxID_OK );
}
