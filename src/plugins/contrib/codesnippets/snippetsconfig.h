/*
	This file is part of Code Snippets, a plugin for Code::Blocks
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
// RCS-ID: $Id: snippetsconfig.h 43 2007-04-11 19:25:25Z Pecan $


#ifndef GLOBAL_H
#define GLOBAL_H

#include <wx/frame.h>

class CodeSnippetsWindow;
#include "snippetsimages.h"

    class CodeSnippetsConfig;
    class CodeSnippetsTreeCtrl;

    extern CodeSnippetsConfig* g_pConfig;
    CodeSnippetsConfig* GetConfig();

    extern int g_activeMenuId;
    int     GetActiveMenuId();
    void    SetActiveMenuId(int menuid);

   #if !defined(BUILDING_PLUGIN)
    wxString cbC2U(const char* str);
    const wxWX2MBbuf cbU2C(const wxString& str);
   #endif
// ----------------------------------------------------------------------------
class CodeSnippetsConfig
// ----------------------------------------------------------------------------
{
    public:
        CodeSnippetsConfig();
        ~CodeSnippetsConfig();

    void SettingsLoad();
    void SettingsSave();
    void SettingsSaveWinPosition();

    wxWindow*   GetMainFrame(){return pMainFrame;}
    SnipImages* GetSnipImages(){return pSnipImages;}
    CodeSnippetsWindow* GetSnippetsWindow(){return pSnippetsWindow;}
    void        SetSnippetsTreeCtrl(CodeSnippetsTreeCtrl* p){ pSnippetsTreeCtrl=p;return;}
    CodeSnippetsTreeCtrl* GetSnippetsTreeCtrl(){return pSnippetsTreeCtrl;}
    wxString    GetVersion(){return m_VersionStr;}

    void CenterChildOnParent(wxWindow* child);


   #if defined(BUILDING_PLUGIN)
    bool IsPlugin(){return true;}
    bool IsApplication(){return false;}
   #else //not defined(BUILDING_PLUGIN)
    bool IsPlugin(){return false;}
    bool IsApplication(){return true;}
   #endif

    // Snippet Tree Search options
    enum SearchScope
    {
        SCOPE_SNIPPETS,		// Searches only snippets
        SCOPE_CATEGORIES,	// Searches only categories
        SCOPE_BOTH			// Searches both snippets and categories
    };
    struct SearchConfiguration
    {
        bool caseSensitive;
        SearchScope scope;

        SearchConfiguration()
        {
            // Default settings
            caseSensitive = true;
            scope = SCOPE_BOTH;
        }
    };

    wxString     AppName;
    wxWindow*    pMainFrame;
    CodeSnippetsWindow*   pSnippetsWindow;
    CodeSnippetsTreeCtrl* pSnippetsTreeCtrl;
	wxString     SettingsExternalEditor;
	wxString     SettingsSnippetsXmlFullPath;
	wxString     SettingsSnippetsCfgFullPath;
	bool         SettingsSearchBox;
    //	ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
    //	m_SearchConfig.caseSensitive = cfgMan->ReadBool(_T("casesensitive"), true);
    //	m_SearchConfig.scope = SearchScope(cfgMan->ReadInt(_T("scope"), 2));
    SnipImages*  pSnipImages;
    int          nEditDlgXpos;
    int          nEditDlgYpos;
    int          nEditDlgWidth;
    int          nEditDlgHeight;
    bool         bEditDlgMaximized;
    int          windowXpos;
    int          windowYpos;
    int          windowWidth;
    int          windowHeight;
    wxString     m_VersionStr;
    SearchConfiguration     m_SearchConfig;


};

#endif // GLOBAL_H
