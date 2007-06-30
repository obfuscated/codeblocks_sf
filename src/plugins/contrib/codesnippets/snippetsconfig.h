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
// RCS-ID: $Id: snippetsconfig.h 93 2007-06-30 21:22:19Z Pecan $


#ifndef GLOBAL_H
#define GLOBAL_H

#include <wx/frame.h>
#include <wx/fileconf.h>

#include "snippetsimages.h"

    class CodeSnippetsConfig;
    class CodeSnippetsTreeCtrl;
    class CodeSnippetsWindow;

    extern CodeSnippetsConfig* g_pConfig;
    CodeSnippetsConfig* GetConfig();

    extern int g_activeMenuId;
    int     GetActiveMenuId();
    void    SetActiveMenuId(int menuid);

    wxString csC2U(const char* str);
    const wxWX2MBbuf csU2C(const wxString& str);
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
    void SettingsSaveString(const wxString settingName, const wxString settingValue );
    wxString SettingsReadString(const wxString settingName );
    wxString GetSettingsWindowState();

    wxWindow*       GetMainFrame(){return pMainFrame;}
    wxMenuBar*      GetMenuBar(){return m_pMenuBar;}
    SnipImages*     GetSnipImages(){return pSnipImages;}

    CodeSnippetsWindow*     GetSnippetsWindow(){return pSnippetsWindow;}
    wxTextCtrl*             GetSnippetsSearchCtrl(){return pSnippetsSearchCtrl;}
    CodeSnippetsTreeCtrl*   GetSnippetsTreeCtrl(){return pSnippetsTreeCtrl;}
    void                    SetSnippetsTreeCtrl(CodeSnippetsTreeCtrl* p){ pSnippetsTreeCtrl=p;return;}
    wxString                GetVersion(){return m_VersionStr;}
    wxFileConfig*           GetCfgFile(){return m_pCfgFile;}
    wxString                GetTempDir();


    void CenterChildOnParent(wxWindow* child);

    bool IsPlugin(){return m_bIsPlugin;}
    bool IsApplication(){return not m_bIsPlugin;}

    bool  IsDockedWindow(wxWindow** pw = 0, wxPoint* pcoord = 0, wxSize* psize = 0);
    bool  IsFloatingWindow(wxWindow** pw = 0, wxPoint* pcoord = 0, wxSize* psize = 0);
    bool  IsExternalWindow();

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
    wxMenuBar*   m_pMenuBar;
    CodeSnippetsWindow*     pSnippetsWindow;
    CodeSnippetsTreeCtrl*   pSnippetsTreeCtrl;
    wxTextCtrl*             pSnippetsSearchCtrl;
	wxString     SettingsExternalEditor;
	wxString     SettingsSnippetsXmlFullPath;
	wxString     SettingsSnippetsCfgFullPath;
	wxString     SettingsSnippetsFolder;
    // the key file name as {%HOME%}\codesnippets.ini
    wxString m_ConfigFolder;
    // the programs executable folder
    wxString m_ExecuteFolder;

	bool         SettingsSearchBox;
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
    wxString     m_sWindowHandle;
    SearchConfiguration     m_SearchConfig;
    wxFileConfig* m_pCfgFile;
    wxString     SettingsWindowState;
    bool         m_bIsPlugin;

    // Mouse DragScrolling settings
    int         MouseDragSensitivity;
    int         MouseToLineRatio;
    int         MouseContextDelay;

    wxWindow*   m_pEvtCloseConnect;

};

#endif // GLOBAL_H
