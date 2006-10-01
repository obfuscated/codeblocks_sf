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
    #include <wx/arrstr.h>
    #include <wx/sizer.h>
#endif
#include "simplelistlog.h" // class's header file

int ID_List = wxNewId();


// class constructor
SimpleListLog::SimpleListLog(bool fixedPitchFont)
{
	Init(fixedPitchFont);
}

SimpleListLog::SimpleListLog(int numCols, int widths[], const wxArrayString& titles, bool fixedPitchFont)
{
	Init(fixedPitchFont);
	SetColumns(numCols, widths, titles);
}

void SimpleListLog::Init(bool fixedPitchFont)
{
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
	m_pList = new wxListCtrl(this, ID_List, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    bs->Add(m_pList, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

    m_UseFixedPitchFont = fixedPitchFont;
    m_pList->SetFont(GetDefaultLogFont(fixedPitchFont));
}

void SimpleListLog::ResetLogFont()
{
    m_pList->SetFont(GetDefaultLogFont(m_UseFixedPitchFont));
}

// class destructor
SimpleListLog::~SimpleListLog()
{
}

void SimpleListLog::AddLog(const wxString& msg, bool addNewLine)
{
	wxArrayString array;
	array.Add(msg);
	AddLog(array);
}

void SimpleListLog::SetColumns(int num, int widths[], const wxArrayString& titles)
{
	Clear();
	m_pList->ClearAll();

	for (int i = 0; i < num; ++i)
	{
		m_pList->InsertColumn(i, titles[i], wxLIST_FORMAT_LEFT, widths[i]);
	}
}

void SimpleListLog::AddLog(const wxArrayString& values)
{
	if (!values.GetCount())
		return;

	m_pList->Freeze();
	m_pList->InsertItem(m_pList->GetItemCount(), values[0]);
	int idx = m_pList->GetItemCount() - 1;
	for (int i = 1; i < m_pList->GetColumnCount() && i < (int)values.GetCount(); ++i)
	{
		m_pList->SetItem(idx, i, values[i]);
	}
	m_pList->Thaw();
}

void SimpleListLog::Clear()
{
	m_pList->Freeze();
	m_pList->DeleteAllItems();
	m_pList->Thaw();
}
