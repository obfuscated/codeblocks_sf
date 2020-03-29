/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#ifndef CBHELPER_H
#define CBHELPER_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
//------------------------------------------------------------------------------

static const int CASE_KEEP  = 0;
static const int CASE_LOWER = 1;
static const int CASE_UPPER = 2;

static const int QUOTE_AUTO   = 0;
static const int QUOTE_NEVER  = 1;
static const int QUOTE_ALWAYS = 2;

void ShowStringList(const CString& Title, const CString& LinePrefix,
                    const CStringList& StringList);
CString MakefileFriendly(const CString& AString);
CString XMLFriendly(const CString& AString);
CString FlatFileName(const CString& FileName);

#endif
//------------------------------------------------------------------------------
