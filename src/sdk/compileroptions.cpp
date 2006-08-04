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

#include "compileroptions.h"

CompilerOptions::CompilerOptions()
{
}

CompilerOptions::CompilerOptions(const CompilerOptions& other)
{
    // copy ctor
    *this = other;
}

CompilerOptions& CompilerOptions::operator=(const CompilerOptions& other)
{
    ClearOptions();
	for (unsigned int i = 0; i < other.m_Options.GetCount(); ++i)
	{
        CompOption* coption = new CompOption(*(other.m_Options[i]));
        AddOption(coption);
	}
	return *this;
}

CompilerOptions::~CompilerOptions()
{
	ClearOptions();
}

void CompilerOptions::ClearOptions()
{
	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* coption = m_Options.Item(i);
		delete coption;
	}
	m_Options.Clear();
}

void CompilerOptions::AddOption(CompOption* coption)
{
	m_Options.Add(coption);
}

void CompilerOptions::AddOption(const wxString& name,
								const wxString& option,
								const wxString& category,
								const wxString& additionalLibs,
								bool doChecks,
								const wxString& checkAgainst,
								const wxString& checkMessage)
{
	if (name.IsEmpty() || (option.IsEmpty() && additionalLibs.IsEmpty()))
		return;
	CompOption* coption = new CompOption;

	wxString listboxname = name + _T("  [");
	if (option.IsEmpty())
        listboxname += additionalLibs;
    else
        listboxname += option;
    listboxname += _T("]");

	coption->name = listboxname;
	coption->option = option;
	coption->additionalLibs = additionalLibs;
	coption->enabled = false;
	coption->category = category;
	coption->doChecks = doChecks;
	coption->checkAgainst = checkAgainst;
	coption->checkMessage = checkMessage;
	AddOption(coption);
}

CompOption* CompilerOptions::GetOptionByName(const wxString& name)
{
	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* coption = m_Options.Item(i);
		if (coption->name == name)
			return coption;
	}
	return 0L;
}

CompOption* CompilerOptions::GetOptionByOption(const wxString& option)
{
	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* coption = m_Options.Item(i);
		if (coption->option == option)
			return coption;
	}
	return 0L;
}

CompOption* CompilerOptions::GetOptionByAdditionalLibs(const wxString& libs)
{
	for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
	{
		CompOption* coption = m_Options.Item(i);
		if (coption->additionalLibs == libs)
			return coption;
	}
	return 0L;
}
