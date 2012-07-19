/** \file wxshyperlinkctrl.cpp
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

#include "wxshyperlinkctrl.h"
#include "wx/hyperlink.h"


namespace
{
    wxsRegisterItem<wxsHyperlinkCtrl> Reg(_T("HyperlinkCtrl"),wxsTWidget,_T("Standard"),260);

    WXS_ST_BEGIN(wxsHyperlinkCtrlStyles,_T("wxHL_CONTEXTMENU|wxNO_BORDER|wxHL_ALIGN_CENTRE"))
        WXS_ST_CATEGORY("wxHyperlinkCtrl")
        WXS_ST(wxHL_CONTEXTMENU)
        WXS_ST(wxHL_ALIGN_LEFT)
        WXS_ST(wxHL_ALIGN_RIGHT)
        WXS_ST(wxHL_ALIGN_CENTRE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsHyperlinkCtrlEvents)
        WXS_EVI(EVT_HYPERLINK,wxEVT_COMMAND_HYPERLINK,wxCommandEvent,Click)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsHyperlinkCtrl::wxsHyperlinkCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsHyperlinkCtrlEvents,
        wxsHyperlinkCtrlStyles),
        m_Label(wxT("http://www.codeblocks.org"))
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsHyperlinkCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/hyperlink.h>"),GetInfo().ClassName,hfInPCH);

            #if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("%C(%W, %I, %t, %t, %P, %S, %T, %N);\n"),m_Label.wx_str(), m_URL.wx_str());
            #else
            Codef(_T("%C(%W, %I, %t, %t, %P, %S, %T, %N);\n"),m_Label.c_str(), m_URL.c_str());
            #endif

            wxString ss = m_NormalColour.BuildCode( GetCoderContext() );
            #if wxCHECK_VERSION(2, 9, 0)
            if (!ss.IsEmpty()) Codef(_T("%ASetNormalColour(%s);\n"), ss.wx_str());
            #else
            if (!ss.IsEmpty()) Codef(_T("%ASetNormalColour(%s);\n"), ss.c_str());
            #endif

            ss = m_HoverColour.BuildCode( GetCoderContext() );
            #if wxCHECK_VERSION(2, 9, 0)
            if (!ss.IsEmpty()) Codef(_T("%ASetHoverColour(%s);\n"), ss.wx_str());
            #else
            if (!ss.IsEmpty()) Codef(_T("%ASetHoverColour(%s);\n"), ss.c_str());
            #endif

            ss = m_VisitedColour.BuildCode( GetCoderContext() );
            #if wxCHECK_VERSION(2, 9, 0)
            if (!ss.IsEmpty()) Codef(_T("%ASetVisitedColour(%s);\n"), ss.wx_str());
            #else
            if (!ss.IsEmpty()) Codef(_T("%ASetVisitedColour(%s);\n"), ss.c_str());
            #endif

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsHyperlinkCtrl::OnBuildCreatingCode"),GetLanguage());
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
wxObject* wxsHyperlinkCtrl::OnBuildPreview(wxWindow* parent,long flags)
{
    wxHyperlinkCtrl* preview = new wxHyperlinkCtrl(parent,GetId(),m_Label,m_URL,Pos(parent),Size(parent),Style());
    wxColour cc = m_NormalColour.GetColour();
    if ( cc.IsOk() )
    {
        preview->SetNormalColour(cc);
    }
    cc = m_HoverColour.GetColour();
    if ( cc.IsOk() )
    {
        preview->SetHoverColour(cc);
    }
    cc = m_VisitedColour.GetColour();
    if ( cc.IsOk() )
    {
        preview->SetVisitedColour(cc);
    }

    return SetupWindow(preview,flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsHyperlinkCtrl::OnEnumWidgetProperties(long flags)
{
    WXS_STRING(wxsHyperlinkCtrl,m_Label,_("Label"),_T("label"),_T(""),false)
    WXS_SHORT_STRING(wxsHyperlinkCtrl,m_URL,_("URL"),_T("url"),_T(""),false)
    WXS_COLOUR(wxsHyperlinkCtrl, m_NormalColour,_T("Normal"),_T("normal_colour"))
    WXS_COLOUR(wxsHyperlinkCtrl, m_HoverColour,_T("Hover"),_T("hover_colour"))
    WXS_COLOUR(wxsHyperlinkCtrl, m_VisitedColour,_T("Visited"),_T("visited_colour"))
}

