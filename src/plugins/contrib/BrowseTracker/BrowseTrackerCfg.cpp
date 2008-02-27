/*
	This file is part of Browse Tracker, a plugin for Code::Blocks
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
// RCS-ID: $Id: BrowseTracker.cpp 22 2007-12-18 14:41:58Z Pecan $

#include <wx/fileconf.h>

#include "BrowseTrackerDefs.h"
#include "BrowseTrackerCfg.h"
#include "ConfigDlg.h"
#include "manager.h"
#include "Version.h"

// ----------------------------------------------------------------------------
BrowseTrackerCfg::BrowseTrackerCfg()
// ----------------------------------------------------------------------------
    :m_configDlg(Manager::Get()->GetAppWindow() )
{
    //ctor
    pCfgFile = 0;
}
// ----------------------------------------------------------------------------
void BrowseTrackerCfg::GetUserOptions(wxString configFullPath)
// ----------------------------------------------------------------------------
{

    ReadUserOptions( configFullPath );

    #if defined(__WXMSW__)
    m_configDlg.m_staticText4->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
    #endif

    // set the current values
    m_configDlg.Cfg_BrowseMarksEnabled->SetValue( m_BrowseMarksEnabled);
    m_configDlg.Cfg_MarkStyle->SetSelection(m_UserMarksStyle);
    m_configDlg.Cfg_ToggleKey->SetSelection( m_ToggleKey );
	m_configDlg.Cfg_LeftMouseDelay->SetValue( m_LeftMouseDelay ) ;
	m_configDlg.Cfg_ClearAllKey->SetSelection( m_ClearAllKey ) ;

    m_configDlg.ShowModal();

    // get any new user values
    m_BrowseMarksEnabled = m_configDlg.Cfg_BrowseMarksEnabled->GetValue();
    m_UserMarksStyle = m_configDlg.Cfg_MarkStyle->GetSelection();
    m_ToggleKey = m_configDlg.Cfg_ToggleKey->GetSelection();
	m_LeftMouseDelay = m_configDlg.Cfg_LeftMouseDelay->GetValue();
	m_ClearAllKey = m_configDlg.Cfg_ClearAllKey->GetSelection();

    // write user options to config file
	SaveUserOptions( configFullPath );

}//Init
// ----------------------------------------------------------------------------
BrowseTrackerCfg::~BrowseTrackerCfg()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
void BrowseTrackerCfg::ReadUserOptions(wxString configFullPath)
// ----------------------------------------------------------------------------
{
    if (not pCfgFile) pCfgFile = new wxFileConfig(
                    wxEmptyString,              // appname
                    wxEmptyString,              // vendor
                    configFullPath,             // local filename
                    wxEmptyString,              // global file
                    wxCONFIG_USE_LOCAL_FILE);
                    //0);
    wxFileConfig& cfgFile = *pCfgFile;

	cfgFile.Read( wxT("BrowseMarksEnabled"),        &m_BrowseMarksEnabled, 0 ) ;
	cfgFile.Read( wxT("BrowseMarksStyle"),          &m_UserMarksStyle, 0 ) ;
	cfgFile.Read( wxT("BrowseMarksToggleKey"),      &m_ToggleKey, Left_Mouse ) ;
	cfgFile.Read( wxT("LeftMouseDelay"),            &m_LeftMouseDelay, 200 ) ;
	cfgFile.Read( wxT("BrowseMarksClearAllMethod"), &m_ClearAllKey, ClearAllOnSingleClick ) ;

}
// ----------------------------------------------------------------------------
void BrowseTrackerCfg::SaveUserOptions(wxString configFullPath)
// ----------------------------------------------------------------------------
{
    if (not pCfgFile) pCfgFile = new wxFileConfig(
                    wxEmptyString,              // appname
                    wxEmptyString,              // vendor
                    configFullPath,             // local filename
                    wxEmptyString,              // global file
                    wxCONFIG_USE_LOCAL_FILE);
                    //0);
    wxFileConfig& cfgFile = *pCfgFile;

	cfgFile.Write( wxT("BrowseMarksEnabled"),       m_BrowseMarksEnabled ) ;
	cfgFile.Write( wxT("BrowseMarksStyle"),         m_UserMarksStyle ) ;
    cfgFile.Write( wxT("BrowseMarksToggleKey"),     m_ToggleKey ) ;
    cfgFile.Write( wxT("LeftMouseDelay"),           m_LeftMouseDelay ) ;
    cfgFile.Write( wxT("BrowseMarksClearAllMethod"),m_ClearAllKey ) ;

    cfgFile.Flush();

}
// ----------------------------------------------------------------------------


