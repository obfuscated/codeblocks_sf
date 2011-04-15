/*
	This file is part of CodeSnippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
// RCS-ID: $Id$


///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 24 2006)
// http://wxformbuilder.sourceforge.net/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

//#include "wxscintilla/include/wx/wxscintilla.h" //svn5785
#include <wx/wxscintilla.h>                       //svn5785
#include "snippetpropertyform.h"
//-#include "edit.h"

///////////////////////////////////////////////////////////////////////////

SnippetPropertyForm::SnippetPropertyForm( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxScrollingDialog( parent, id, title, pos, size, style )
{
	wxBoxSizer* sbSizer;
	sbSizer = new wxBoxSizer(  wxVERTICAL );

	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxHORIZONTAL );
    // Label
	m_ItemLabelStaticText = new wxStaticText( this, ID_DEFAULT, wxT("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer->Add( m_ItemLabelStaticText, 0, wxALL, 5 );
    // Label Text Area
	//-m_ItemLabelTextCtrl = new wxTextCtrl( this, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	m_ItemLabelTextCtrl = new wxTextCtrl( this, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER  );
	bSizer->Add( m_ItemLabelTextCtrl, 1, wxALL, 5 );

	sbSizer->Add( bSizer, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
    // Snippet Label
	m_SnippetStaticText = new wxStaticText( this, ID_DEFAULT, wxT("Snippet | File Link"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_SnippetStaticText, 0, wxALL, 5 );

    // create wxscintilla edit control
    //-m_SnippetEditCtrl = new Edit(this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0);//|wxHSCROLL|wxNO_BORDER|wxVSCROLL|wxTE_MULTILINE  );
    m_SnippetEditCtrl = new wxScintilla(this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0);//|wxHSCROLL|wxNO_BORDER|wxVSCROLL|wxTE_MULTILINE  );
	bSizer1->Add( m_SnippetEditCtrl, 1, wxALL|wxEXPAND, 5 );

	sbSizer->Add( bSizer1, 1, wxEXPAND, 5 );
    // Buttons
	wxFlexGridSizer* fgSizer;
	fgSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer->SetFlexibleDirection( wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_SnippetButton = new wxButton( this, ID_SNIPPETBUTTON, wxT("ExtEdit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_SnippetButton, 0, wxALL, 5 );

	m_FileSelectButton = new wxButton( this, ID_FILESELECTBUTTON, wxT("Link target"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_FileSelectButton, 0, wxALL, 5 );

	fgSizer->Add( bSizer5, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer1->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer1->Realize();
	bSizer6->Add( m_sdbSizer1, 0, wxALL, 5 );

	fgSizer->Add( bSizer6, 1, wxEXPAND, 5 );

	sbSizer->Add( fgSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( sbSizer );
	this->Layout();
}
