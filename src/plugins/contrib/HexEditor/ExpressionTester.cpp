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

#include "ExpressionTester.h"

#include "ExpressionParser.h"
#include "ExpressionExecutor.h"
#include "ExpressionTestCases.h"
#include "SelectStoredExpressionDlg.h"
#include "TestCasesDlg.h"


#include <globals.h>

//(*InternalHeaders(ExpressionTester)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(ExpressionTester)
const long ExpressionTester::ID_STATICTEXT1 = wxNewId();
const long ExpressionTester::ID_TEXTCTRL1 = wxNewId();
const long ExpressionTester::ID_BUTTON3 = wxNewId();
const long ExpressionTester::ID_STATICTEXT2 = wxNewId();
const long ExpressionTester::ID_STATICTEXT3 = wxNewId();
const long ExpressionTester::ID_STATICTEXT4 = wxNewId();
const long ExpressionTester::ID_STATICTEXT5 = wxNewId();
const long ExpressionTester::ID_STATICTEXT6 = wxNewId();
const long ExpressionTester::ID_TEXTCTRL2 = wxNewId();
const long ExpressionTester::ID_BUTTON4 = wxNewId();
const long ExpressionTester::ID_BUTTON2 = wxNewId();
const long ExpressionTester::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ExpressionTester,wxScrollingDialog)
	//(*EventTable(ExpressionTester)
	//*)
END_EVENT_TABLE()

ExpressionTester::ExpressionTester( wxWindow* parent, FileContentBase* content, FileContentBase::OffsetT current )
{
    m_Content = content;
    m_Current = current;
	BuildContent(parent);
}

void ExpressionTester::BuildContent(wxWindow* parent)
{
	//(*Initialize(ExpressionTester)
	wxFlexGridSizer* FlexGridSizer1;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("ExpressionTester"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 5, 5);
	FlexGridSizer1->AddGrowableCol(1);
	FlexGridSizer1->AddGrowableRow(3);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Expr:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	m_Expr = new wxTextCtrl(this, ID_TEXTCTRL1, _("1 + 2 * 3"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer3->Add(m_Expr, 1, wxALIGN_CENTER_VERTICAL, 5);
	Button3 = new wxButton(this, ID_BUTTON3, _("v"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer3->Add(Button3, 0, wxLEFT|wxEXPAND, 5);
	FlexGridSizer1->Add(BoxSizer3, 1, wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Parsing:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALIGN_CENTER_VERTICAL, 5);
	m_Parsing = new wxStaticText(this, ID_STATICTEXT3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(m_Parsing, 1, wxEXPAND, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Result:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALIGN_CENTER_VERTICAL, 5);
	m_Result = new wxStaticText(this, ID_STATICTEXT5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(m_Result, 1, wxEXPAND, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT6, _("Code dump:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(StaticText3, 1, wxALIGN_CENTER_VERTICAL, 5);
	m_Dump = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(m_Dump, 1, wxEXPAND, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button4 = new wxButton(this, ID_BUTTON4, _("Auto test"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	Button4->SetToolTip(_("Perform automatic parser and executor test"));
	BoxSizer2->Add(Button4, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(15,9,1, wxEXPAND, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("\?"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer2->Add(Button2, 0, wxRIGHT|wxEXPAND, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("Parse"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxRIGHT|wxEXPAND, 5);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxEXPAND, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(357,2,0, wxEXPAND, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&ExpressionTester::OnButton1Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ExpressionTester::OnButton3Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ExpressionTester::OnButton4Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ExpressionTester::OnButton2Click);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ExpressionTester::OnButton1Click);
	//*)
}

ExpressionTester::~ExpressionTester()
{
	//(*Destroy(ExpressionTester)
	//*)
}


void ExpressionTester::OnButton1Click(wxCommandEvent& /*event*/)
{
    Expression::Parser parser;
    Expression::Preprocessed code;

    if ( parser.Parse( m_Expr->GetValue(), code ) )
    {
        m_Parsing->SetLabel( _("OK") );
        m_Dump->SetValue(
            _("Code dump:\n") + code.DumpCode() +
            _("====================\n"
              "Arguments:\n") + code.DumpArgs() );

        Expression::Executor exec;
        if ( exec.Execute( code, m_Content, m_Current ) )
        {
            unsigned long long v1;
            long long v2;
            long double v3;
            if ( exec.GetResult( v1 ) )
            {
                m_Result->SetLabel( wxString::Format( _T("%llu"), v1 ) );
            }
            else if ( exec.GetResult( v2 ) )
            {
                m_Result->SetLabel( wxString::Format( _T("%lld"), v2 ) );
            }
            else if ( exec.GetResult( v3 ) )
            {
                m_Result->SetLabel( wxString::Format( _T("%f"), (double)v3 ) );
            }
            else
            {
                m_Result->SetLabel( _("???") );
            }
        }
        else
        {
            m_Result->SetLabel( _("Error: ") + exec.ErrorDesc() );
        }
    }
    else
    {
        m_Result->SetLabel( _T("---") );
        int pos = 0;
        wxString desc = parser.ParseErrorDesc( pos );
        m_Parsing->SetLabel( wxString::Format( _("Err at %d: %s"), pos, desc.wx_str() ) );
    }
}

void ExpressionTester::OnButton2Click(wxCommandEvent& /*event*/)
{
    cbMessageBox( Expression::Parser::GetHelpString() );

}

void ExpressionTester::OnButton3Click(wxCommandEvent& event)
{
    SelectStoredExpressionDlg dlg( this, m_Expr->GetValue() );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_Expr->SetValue( dlg.GetExpression() );
        OnButton1Click(event);
    }
}

void ExpressionTester::OnButton4Click(wxCommandEvent& /*event*/)
{
    TestCasesDlg( this, Expression::GetTests() ).ShowModal();
}
