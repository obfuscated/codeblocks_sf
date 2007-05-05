/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: menuidentifiers.h 58 2007-04-22 04:40:57Z Pecan $

#ifndef MENUIDENTIFIERS_H_INCLUDED
#define MENUIDENTIFIERS_H_INCLUDED

// Applications Main Menu ids
extern int idMenuFileOpen       ;
extern int idMenuFileSaveAs     ;
extern int idMenuSettingsOptions;
//-extern int idMnuOpenAsFileLink      ;
extern int idMnuConvertToFileLink      ;


//#if defined(__WXMSW__) && defined(BUILDING_PLUGIN)
 //NB: linux makefile does not define BUILDING_PLUGIN
extern int idViewSnippets ;
//#endif

#if defined(__WXMSW__) && !defined(BUILDING_PLUGIN)
 //NB: linux makefile does not define BUILDING_PLUGIN
extern int idMenuProperties    ;
#endif

// Search control menu ids
extern int idSearchSnippetCtrl ;
extern int idSearchCfgBtn      ;
extern int idSnippetsTreeCtrl  ;

// Context Menu items
extern int idMnuAddSubCategory ;
extern int idMnuRemove         ;
extern int idMnuConvertToCategory      ;
extern int idMnuAddSnippet     ;
extern int idMnuApplySnippet   ;
extern int idMnuLoadSnippetsFromFile   ;
extern int idMnuSaveSnippets    ;
extern int idMnuSaveSnippetsToFile     ;
extern int idMnuRemoveAll       ;
extern int idMnuCopyToClipboard ;
extern int idMnuEditSnippet     ;
extern int idMnuConvertToFileLink      ;
extern int idMnuProperties      ;
extern int idMnuSettings        ;


// Search config menu items
extern int idMnuCaseSensitive   ;
extern int idMnuClear           ;
extern int idMnuScope           ;
extern int idMnuScopeSnippets   ;
extern int idMnuScopeCategories ;
extern int idMnuScopeBoth       ;


#endif // MENUIDENTIFIERS_H_INCLUDED
