/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010  Gary Harris
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
*    Icon elements from NUVOLA ICON THEME for KDE 3.x
*    distributed under the terms of the GNU LGPL.
*    Copyright (c)  2003-2004  David Vignoni.
*    http://www.icon-king.com
*/

#include "wxsmediactrl.h"

#if wxUSE_MEDIACTRL

#include "wx/mediactrl.h"


namespace
{
    wxsRegisterItem<wxsMediaCtrl> Reg(_T("MediaCtrl"),wxsTWidget,_T("Standard"),210);

    WXS_EV_BEGIN(wxsMediaCtrlEvents)
        WXS_EVI(EVT_MEDIA_LOADED, wxEVT_MEDIA_LOADED, wxMediaEvent, Loaded)
        WXS_EVI(EVT_MEDIA_PLAY, wxEVT_MEDIA_PLAY, wxMediaEvent, Play)
        WXS_EVI(EVT_MEDIA_PAUSE, wxEVT_MEDIA_PAUSE, wxMediaEvent, Pause)
        WXS_EVI(EVT_MEDIA_STATECHANGED, wxEVT_MEDIA_STATECHANGED, wxMediaEvent, StateChanged)
        WXS_EVI(EVT_MEDIA_STOP, wxEVT_MEDIA_STOP, wxMediaEvent, Stop)
        WXS_EVI(EVT_MEDIA_FINISHED, wxEVT_MEDIA_FINISHED, wxMediaEvent, Finished)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsMediaCtrl::wxsMediaCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsMediaCtrlEvents,
        NULL),
        m_sMediaFile(wxEmptyString),
        m_sProxy(wxEmptyString),
        m_iControls(wxMEDIACTRLPLAYERCONTROLS_DEFAULT),
        m_iVolume(5)
//        m_sBackend(wxEmptyString)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsMediaCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/mediactrl.h>"),GetInfo().ClassName,hfInPCH);

            Codef(_T("%C(%W, %I, %s, %P, %S, %T);\n"), wxT("wxEmptyString"));
            #if wxCHECK_VERSION(2, 9, 0)
            if(m_sMediaFile.StartsWith(wxT("http://"))){
                // Online media, possibly with proxy.
                if(!m_sProxy.IsEmpty()){
                    Codef(_T("%ALoadURIWithProxy(%n, %n);\n"), m_sMediaFile.wx_str(), m_sProxy.wx_str());
                }
                else{
                    Codef(_T("%ALoadURI(%n);\n"), m_sMediaFile.wx_str());
                }
            }
            else{
                // Local media file.
                Codef(_T("%ALoad(%n);\n"), m_sMediaFile.wx_str());
            }
            #else
            if(m_sMediaFile.StartsWith(wxT("http://"))){
                // Online media, possibly with proxy.
                if(!m_sProxy.IsEmpty()){
                    Codef(_T("%ALoadURIWithProxy(%n, %n);\n"), m_sMediaFile.c_str(), m_sProxy.c_str());
                }
                else{
                    Codef(_T("%ALoadURI(%n);\n"), m_sMediaFile.c_str());
                }
            }
            else{
                // Local media file.
                Codef(_T("%ALoad(%n);\n"), m_sMediaFile.c_str());
            }
            #endif
            switch(m_iControls){
                case wxMEDIACTRLPLAYERCONTROLS_NONE:
                    Codef(_T("%AShowPlayerControls(%s);\n"), wxT("wxMEDIACTRLPLAYERCONTROLS_NONE"));
                    break;
                case wxMEDIACTRLPLAYERCONTROLS_STEP:
                    Codef(_T("%AShowPlayerControls(%s);\n"), wxT("wxMEDIACTRLPLAYERCONTROLS_STEP"));
                    break;
                case wxMEDIACTRLPLAYERCONTROLS_VOLUME:
                    Codef(_T("%AShowPlayerControls(%s);\n"), wxT("wxMEDIACTRLPLAYERCONTROLS_VOLUME"));
                    break;
                case wxMEDIACTRLPLAYERCONTROLS_DEFAULT:
                    Codef(_T("%AShowPlayerControls(%s);\n"), wxT("wxMEDIACTRLPLAYERCONTROLS_DEFAULT"));
                    break;
            }
            // WXS_ENUM won't store doubles so I store the volume level as an int  = (real value * 10)
            // and jump through hoops to get a double in the output.
            wxString sVol;
            if(m_iVolume == 10){
                sVol = wxT("1.0");
            }
            else{
                sVol = wxString::Format(wxT("0.%d"), m_iVolume);
            }
            Codef(_T("%ASetVolume(%s);\n"), sVol.c_str());

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsMediaCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief    Build the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                The control flags.
 * \return wxObject*                 The constructed control.
 *
 */
wxObject* wxsMediaCtrl::OnBuildPreview(wxWindow* parent,long flags)
{
    wxMediaCtrl* preview = new wxMediaCtrl(parent, GetId(), wxEmptyString, Pos(parent), Size(parent), Style());//, m_sBackend);

    if(m_sMediaFile.StartsWith(wxT("http://"))){
        // Online media, possibly with proxy.
        if(!m_sProxy.IsEmpty()){
            preview->LoadURIWithProxy(m_sMediaFile, m_sProxy);
        }
        else{
            preview->LoadURI(m_sMediaFile);
        }
    }
    else{
        // Local media file.
        preview->Load(m_sMediaFile);
    }
    switch(m_iControls){
        case wxMEDIACTRLPLAYERCONTROLS_NONE:
            preview->ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_NONE);
            break;
        case wxMEDIACTRLPLAYERCONTROLS_STEP:
            preview->ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_STEP);
            break;
        case wxMEDIACTRLPLAYERCONTROLS_VOLUME:
            preview->ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_VOLUME);
            break;
        case wxMEDIACTRLPLAYERCONTROLS_DEFAULT:
            preview->ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_DEFAULT);
            break;
    }
    // WXS_ENUM won't store doubles so I store the volume level as an int  = (real value * 10) and divide by 10 here.
    // Note: If the cast is omitted, the result is always rounded to 0 or 1, I presume by implicit casting to long.
    preview->SetVolume((double)m_iVolume / 10);

    return SetupWindow(preview, flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
  * \todo (Gary#5#): Support backends...possibly.
 */
void wxsMediaCtrl::OnEnumWidgetProperties(long flags)
{
    static const long VolumeLevels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};                                                            //!< Volume levels.
    /*!< Volume level names. */
    static const wxChar* VolumeNames[]  = {wxT("0.0"), wxT("0.1"), wxT("0.2"), wxT("0.3"), wxT("0.4"), wxT("0.5"), wxT("0.6"), wxT("0.7"), wxT("0.8"), wxT("0.9"), wxT("1.0"), NULL};
    static const long ControlStates[] = {wxMEDIACTRLPLAYERCONTROLS_NONE,                                         //!< No controls.
                                                         wxMEDIACTRLPLAYERCONTROLS_STEP,                                         //!< Step controls like fastfoward, step one frame etc.
                                                         wxMEDIACTRLPLAYERCONTROLS_VOLUME,                                     //!< Volume controls like the speaker icon, volume slider, etc.
                                                         wxMEDIACTRLPLAYERCONTROLS_DEFAULT};                                 //!< Default controls for the toolkit.
    static const wxChar* ControlNames[]  = {wxT("None"), wxT("Step"), wxT("Volume"), wxT("Default"), NULL};    //!< Control state names.

    WXS_SHORT_STRING(wxsMediaCtrl, m_sMediaFile, _("Media File"), _T("media_file"), _T(""), false)
    WXS_SHORT_STRING(wxsMediaCtrl, m_sProxy, _("Proxy"), _T("proxy"), _T(""), false)
    WXS_ENUM(wxsMediaCtrl, m_iControls, _("Controls"), _T("controls"), ControlStates, ControlNames, wxMEDIACTRLPLAYERCONTROLS_DEFAULT)
    WXS_ENUM(wxsMediaCtrl, m_iVolume,  _("Volume"), _T("volume"), VolumeLevels, VolumeNames, 5)
}

#endif // wxUSE_MEDIACTRL

