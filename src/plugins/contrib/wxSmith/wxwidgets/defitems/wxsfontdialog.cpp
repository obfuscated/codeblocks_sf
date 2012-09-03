/** \file wxsfontdialog.cpp
*
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
*/

#include "wxsfontdialog.h"
#include "../wxsitemresdata.h"
#include <wx/fontdlg.h>

namespace
{
    wxsRegisterItem<wxsFontDialog> Reg(_T("FontDialog"), wxsTTool, _T("Dialogs"), 160, false);
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsFontDialog::wxsFontDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info),
    m_bAllowSymbols(true),
    m_bEnableEffects(true),
    m_bShowHelp(false),
    m_iMinSize(0),
    m_iMaxSize(0)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsFontDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/fontdlg.h>"),GetInfo().ClassName,hfInPCH);

            wxString sfontName = GetCoderContext()->GetUniqueName(_T("fontData"));
            Codef(_T("\twxFontData %s;\n"), sfontName.wx_str());

            wxString sClr = m_cdColour.BuildCode(GetCoderContext());
            if(!sClr.IsEmpty())
                Codef(_T("\t%s.SetColour(%s);\n"), sfontName.wx_str(), sClr.wx_str());

            if(!m_bEnableEffects)
                Codef(_T("\t%s.EnableEffects(%b);\n"), sfontName.wx_str(), m_bEnableEffects);

            Codef(_T("\t%s.SetInitialFont(*wxNORMAL_FONT);\n"), sfontName.wx_str());

            // These functions are Windows only.
            if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
                if(!m_bShowHelp){
                    Codef(_T("\t%s.SetAllowSymbols(%b);\n"), sfontName.wx_str(), m_bAllowSymbols);
                }
                if(m_iMinSize > 0 || m_iMaxSize > 0){
                    Codef(_T("\t%s.SetRange(%d, %d);\n"), sfontName.wx_str(), m_iMinSize, m_iMaxSize);
                }
                if(m_bShowHelp){
                    Codef(_T("\t%s.SetShowHelp(%b);\n"), sfontName.wx_str(), m_bShowHelp);
                }
            }

            Codef(_T("%C(%W, %s);\n"), sfontName.wx_str());
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFontDialog::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsFontDialog::OnEnumToolProperties(long Flags)
{
    // These functions are Windows only.
    if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
        WXS_BOOL(wxsFontDialog, m_bAllowSymbols, _("Allow Symbols"), _T("allow_symbols"), true)
    }
    WXS_COLOUR(wxsFontDialog, m_cdColour, _("Colour"), _T("colour"));
    // This one is used by generic as well as Windows, so keep it available.
    WXS_BOOL(wxsFontDialog, m_bEnableEffects, _("Enable Effects"), _T("enable_effects"), true)
    // These functions are Windows only.
    if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
        WXS_LONG(wxsFontDialog, m_iMinSize,  _("Min. Size"), _T("min_size"), 0)
        WXS_LONG(wxsFontDialog, m_iMaxSize,  _("Max. Size"), _T("max_size"), 0)
        WXS_BOOL(wxsFontDialog, m_bShowHelp, _("Show Help"), _T("show_help"), false)
    }
}
