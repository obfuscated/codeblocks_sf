/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/sizer.h>
    #include <wx/textctrl.h>
    #include "manager.h"
#endif
#include "simpletextlog.h" // class's header file

const int ID_Text = wxNewId();

// class constructor
SimpleTextLog::SimpleTextLog(bool fixedPitchFont)
{
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
	m_Text = new wxTextCtrl(this, ID_Text, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH2);
    bs->Add(m_Text, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

    m_UseFixedPitchFont = fixedPitchFont;
    m_Text->SetFont(GetDefaultLogFont(fixedPitchFont));
}

// class destructor
SimpleTextLog::~SimpleTextLog()
{
}

void SimpleTextLog::AddLog(const wxString& msg, bool addNewLine)
{
    m_Text->AppendText(msg);
    if (addNewLine)
        m_Text->AppendText(_T('\n'));
    // there's a small glitch in wxTextCtrl
    // this is a hack to always show some text...
	// (thanks to Kevin Altis's message in wx-users mailing list)
	m_Text->ScrollLines(-1);
	Manager::ProcessPendingEvents();
}

void SimpleTextLog::ResetLogFont()
{
    m_Text->SetFont(GetDefaultLogFont(m_UseFixedPitchFont));
}
