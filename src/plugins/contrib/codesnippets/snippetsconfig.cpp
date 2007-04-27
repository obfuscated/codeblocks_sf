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
// RCS-ID: $Id: snippetsconfig.cpp 68 2007-04-27 21:08:11Z Pecan $
#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif
    #include <wx/fileconf.h>
    #include <wx/stdpaths.h>
    #include <wx/app.h>

#include <sstream>

#include "manager.h"
#include "version.h"
#include "snippetsconfig.h"

    // anchor to global configuration
    CodeSnippetsConfig* g_pConfig = 0;
    CodeSnippetsConfig* GetConfig(){return g_pConfig;}
    int g_activeMenuId = 0;
    int     GetActiveMenuId(){return g_activeMenuId;}
    void    SetActiveMenuId(int menuId)
        {   g_activeMenuId = menuId;
        }

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
    m_pCfgFile = 0;
	SettingsExternalEditor = wxEmptyString;
	SettingsSnippetsCfgFullPath = wxEmptyString;
	SettingsSnippetsXmlFullPath = wxEmptyString;
	SettingsSnippetsFolder = wxEmptyString;
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
    SettingsWindowState = wxT("Floating");
    m_pEvtCloseConnect = 0;
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

    //    wxFileConfig cfgFile(wxEmptyString,       // appname
    //                        wxEmptyString,      // vendor
    //                        SettingsSnippetsCfgFullPath,      // local filename
    //                        wxEmptyString,      // global file
    //                        wxCONFIG_USE_LOCAL_FILE);
    //                        //0);
    if (m_pCfgFile)
    {   // if we dont do this, we only get in-memory cache
        // and we want the new physical file data.
         delete(m_pCfgFile);
         m_pCfgFile = 0;
    }
    if (not m_pCfgFile)
        m_pCfgFile = new wxFileConfig(
                    wxEmptyString,              // appname
                    wxEmptyString,              // vendor
                    SettingsSnippetsCfgFullPath,// local filename
                    wxEmptyString,              // global file
                    wxCONFIG_USE_LOCAL_FILE);
                    //0);
    wxFileConfig& cfgFile = *GetCfgFile();

	cfgFile.Read( wxT("ExternalEditor"),  &SettingsExternalEditor, wxEmptyString ) ;
	cfgFile.Read( wxT("SnippetFile"),     &SettingsSnippetsXmlFullPath, wxEmptyString ) ;
	cfgFile.Read( wxT("SnippetFolder"),   &SettingsSnippetsFolder, wxEmptyString ) ;
	cfgFile.Read( wxT("ViewSearchBox"),   &GetConfig()->SettingsSearchBox, true ) ;
	cfgFile.Read( wxT("casesensitive"),     &m_SearchConfig.caseSensitive, true ) ;
	int nScope;
	cfgFile.Read( wxT("scope"),             &nScope, SCOPE_BOTH ) ;
    m_SearchConfig.scope = (SearchScope)nScope;

    // read mouse DragScrolling settings
    //cfgFile.Read(_T("MouseDragScrollEnabled"),  &MouseDragScrollEnabled ) ;
	//cfgFile.Read(_T("MouseEditorFocusEnabled"), &MouseEditorFocusEnabled ) ;
	//cfgFile.Read(_T("MouseFocusEnabled"),       &MouseFocusEnabled ) ;
	//cfgFile.Read(_T("MouseDragDirection"),      &MouseDragDirection ) ;
	//cfgFile.Read(_T("MouseDragKey"),            &MouseDragKey ) ;
	cfgFile.Read(_T("MouseDragSensitivity"),    &MouseDragSensitivity, 8 ) ;
	cfgFile.Read(_T("MouseToLineRatio"),        &MouseToLineRatio, 30 ) ;
    cfgFile.Read(_T("MouseContextDelay"),       &MouseContextDelay, 192 );

    // read user specified window state (External, Floating, or Docked)
    cfgFile.Read( wxT("WindowState"), &SettingsWindowState, wxT("Floating") );
     LOGIT( _T("WindowState[%s]"), SettingsWindowState.c_str() );

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
     LOGIT( _T("SettingsSnippetsFolder[%s]"),SettingsSnippetsFolder.c_str() );
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

//    wxFileConfig cfgFile(wxEmptyString, // appname
//                        wxEmptyString,      // vendor
//                        SettingsSnippetsCfgFullPath,    // local filename
//                        wxEmptyString,                  // global file
//                        wxCONFIG_USE_LOCAL_FILE);

    wxFileConfig& cfgFile = *GetCfgFile();

	cfgFile.Write( wxT("ExternalEditor"),  SettingsExternalEditor ) ;
	cfgFile.Write( wxT("SnippetFile"),     SettingsSnippetsXmlFullPath ) ;
	cfgFile.Write( wxT("SnippetFolder"),   SettingsSnippetsFolder ) ;
	cfgFile.Write( wxT("ViewSearchBox"),   SettingsSearchBox ) ;
	cfgFile.Write( wxT("casesensitive"),   m_SearchConfig.caseSensitive ) ;
	cfgFile.Write( wxT("scope"),           m_SearchConfig.scope );

    // Write Mouse DragScrolling settings
    cfgFile.Write( wxT("MouseDragSensitivity"),  MouseDragSensitivity ) ;
    cfgFile.Write( wxT("MouseToLineRatio"),      MouseToLineRatio );
    cfgFile.Write( wxT("MouseContextDelay"),     MouseContextDelay );

    //wxString lastWindowState = wxT("External");
    //if ( IsFloatingWindow() ) {lastWindowState = wxT("Floating");}
    //if ( IsDockedWindow() ) {lastWindowState = wxT("Docked");}
     LOGIT( _T("WindowState[%s]"), SettingsWindowState.c_str() );
    cfgFile.Write( wxT("WindowState"), SettingsWindowState );

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
    cfgFile.Flush();
}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsSaveString(const wxString settingName, const wxString settingValue )
// ----------------------------------------------------------------------------
{
        // file will be saved in $HOME/codesnippets.ini
        // we should probably enable the following definition. Without t
        // ALL entries are re-written to the conf, overwritting any put there
        // by an external pgm.

//    wxFileConfig cfgFile(wxEmptyString,                 // appname
//                        wxEmptyString,                  // vendor
//                        SettingsSnippetsCfgFullPath,    // local filename
//                        wxEmptyString,                  // global file
//                        wxCONFIG_USE_LOCAL_FILE);
    wxFileConfig& cfgFile = *GetCfgFile();

	cfgFile.Write( settingName,  settingValue ) ;
	cfgFile.Flush();

}
// ----------------------------------------------------------------------------
wxString CodeSnippetsConfig::SettingsReadString(const wxString settingName )
// ----------------------------------------------------------------------------
{
    // fetch .ini string value for caller
    // We have to force a re-read of the conf file because an external
    // pgm may have reset the desired window state. A local cfgFile does that.

    wxFileConfig cfgFile(wxEmptyString,                 // appname
                         wxEmptyString,      // vendor
                         GetConfig()->SettingsSnippetsCfgFullPath, // local filename
                         wxEmptyString,      // global file
                         wxCONFIG_USE_LOCAL_FILE);

    wxString result;
	cfgFile.Read( settingName,  &result, wxEmptyString ) ;
	return result;
}
// ----------------------------------------------------------------------------
wxString CodeSnippetsConfig::GetSettingsWindowState()
// ----------------------------------------------------------------------------
{
    SettingsWindowState = SettingsReadString(wxT("WindowState"));
    LOGIT( _T("GetSettingsWindowState[%s]"),GetConfig()->SettingsWindowState.c_str() );
    return GetConfig()->SettingsWindowState;
}

// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsSaveWinPosition()
// ----------------------------------------------------------------------------
{
    // This routine was specifically written to solve the placement of floating windows
    // It should only be called on floating windows just before the window is undocked/hidden
    // else the mainframe position will be recorded instead of the floating window.

//    wxFileConfig cfgFile(wxEmptyString, // appname
//                        wxEmptyString,      // vendor
//                        SettingsSnippetsCfgFullPath,    // local filename
//                        wxEmptyString,                  // global file
//                        wxCONFIG_USE_LOCAL_FILE);
    wxFileConfig& cfgFile = *GetCfgFile();

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
    cfgFile.Flush();

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

    // Get Snippets top window
    wxWindow* pwSnippet = (wxWindow*)GetSnippetsWindow();
    if ( not pwSnippet ) return false;

    while (pwSnippet->GetParent())
    {
        pwSnippet = pwSnippet->GetParent();
        //LOGIT( _T("IsDocked[%p][%s]"), pwSnippet, pwSnippet->GetName().c_str() );
        if (pwSnippet->GetName() == wxT("frame")) break;
    }
    // Get CodeBlocks main window
    wxWindow* pwCodeBlocks = wxTheApp->GetTopWindow();
     //LOGIT( _T("IsDockedpwSnippet[%p]pwCodeBlocks[%p]"),pwSnippet,pwCodeBlocks );
     //LOGIT( _T("IsDocked pwSnippet[%s]pwCodeBlocks[%s]"),pwSnippet->GetName().c_str(),pwCodeBlocks->GetName().c_str() );

    // Floating windows have different parents than their original MainFrame
    // if parent window != CodeBlocks MainFrame, then Floating, not docked
    if ( pwSnippet != pwCodeBlocks )
    {
        //LOGIT( _T("IsDocked[%s]"),wxT("false"));
        return false;
    }

    // Have Docked window, point to window
    wxWindow* pmf = pwSnippet;
    if (pWindowRequest) *pWindowRequest = pmf;
    if (pCoordRequest)
    {   *pCoordRequest = pmf->GetPosition();
         if ( *pCoordRequest == wxPoint(0,0 ) )
           pmf->ClientToScreen(&pCoordRequest->x, &pCoordRequest->y);
    }
    if (pSizeRequest) *pSizeRequest = pmf->GetSize();
    //LOGIT( _T("IsDocked[%s]"),wxT("true"));
    return true;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsConfig::IsFloatingWindow(wxWindow** pWindowRequest, wxPoint* pCoordRequest, wxSize* pSizeRequest)
// ----------------------------------------------------------------------------
{
    // If window is Floating window, return its info

    if ( IsApplication() ) {return false;}

    // Get Snippets top window
    wxWindow* pwSnippet = (wxWindow*)GetSnippetsWindow();
    if ( not pwSnippet ) return false;

    //LOGIT( _T("IsFloating[%p][%s]"), pwSnippet, pwSnippet->GetName().c_str() );
    while (pwSnippet->GetParent())
    {
        pwSnippet = pwSnippet->GetParent();
        LOGIT( _T("IsFloating[%p][%s]"), pwSnippet, pwSnippet->GetName().c_str() );
        if (pwSnippet->GetName() == wxT("frame")) break;
    }
    // Get CodeBlocks main window
    wxWindow* pwCodeBlocks = Manager::Get()->GetAppWindow();
     //LOGIT( _T("IsFloating pwSnippet[%p]pwCodeBlocks[%p]"),pwSnippet,pwCodeBlocks );
     //LOGIT( _T("IsFloating pwSnippet[%s]pwCodeBlocks[%s]"),pwSnippet->GetName().c_str(),pwCodeBlocks->GetName().c_str() );

    // Floating windows have different parents than their original MainFrame
    // if parent window == CodeBlocks MainFrame, then floating, not docked
    if ( pwSnippet == pwCodeBlocks )
    {
        //LOGIT( _T("IsFloating[%s]"),wxT("false"));
        return false;
    }

    // Have floating window, point to reparented focused window
    wxWindow* pmf = pwSnippet;
    if (pWindowRequest) *pWindowRequest = pmf;
    if (pCoordRequest)
    {   *pCoordRequest = pmf->GetPosition();
         if ( *pCoordRequest == wxPoint(0,0 ) )
           pmf->ClientToScreen(&pCoordRequest->x, &pCoordRequest->y);
    }
    if (pSizeRequest) *pSizeRequest = pmf->GetSize();
    //LOGIT( _T("IsFloating[%s]"),wxT("true"));
    return true;
}
