/** \file wxsdialupmanager.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2010 Gary Harris
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <wx/dialup.h>
#include "wxsdialupmanager.h"

namespace
{
    wxsRegisterItem<wxsDialUpManager> Reg(_T("DialUpManager"), wxsTTool, _T("Tools"), 80, false);

    WXS_EV_BEGIN(wxsDialUpManagerEvents)
        WXS_EVI(EVT_DIALUP_CONNECTED, wxEVT_DIALUP_CONNECTED, wxDialUpEvent, Connected)
        WXS_EVI(EVT_DIALUP_DISCONNECTED, wxEVT_DIALUP_DISCONNECTED, wxDialUpEvent, Disconnected)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
 wxsDialUpManager::wxsDialUpManager(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        wxsDialUpManagerEvents,
        NULL,
        flVariable|flId|flSubclass|flExtraCode),
        m_iAutoCheckInterval(60)
#ifndef __WXMSW__
        ,
        m_sWellKnownHost(wxT("www.yahoo.com")),
        m_iPortNo(80),
        m_sDialCommand(wxT("/usr/bin/pon")),
        m_sHangUpCommand(wxT("/usr/bin/poff"))
#endif
{
    // AutoCheckOnlineStatus defaults to ON on Windows and OFF on Linux.
#ifdef __WXMSW__
        m_bAutoCheckOnlineStatus = true;
#else
        m_bAutoCheckOnlineStatus = false;
#endif
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsDialUpManager::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/dialup.h>"),GetInfo().ClassName,hfInPCH);

            Codef(wxT("%O = wxDialUpManager::Create();\n"));

#ifdef __WXMSW__
            // AutoCheckOnlineStatus defaults to ON on Windows and OFF on Linux.
            if(!m_bAutoCheckOnlineStatus){
                Codef(_T("%ADisableAutoCheckOnlineStatus();\n"));
            }
#else
            if(m_bAutoCheckOnlineStatus){
                if(m_iAutoCheckInterval != 60){
                    Codef(_T("%AEnableAutoCheckOnlineStatus(%d);\n"), m_iAutoCheckInterval);
                }
                else{
                    Codef(_T("%AEnableAutoCheckOnlineStatus();\n"));
                }
            }
            // These functions are only used on Unix.
            if(!m_sWellKnownHost.IsSameAs(wxT("www.yahoo.com")) || m_iPortNo != 80){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("%ASetWellKnownHost(%n, %d);\n"), m_sWellKnownHost.wx_str(), m_iPortNo);
                #else
                Codef(_T("%ASetWellKnownHost(%n, %d);\n"), m_sWellKnownHost.c_str(), m_iPortNo);
                #endif
            }

            if(!m_sDialCommand.IsSameAs(wxT("/usr/bin/pon")) || !m_sHangUpCommand.IsSameAs(wxT("/usr/bin/poff"))){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("%ASetConnectCommand(%n);\n"), m_sDialCommand.wx_str(), m_sHangUpCommand.wx_str());
                #else
                Codef(_T("%ASetConnectCommand(%n);\n"), m_sWellKnownHost.c_str(), m_sHangUpCommand.c_str());
                #endif
            }
#endif

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDialUpManager::OnBuildCreatingCode"), GetLanguage());
        }
    }
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsDialUpManager::OnEnumToolProperties(long Flags)
{
    bool bAutoCheck;
    if ((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0)
    {
        bAutoCheck = true;
    }
    else
    {
        bAutoCheck = false;
    }
    WXS_BOOL(wxsDialUpManager, m_bAutoCheckOnlineStatus, _("Auto-check online status"), _T("auto_check_online_status"), bAutoCheck)
#ifndef __WXMSW__
    // These properties are only used on Unix.
    WXS_LONG(wxsDialUpManager,         m_iAutoCheckInterval, _("Auto-check interval (secs)"), wxT("auto_check_interval"), 60)
    WXS_SHORT_STRING(wxsDialUpManager, m_sWellKnownHost,     _("Well Known Host"),            wxT("well_known_host"),     wxT("www.yahoo.com"), false)
    WXS_LONG(wxsDialUpManager,         m_iPortNo,            _("Port no."),                   wxT("port_no"),             80)
    WXS_SHORT_STRING(wxsDialUpManager, m_sDialCommand,       _("Dial command"),               wxT("dial_command"),        wxT("/usr/bin/pon"),  false)
    WXS_SHORT_STRING(wxsDialUpManager, m_sHangUpCommand,     _("Hang up command"),            wxT("hangup_command"),      wxT("/usr/bin/poff"), false)
#endif
}
