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
#ifdef WX_PRECOMP
    #include "wx_pch.h"
#endif

    #include "pluginmanager.h"

    #include <wx/fileconf.h>
    #include <wx/stdpaths.h>
    #include <wx/app.h>
    #include <wx/filename.h>

#include <sstream>

#include "manager.h"
#include "version.h"
#include "snippetsconfig.h"
#include "editormanager.h"

    // Rreferenced in snippetproperty.cpp
    // So each has an extern to the definition here.
    //-int idMenuProperties = wxNewId();

    // anchor to global configuration
    CodeSnippetsConfig* g_pConfig = 0;
    CodeSnippetsConfig* GetConfig(){return g_pConfig;}
    void SetConfig( CodeSnippetsConfig* p) {g_pConfig = p;}

    int     g_activeMenuId = 0;
    int     GetActiveMenuId(){return g_activeMenuId;}
    void    SetActiveMenuId(int menuId) { g_activeMenuId = menuId; }

    // ------------------------------------
    // UTF8 conversion routines
    // ------------------------------------
    // Return @c str as a proper unicode-compatible string
    wxString csC2U(const char* str)
    {
        #if wxUSE_UNICODE
            return wxString(str, wxConvUTF8);
        #else
            return wxString(str);
        #endif
    }

    // Return multibyte (C string) representation of the string
    const wxWX2MBbuf csU2C(const wxString& str)
    {
        #if wxUSE_UNICODE
            return str.mb_str(wxConvUTF8);
        #else
            return (wxChar*)str.mb_str();
        #endif
    }

// ----------------------------------------------------------------------------
CodeSnippetsConfig::CodeSnippetsConfig()
// ----------------------------------------------------------------------------
{

    //ctor
    AppVersion SnippetVersion;

    AppName = wxEmptyString;
    pMainFrame = 0;
    m_pMenuBar = 0;
    pSnippetsWindow = 0;
    pSnippetsTreeCtrl = 0;
    pSnippetsSearchCtrl = 0;
    m_bIsPlugin = 0;
	SettingsExternalEditor  = wxEmptyString;
	SettingsSnippetsCfgPath = wxEmptyString;
	SettingsSnippetsXmlPath = wxEmptyString;
	SettingsSnippetsFolder  = wxEmptyString;
	SettingsCBConfigPath    = wxEmptyString;
	SettingsSearchBox = false;
	SettingsEditorsStayOnTop = true;
	SettingsToolTipsOption = true;
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
    //-m_sWindowHandle = wxEmptyString;
    m_SettingsWindowState = wxT("Floating");
    m_bWindowStateChanged = false;
    m_pOpenFilesList = 0;
    m_pThreadSearchPlugin = 0;
    m_pDragScrollPlugin = 0;
    g_lKeepAlivePid = 0;
    m_AppParent = wxEmptyString;

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


    #ifdef LOGGING
     wxString fn(__FUNCTION__, wxConvUTF8);
     LOGIT( _T("--- [%s] ---"),fn.c_str() );
     LOGIT(wxT("Loading Settings File[%s]"),SettingsSnippetsCfgPath.c_str());
    #endif //LOGGING

    wxFileConfig cfgFile(
                    wxEmptyString,              // appname
                    wxEmptyString,              // vendor
                    SettingsSnippetsCfgPath,// local filename
                    wxEmptyString,              // global file
                    wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Read( wxT("ExternalEditor"),  &SettingsExternalEditor, wxEmptyString ) ;
	cfgFile.Read( wxT("SnippetFile"),     &SettingsSnippetsXmlPath, wxEmptyString ) ;
	cfgFile.Read( wxT("SnippetFolder"),   &SettingsSnippetsFolder, wxEmptyString ) ;
	cfgFile.Read( wxT("ViewSearchBox"),   &GetConfig()->SettingsSearchBox, true ) ;
	cfgFile.Read( wxT("casesensitive"),   &m_SearchConfig.caseSensitive, true ) ;
	int nScope;
	cfgFile.Read( wxT("scope"),             &nScope, SCOPE_BOTH ) ;
    m_SearchConfig.scope = (SearchScope)nScope;

    // read Editors Stay-On-Top of main window option
    cfgFile.Read( _T("EditorsStayOnTop"), &SettingsEditorsStayOnTop, true);
    // read Editors ToolTips option
    cfgFile.Read( _T("ToolTipsOption"), &SettingsToolTipsOption, true);

    // Read External App state. Launched App will see it as false because
    // plugin has not set it true yet, so if this is launched App, set it true
	cfgFile.Read( wxT("ExternalPersistentOpen"), &m_IsExternalPersistentOpen, false ) ;
	if ( IsApplication() ) SetExternalPersistentOpen(true);

    // read user specified window state (External, Floating, or Docked)
    cfgFile.Read( wxT("WindowState"), &m_SettingsWindowState, wxT("Floating") );
    #if defined(LOGGING)
     LOGIT( _T("WindowState[%s]"), GetSettingsWindowState().c_str() );
    #endif
    // read last window position
    wxString winPos;
    cfgFile.Read( wxT("WindowPosition"),  &winPos, wxEmptyString) ;
    if ( not winPos.IsEmpty() )
    {
        const wxWX2MBbuf buf = csU2C(winPos);
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
     LOGIT( _T("SettingsExternalEditor[%s]"), SettingsExternalEditor.c_str() );
     LOGIT( _T("SettingsSnippetsXmlPath[%s]"),SettingsSnippetsXmlPath.c_str() );
     LOGIT( _T("SettingsSnippetsFolder[%s]"), SettingsSnippetsFolder.c_str() );
     LOGIT( _T("SettingsSearchBox[%d]"),      SettingsSearchBox );
     LOGIT( _T("caseSensitive[%d]"),            m_SearchConfig.caseSensitive );
     LOGIT( _T("SettingsSnippetsXmlPath[%s]"),  SettingsSnippetsXmlPath.c_str() );
     LOGIT( _T("SettingsEditorsStayOnTop[%s]"), SettingsEditorsStayOnTop?_T("True"):_T("False") );
     LOGIT( _T("SettingsToolTipsOption[%s]"),   SettingsToolTipsOption?_T("True"):_T("False") );
     LOGIT( _T("ExternalPersistentOpen[%s]"),   IsExternalPersistentOpen()?_T("True"):_T("False") );
    #endif //LOGGING

////    // read windowHandle of last or current pgm instance that ran
////    // CodeSnippetsApp will check if it's actually running
////    if ( cfgFile.Exists( wxT("WindowHandle")))
////        cfgFile.Read( wxT("WindowHandle"),  &m_sWindowHandle, wxEmptyString) ;

    // set a global snippets xml file path
    wxFileName SettingsFullPath(SettingsSnippetsCfgPath);
    if (SettingsSnippetsXmlPath.IsEmpty())
        SettingsSnippetsXmlPath = SettingsFullPath.GetPath() + wxFILE_SEP_PATH +  AppName + _T(".xml");

}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SettingsSave()
// ----------------------------------------------------------------------------
{
        // file will be saved in $HOME/codesnippets.ini

    #ifdef LOGGING
     wxString fn(__FUNCTION__, wxConvUTF8);
     LOGIT( _T("--- [%s] ---"),fn.c_str() );
     LOGIT(wxT("Saving Settings File[%s]"),SettingsSnippetsCfgPath.c_str());
    #endif //LOGGING

    wxFileConfig cfgFile(wxEmptyString, // appname
                        wxEmptyString,      // vendor
                        SettingsSnippetsCfgPath,    // local filename
                        wxEmptyString,                  // global file
                        wxCONFIG_USE_LOCAL_FILE);

    //-wxFileConfig& cfgFile = *GetCfgFile();

	cfgFile.Write( wxT("ExternalEditor"),  SettingsExternalEditor ) ;
	cfgFile.Write( wxT("SnippetFile"),     SettingsSnippetsXmlPath ) ;
	cfgFile.Write( wxT("SnippetFolder"),   SettingsSnippetsFolder ) ;
	cfgFile.Write( wxT("ViewSearchBox"),   SettingsSearchBox ) ;
	cfgFile.Write( wxT("casesensitive"),   m_SearchConfig.caseSensitive ) ;
	cfgFile.Write( wxT("scope"),           int(m_SearchConfig.scope) );
	cfgFile.Write( wxT("EditorsStayOnTop"),SettingsEditorsStayOnTop );
	cfgFile.Write( wxT("ToolTipsOption"),  SettingsToolTipsOption );
	if ( IsPlugin() )
	{   // Write ExternalPersistent for plugin use only
        cfgFile.Write( wxT("ExternalPersistentOpen"),IsExternalPersistentOpen() );
        #if defined(LOGGING)
        //LOGIT( _T("ExternalPersistentOpen is[%s]"), IsExternalPersistentOpen()?_T("true"):_T("false"));
        #endif
	}

    //wxString lastWindowState = wxT("External");
    //if ( IsFloatingWindow() ) {lastWindowState = wxT("Floating");}
    //if ( IsDockedWindow() ) {lastWindowState = wxT("Docked");}
    #if defined(LOGGING)
     LOGIT( _T("WindowState[%s]"), m_SettingsWindowState.c_str() );
    #endif
    cfgFile.Write( wxT("WindowState"), m_SettingsWindowState );

    if (IsApplication())
        if ( GetMainFrame() && GetMainFrame()->IsShown() )
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

    wxFileConfig cfgFile(wxEmptyString,                 // appname
                        wxEmptyString,                  // vendor
                        SettingsSnippetsCfgPath,    // local filename
                        wxEmptyString,                  // global file
                        wxCONFIG_USE_LOCAL_FILE);

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
                         GetConfig()->SettingsSnippetsCfgPath, // local filename
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
    m_SettingsWindowState = SettingsReadString(wxT("WindowState"));
    //LOGIT( _T("GetSettingsWindowState[%s]"),GetConfig()->SettingsWindowState.c_str() );
    return m_SettingsWindowState;
}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::SetSettingsWindowState(const wxString windowState)
// ----------------------------------------------------------------------------
{
    m_SettingsWindowState = windowState;
    SettingsSaveString( _T("WindowState"), m_SettingsWindowState );
    //LOGIT( _T("SetSettingsWindowState[%s]"),GetSettingsWindowState.c_str() );
    return ;
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
                        SettingsSnippetsCfgPath,    // local filename
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
    cfgFile.Flush();

}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::CenterChildOnParent(wxWindow* child, wxWindow* parentParam )
// ----------------------------------------------------------------------------
{
    //For docked window we cannot get its position. Just move
    // the window over the last known parent position

    int displayX; int displayY;
    ::wxDisplaySize(&displayX, &displayY);

    //(pecan 2008/5/04)
    // Now using Plugin code for both app and plugin
    //if ( GetConfig()->IsPlugin() ) // //(pecan 2008/5/04)
    {
        wxPoint parentPosn( GetConfig()->windowXpos+(GetConfig()->windowWidth>>1),
            GetConfig()->windowYpos+(GetConfig()->windowHeight>>1) );
        do {   // place bottomLeft child at bottomLeft of parent window
            int parentx,parenty;
            int parentsizex,parentsizey;
            int childsizex,childsizey;
            wxWindow* parent = child->GetParent();
            if (parentParam) parent = parentParam;
            if (not parent) break;
            parent->GetScreenPosition(&parentx,&parenty);
            parent->GetSize(&parentsizex,&parentsizey);
            child->GetSize(&childsizex,&childsizey);
            parentPosn.x = parentx+20;
            parentPosn.y = parenty+parentsizey-childsizey;
            // Make sure child is not off right/bottom of screen
            if ( (parentPosn.x+childsizex) > displayX)
                parentPosn.x = displayX-childsizex;
            if ( (parentPosn.y+childsizey) > displayY)
                parentPosn.y = displayY-childsizey;
            // Make sure child is not off left/top of screen
            if ( parentPosn.x < 1) parentPosn.x = 1;
            if ( parentPosn.y < 1) parentPosn.y = 1;
        }while(false);
        child->Move( parentPosn.x, parentPosn.y);
        return;
    }

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
////#include "../utils/ToolBox/ToolBox.h"
// ----------------------------------------------------------------------------
bool CodeSnippetsConfig::IsFloatingWindow(wxWindow** pWindowRequest, wxPoint* pCoordRequest, wxSize* pSizeRequest)
// ----------------------------------------------------------------------------
{
    //-ToolBox toolbox; debugging
    //-toolbox.ShowWindowsAndEvtHandlers(); debugging

    // If window is Floating window, return its info

    if ( IsApplication() ) {return false;}

    // Get Snippets top window
    wxWindow* pwSnippet = (wxWindow*)GetSnippetsWindow();
    if ( not pwSnippet ) return false;

    while (pwSnippet->GetParent())
    {
        pwSnippet = pwSnippet->GetParent();
        #if defined(LOGGING)
        //-LOGIT( _T("IsFloating Parent[%p][%s]"), pwSnippet, pwSnippet->GetName().c_str() );
        #endif
        // break out after we get a pointer to any parent frame
        if (pwSnippet->GetName() == wxT("frame")) break;
    }
    // Get CodeBlocks main window
    //wxWindow* pwCodeBlocks = Manager::Get()->GetAppWindow();
    wxWindow* pwCodeBlocks = GetConfig()->GetMainFrame();
     //LOGIT( _T("IsFloating pwSnippet[%p]pwCodeBlocks[%p]"),pwSnippet,pwCodeBlocks );
     //LOGIT( _T("IsFloating pwSnippet[%s]pwCodeBlocks[%s]"),pwSnippet->GetName().c_str(),pwCodeBlocks->GetName().c_str() );

    // Floating windows have different parents than their original MainFrame
    // if parent window == CodeBlocks MainFrame, then docked
    if ( pwSnippet == pwCodeBlocks )
    {
        #if defined(LOGGING)
        //LOGIT( _T("IsFloating[%s]"),wxT("false"));
        #endif
        return false;
    }

    #if defined(LOGGING)
    //LOGIT( _T("IsFloating[%s]"),wxT("true"));
    #endif

    // Have floating window, point to reparented focused window
    wxWindow* pmf = pwSnippet;
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
bool CodeSnippetsConfig::IsExternalWindow()
// ----------------------------------------------------------------------------
{
  	if ( GetConfig()->GetSettingsWindowState().Contains(wxT("External")) )
        return true;
    return false;
}
// ----------------------------------------------------------------------------
wxString CodeSnippetsConfig::GetTempDir()
// ----------------------------------------------------------------------------
{
    #if wxCHECK_VERSION(2, 8, 0)
        return wxFileName::GetTempDir();
    #else
        wxString tempFile = wxFileName::CreateTempFileName(wxEmptyString);
        wxString temp_folder = wxFileName(tempFile).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        ::wxRemoveFile(tempFile);
        return temp_folder;
    #endif
}
// ----------------------------------------------------------------------------
wxFrame* CodeSnippetsConfig::GetEditorManagerFrame(const int index)
// ----------------------------------------------------------------------------
{
    wxFrame* frame;
    if ( (index < 0) or (index > GetEditorManagerCount()) )
        return 0;
    // iterate over all the elements in the class
    EdManagerMapArray::iterator it;
    int i = 0;
    for( it = m_EdManagerMapArray.begin(); it != m_EdManagerMapArray.end(); ++it )
    {
        if (i == index)
        {
            frame = it->first;
            return frame;
        }
        ++i;
    }
    return 0;
}
// ----------------------------------------------------------------------------
EditorManager* CodeSnippetsConfig::GetEditorManager(const int index)
// ----------------------------------------------------------------------------
{
    EditorManager* edMgr;
    if ( (index < 0) or (index > GetEditorManagerCount()) )
        return 0;
    // iterate over all the elements in the class
    EdManagerMapArray::iterator it;
    int i = 0;
    for( it = m_EdManagerMapArray.begin(); it != m_EdManagerMapArray.end(); ++it )
    {
        if (i == index)
        {
            edMgr = it->second;
            return edMgr;
        }
        ++i;
    }
    return 0;
}
// ----------------------------------------------------------------------------
int CodeSnippetsConfig::GetEditorManagerCount()
// ----------------------------------------------------------------------------
{
    return m_EdManagerMapArray.size();
}
// ----------------------------------------------------------------------------
EditorManager* CodeSnippetsConfig::GetEditorManager(wxWindow* frame)
// ----------------------------------------------------------------------------
{
    return GetEditorManager((wxFrame*) frame);
}
// ----------------------------------------------------------------------------
EditorManager* CodeSnippetsConfig::GetEditorManager(wxFrame* frame)
// ----------------------------------------------------------------------------
{
    // Return the EditorManager belonging to this frame.

    EdManagerMapArray::iterator it = m_EdManagerMapArray.find(frame);
    if (it not_eq m_EdManagerMapArray.end() )
        return it->second;

    wxWindow* p = frame;
    while (p->GetParent())
    {
        p = p->GetParent();
        if ( not p->IsTopLevel() ) continue;
        it =  m_EdManagerMapArray.find(frame);
        if (it not_eq m_EdManagerMapArray.end() )
        {
            #if defined(LOGGING)
            //LOGIT( _T("GetEditorManager frame[%p]Mgr[%p]"), frame, it->second);
            #endif
            return it->second;
        }
    }//while

    return 0;
}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::RegisterEditorManager(wxFrame* frame, EditorManager* edMgr)
// ----------------------------------------------------------------------------
{
    EdManagerMapArray::iterator it = m_EdManagerMapArray.find(frame);
    if ( it == m_EdManagerMapArray.end()  )
        m_EdManagerMapArray[frame] = edMgr;
    #if defined(LOGGING)
    LOGIT( _T("RegisterEditorManager frame[%p]Mgr[%p]"), frame, edMgr);
    #endif
}
// ----------------------------------------------------------------------------
void CodeSnippetsConfig::RemoveEditorManager(wxFrame* frame)
// ----------------------------------------------------------------------------
{
    m_EdManagerMapArray.erase(frame);
}
// ----------------------------------------------------------------------------
wxEvtHandler* CodeSnippetsConfig::GetDragScrollEvtHandler()
// ----------------------------------------------------------------------------
{
    // if app, return resident DragScroll pointer
    if (IsApplication() ) return (wxEvtHandler*)m_pDragScrollPlugin;

    // If this is CodeSnippets plugin, verify that DragScroll still exists
    m_pDragScrollPlugin = (cbDragScroll*)(Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll")));
    if (m_pDragScrollPlugin)
        return (wxEvtHandler*)m_pDragScrollPlugin;

    m_pDragScrollPlugin = (cbDragScroll*)GetMainFrame();
    return (wxEvtHandler*)m_pDragScrollPlugin;
}
// ----------------------------------------------------------------------------
