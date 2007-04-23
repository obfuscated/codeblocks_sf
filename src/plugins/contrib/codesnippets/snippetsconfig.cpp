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
// RCS-ID: $Id: snippetsconfig.cpp 59 2007-04-22 19:23:46Z Pecan $
#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif
    #include <wx/fileconf.h>
    #include <wx/stdpaths.h>

#include <sstream>

#include "version.h"
#include "snippetsconfig.h"

    // anchor to global configuration
    CodeSnippetsConfig* g_pConfig = 0;
    CodeSnippetsConfig* GetConfig(){return g_pConfig;}
    int g_activeMenuId = 0;
    int     GetActiveMenuId(){return g_activeMenuId;}
    void    SetActiveMenuId(int menuId){g_activeMenuId = menuId;}

    // ------------------------------------
    // UTF8 conversion routines
    // ------------------------------------
   #if defined(BUILDING_PLUGIN)
     #include "globals.h"  // codeblocks sdk globals.h
   #else
    // Return @c str as a proper unicode-compatible string
    wxString cbC2U(const char* str)
    {
        #if wxUSE_UNICODE
            return wxString(str, wxConvUTF8);
        #else
            return wxString(str);
        #endif
    }

    // Return multibyte (C string) representation of the string
    const wxWX2MBbuf cbU2C(const wxString& str)
    {
        #if wxUSE_UNICODE
            return str.mb_str(wxConvUTF8);
        #else
            return (wxChar*)str.mb_str();
        #endif
    }
   #endif //defined(BUILDING_PLUGIN)

// ----------------------------------------------------------------------------
CodeSnippetsConfig::CodeSnippetsConfig()
// ----------------------------------------------------------------------------
{

    //ctor
    AppVersion SnippetVersion;

    AppName = wxEmptyString;
    pMainFrame = 0;
    pSnippetsWindow = 0;
    pSnippetsTreeCtrl = 0;
	SettingsExternalEditor = wxEmptyString;
	SettingsSnippetsCfgFullPath = wxEmptyString;
	SettingsSnippetsXmlFullPath = wxEmptyString;
	SettingsSearchBox = false;
	m_SearchConfig.caseSensitive = false;
	m_SearchConfig.scope = SCOPE_BOTH;
    pSnipImages = 0;
    nEditDlgWidth = 0;
    nEditDlgHeight = 0;
    bEditDlgMaximized = false;
    windowXpos = 0;
    windowYpos = 0;
    windowWidth = 0;
    windowHeight = 0;
    m_VersionStr = SnippetVersion.GetVersion();
    m_sWindowHandle = wxEmptyString;

}

// ----------------------------------------------------------------------------
CodeSnippetsConfig::~CodeSnippetsConfig()
// ----------------------------------------------------------------------------
{
    //dtor
}

// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsLoad()
// ----------------------------------------------------------------------------
{
        // file will be saved in $HOME/codesnippets.ini

    wxStandardPaths stdPaths;

    #ifdef LOGGING
     wxString fn(__FUNCTION__, wxConvUTF8);
     LOGIT( _T("--- [%s] ---"),fn.c_str() );
     LOGIT(wxT("Loading Settings File[%s]"),SettingsSnippetsCfgFullPath.c_str());
    #endif //LOGGING

    wxFileConfig cfgFile(wxEmptyString,       // appname
                        wxEmptyString,      // vendor
                        SettingsSnippetsCfgFullPath,      // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);
                        //0);
	cfgFile.Read( wxT("ExternalEditor"),  &SettingsExternalEditor, wxEmptyString ) ;
	cfgFile.Read( wxT("SnippetFile"),     &SettingsSnippetsXmlFullPath, wxEmptyString ) ;
	cfgFile.Read( wxT("ViewSearchBox"),   &GetConfig()->SettingsSearchBox, true ) ;
    //	ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
    //	m_SearchConfig.caseSensitive = cfgMan->ReadBool(_T("casesensitive"), true);
    //	m_SearchConfig.scope = SearchScope(cfgMan->ReadInt(_T("scope"), 2));
	cfgFile.Read( wxT("casesensitive"),     &m_SearchConfig.caseSensitive, true ) ;
	int nScope;
	cfgFile.Read( wxT("scope"),             &nScope, SCOPE_BOTH ) ;
    m_SearchConfig.scope = (SearchScope)nScope;

    // read last window position
    wxString winPos;
    cfgFile.Read( wxT("WindowPosition"),  &winPos, wxEmptyString) ;
    if ( not winPos.IsEmpty() )
    {
        const wxWX2MBbuf buf = cbU2C(winPos);
        std::string cstring( buf );
        std::stringstream istream(cstring);
        istream >> windowXpos ;
        istream >> windowYpos ;
        istream >> windowWidth ;
        istream >> windowHeight ;
    }
    else
    {
        windowXpos = 20;
        windowYpos = 20;
        windowWidth = 300;
        windowHeight = int(windowWidth*1.168);
    }
    LOGIT( _T("WindowPosition[%s]"),winPos.c_str() );

    #ifdef LOGGING
     LOGIT( _T("SettingsExternalEditor[%s]"),SettingsExternalEditor.c_str() );
     LOGIT( _T("SettingsSnippetsXmlFullPath[%s]"),SettingsSnippetsXmlFullPath.c_str() );
     LOGIT( _T("SettingsSearchBox[%d]"),SettingsSearchBox );
     LOGIT( _T("caseSensitive[%d]"),m_SearchConfig.caseSensitive );
     LOGIT( _T("SettingsSnippetsXmlFullPath[%s]"),SettingsSnippetsXmlFullPath.c_str() );
    #endif //LOGGING

    // read windowHandle. Will be empty if this is first instance
    cfgFile.Read( wxT("WindowHandle"),  &m_sWindowHandle, wxEmptyString) ;

    // set a global snippets xml file path
    if (SettingsSnippetsXmlFullPath.IsEmpty())
        SettingsSnippetsXmlFullPath = stdPaths.GetUserDataDir() + wxFILE_SEP_PATH +  AppName+ _T(".xml");

}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsSave()
// ----------------------------------------------------------------------------
{
        // file will be saved in $HOME/codesnippets.ini

    #ifdef LOGGING
     wxString fn(__FUNCTION__, wxConvUTF8);
     LOGIT( _T("--- [%s] ---"),fn.c_str() );
     LOGIT(wxT("Saving Settings File[%s]"),SettingsSnippetsCfgFullPath.c_str());
    #endif //LOGGING

    wxFileConfig cfgFile(wxEmptyString, // appname
                        wxEmptyString,      // vendor
                        SettingsSnippetsCfgFullPath,    // local filename
                        wxEmptyString,                  // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Write( wxT("ExternalEditor"),  SettingsExternalEditor ) ;
	cfgFile.Write( wxT("SnippetFile"),     SettingsSnippetsXmlFullPath ) ;
	cfgFile.Write( wxT("ViewSearchBox"),   SettingsSearchBox ) ;
	cfgFile.Write( wxT("casesensitive"),   m_SearchConfig.caseSensitive ) ;
	cfgFile.Write( wxT("scope"),           m_SearchConfig.scope );

    if (IsApplication())
    {   // record the current window position and size
        // here, use the main frame, not the snippets window
        wxWindow* pwin = (wxWindow*)GetMainFrame();;
        int winXposn, winYposn, winWidth, winHeight;
        pwin->GetPosition( &winXposn, &winYposn );
        pwin->GetSize( &winWidth, &winHeight );

        wxString winPos;
        winPos = wxString::Format(wxT("%d %d %d %d"), winXposn,winYposn,winWidth,winHeight);
        cfgFile.Write(wxT("WindowPosition"),  winPos) ;
         LOGIT( _T("Saving WindowPosition[%s]"), winPos.c_str() );
    }
}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsSaveString(const wxString settingName, const wxString settingValue )
// ----------------------------------------------------------------------------
{
        // file will be saved in $HOME/codesnippets.ini

    wxFileConfig cfgFile(wxEmptyString,                 // appname
                        wxEmptyString,                  // vendor
                        SettingsSnippetsCfgFullPath,    // local filename
                        wxEmptyString,                  // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Write( settingName,  settingValue ) ;

}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsSaveWinPosition()
// ----------------------------------------------------------------------------
{
    // This routine was specifically written to solve the placement of floating windows
    // It should only be called on floating windows just before the window is undocked/hidden
    // else the mainframe position will be recorded instead of the floating window.
    wxFileConfig cfgFile(wxEmptyString, // appname
                        wxEmptyString,      // vendor
                        SettingsSnippetsCfgFullPath,    // local filename
                        wxEmptyString,                  // global file
                        wxCONFIG_USE_LOCAL_FILE);

    // record the current window position and size
    // here use the snippets windows' parent, not the main frame
    // Our window was reparented by wxAUI.
    wxWindow* pwin = (wxWindow*)GetSnippetsWindow();
    if (pwin && IsPlugin() && pwin->GetParent() )
        pwin = pwin->GetParent();
    else return;
    int winXposn, winYposn, winWidth, winHeight;
    pwin->GetPosition( &winXposn, &winYposn );
    pwin->GetSize( &winWidth, &winHeight );

    wxString winPos;
    winPos = wxString::Format(wxT("%d %d %d %d"), winXposn,winYposn,winWidth,winHeight);
    cfgFile.Write(wxT("WindowPosition"),  winPos) ;
     LOGIT( _T("SavingWindowPosition[%s]"), winPos.c_str() );

}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::CenterChildOnParent(wxWindow* child)
// ----------------------------------------------------------------------------
{
    //For docked window we cannot get its position. Just move
    // the window over the last known parent position
    if ( GetConfig()->IsPlugin() )
    {   wxPoint parentPosn( GetConfig()->windowXpos+(GetConfig()->windowWidth>>1),
            GetConfig()->windowYpos+(GetConfig()->windowHeight>>1) );
        child->Move( parentPosn.x, parentPosn.y);
        return;
    }

    // If application window
    // Put Top left corner in center of parent (if possible)

    int h; int w;
    int x; int y;
    int displayX; int displayY;
    wxWindow* mainFrame = child->GetParent();
    if (not mainFrame) return;

    // move upper left dialog corner to center of parent
    ::wxDisplaySize(&displayX, &displayY);
    mainFrame->GetPosition(&x, &y );
    mainFrame->ClientToScreen(&x, &y );
    if ((x == 0) || (y == 0))
        if (GetConfig()->pMainFrame)
            GetConfig()->pMainFrame->GetPosition(&x, &y );
    mainFrame->GetClientSize(&w,&h);

    // move child underneath the mouse pointer
    wxPoint movePosn = ::wxGetMousePosition();
    movePosn.x = x+(w>>2);
    movePosn.y = y+(h>>2);

    // Make child is not off the screen
    wxSize size = child->GetSize();
    //-LOGIT( _T("X[%d]Y[%d]width[%d]height[%d]"), x,y,w,h );

    if ( (movePosn.x+size.GetWidth()) > displayX)
        movePosn.x = displayX-size.GetWidth();
    if ( (movePosn.y+size.GetHeight()) > displayY)
        movePosn.y = displayY-size.GetHeight();

    child->Move(movePosn.x, movePosn.y);
    return;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsConfig::IsDockedWindow(wxWindow** pWindowRequest, wxPoint* pCoordRequest, wxSize* pSizeRequest)
// ----------------------------------------------------------------------------
{
    // If window is Docked window, return its info

    if ( IsApplication() ) {return false;}

    wxWindow* pwFocused = wxWindow::FindFocus();
    wxWindow* pmf = GetConfig()->GetMainFrame();
    if ( not pwFocused ) {pwFocused = pmf;}

     //LOGIT( _T("IsDocked pmf[%p]pFocused[%p]"),pmf,pwFocused );
     //LOGIT( _T("IsDocked pmf[%s]pFocused[%s]"),pmf->GetName().c_str(),pwFocused->GetName().c_str() );

    // if focused parent window != MainFrame, then not docked, it's floating
    if (pwFocused->GetParent()) pwFocused = pwFocused->GetParent();
    //LOGIT( _T("IsDocked parent[%p] named[%s]"),pwFocused,pwFocused->GetName().c_str() );
    if (pwFocused->GetParent()) pwFocused = pwFocused->GetParent();
    //LOGIT( _T("IsDocked GrandParent[%p] named[%s]"),pwFocused,pwFocused->GetName().c_str() );

    if ( pmf != pwFocused ) {return false;}
    // Have docked window
    if (pWindowRequest) *pWindowRequest = pmf;
    if (pCoordRequest)
    {   *pCoordRequest = pmf->GetPosition();
         if ( *pCoordRequest == wxPoint(0,0 ) )
           pmf->ClientToScreen(&pCoordRequest->x, &pCoordRequest->y);
    }
    if (pSizeRequest) *pSizeRequest = pmf->GetSize();
    return true;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsConfig::IsFloatingWindow(wxWindow** pWindowRequest, wxPoint* pCoordRequest, wxSize* pSizeRequest)
// ----------------------------------------------------------------------------
{
    // If window is Docked window, return its info
    if ( IsApplication() ) {return false;}

    wxWindow* pwFocused = wxWindow::FindFocus();
    wxWindow* pmf = GetConfig()->GetMainFrame();
    if ( not pwFocused ) {pwFocused = pmf;}
     //LOGIT( _T("IsFloating pmf[%p]pFocused[%p]"),pmf,pwFocused );
     //LOGIT( _T("IsFloating pmf[%s]pFocused[%s]"),pmf->GetName().c_str(),pwFocused->GetName().c_str() );

    // Floating windows have different parents than their original MainFrame
    // if focused parent window == MainFrame, then docked, not floating
    if (pwFocused->GetParent()) pwFocused = pwFocused->GetParent();
    //LOGIT( _T("IsFloating parent[%p] named[%s]"),pwFocused,pwFocused->GetName().c_str() );
    if (pwFocused->GetParent()) pwFocused = pwFocused->GetParent();
    //LOGIT( _T("IsFloating GrandParent[%p] named[%s]"),pwFocused,pwFocused->GetName().c_str() );

    if ( pmf == pwFocused ) {return false;}
    LOGIT( _T("IsFloating New pmf[%p]pFocused[%p]"),pmf,pwFocused );

    // Have floating window, point to reparented focused window
    pmf = pwFocused;
    if (pWindowRequest) *pWindowRequest = pmf;
    if (pCoordRequest)
    {   *pCoordRequest = pmf->GetPosition();
         if ( *pCoordRequest == wxPoint(0,0 ) )
           pmf->ClientToScreen(&pCoordRequest->x, &pCoordRequest->y);
    }
    if (pSizeRequest) *pSizeRequest = pmf->GetSize();
    return true;
}
// ----------------------------------------------------------------------------
