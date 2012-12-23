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
*/

#include "wxsanimationctrl.h"
#include "wx/animate.h"

#include <prep.h>

namespace
{
    wxsRegisterItem<wxsAnimationCtrl> Reg(_T("AnimationCtrl"),wxsTWidget,_T("Standard"),370);

    WXS_ST_BEGIN(wxsAnimationCtrlStyles,_T("wxAC_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxAnimationCtrl")
        WXS_ST(wxAC_DEFAULT_STYLE)
        WXS_ST(wxAC_NO_AUTORESIZE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsAnimationCtrl::wxsAnimationCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        wxsAnimationCtrlStyles),
        m_bPlay(false)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsAnimationCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/animate.h>"),GetInfo().ClassName,hfInPCH);

            wxString sAnimName = GetCoderContext()->GetUniqueName(_T("anim"));
            Codef(_T("\twxAnimation %s(%n);\n"), sAnimName.wx_str(), m_sAnimation.wx_str());
            Codef(_T("%C(%W, %I, %s, %P, %S, %T, %N);\n"), sAnimName.wx_str());

            if(!m_bmpInactive.IsEmpty()){
                Codef(_T("%ASetInactiveBitmap(%i);\n"), &m_bmpInactive, _T("wxART_OTHER"));
            }
            if(m_bPlay){
                Codef(_T("%APlay();\n"));
            }

            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsAnimationCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsAnimationCtrl::OnBuildPreview(wxWindow* parent,long flags)
{
    wxAnimation anim; anim.LoadFile(m_sAnimation);
    wxAnimationCtrl* preview = new wxAnimationCtrl(parent, GetId(), anim, Pos(parent), Size(parent), Style());

    if(!m_bmpInactive.IsEmpty())
    {
        preview->SetInactiveBitmap(m_bmpInactive.GetPreview(wxDefaultSize));
    }
    if(m_bPlay)
    {
        preview->Play();
    }

    return SetupWindow(preview,flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsAnimationCtrl::OnEnumWidgetProperties(long flags)
{
    WXS_SHORT_STRING(wxsAnimationCtrl, m_sAnimation, _("Animation"), _T("animation"), _T(""), false)
    WXS_BITMAP(wxsAnimationCtrl, m_bmpInactive, _("Inactive Bitmap"), _T("inactive_bitmap"), _T("wxART_OTHER"));
    WXS_BOOL(wxsAnimationCtrl, m_bPlay, _("Play"), _T("play"), false)
}

