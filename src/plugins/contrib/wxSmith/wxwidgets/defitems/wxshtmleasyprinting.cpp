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

#include "wxshtmleasyprinting.h"
#include <wx/html/htmprint.h>

namespace
{
    wxsRegisterItem<wxsHtmlEasyPrinting> Reg(_T("HtmlEasyPrinting"), wxsTTool, _T("Tools"), 75, false);
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsHtmlEasyPrinting::wxsHtmlEasyPrinting(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        NULL,
        NULL),
        m_sHeader(wxEmptyString),
        m_sFooter(wxEmptyString),
        m_iHeaderPages(wxPAGE_ALL),
        m_iFooterPages(wxPAGE_ALL)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsHtmlEasyPrinting::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/html/htmprint.h>"),GetInfo().ClassName,hfInPCH);

            Codef(_T("%C(%N, %W);\n"));

            if(!m_sHeader.IsEmpty()){
#if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("%ASetHeader(%s, %d);\n"), m_sHeader.wx_str(), m_iHeaderPages);
#else
                Codef(_T("%ASetHeader(%s, %d);\n"), m_sHeader.c_str(), m_iHeaderPages);
#endif
            }
            if(!m_sFooter.IsEmpty()){
#if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("%ASetHeader(%s, %d);\n"), m_sFooter.wx_str(), m_iFooterPages);
#else
                Codef(_T("%ASetHeader(%s, %d);\n"), m_sFooter.c_str(), m_iFooterPages);
#endif
            }

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsHtmlEasyPrinting::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsHtmlEasyPrinting::OnEnumToolProperties(long flags)
{
    static const long arrHeaderPages[] = {wxPAGE_ALL, wxPAGE_EVEN, wxPAGE_ODD};                                                                            //!< Header and footer page values.
    static const wxChar* arrHeaderPageNames[]  = {wxT("wxPAGE_ALL"), wxT("wxPAGE_EVEN"), wxT("wxPAGE_ODD"), NULL};        //!< Header and footer page value names.

    WXS_SHORT_STRING(wxsHtmlEasyPrinting, m_sHeader, _("Header"), _T("header"), wxEmptyString, false)
    WXS_SHORT_STRING(wxsHtmlEasyPrinting, m_sFooter, _("Footer"), _T("footer"), wxEmptyString, false)
    WXS_ENUM(wxsHtmlEasyPrinting, m_iHeaderPages, _("Header Pages"), _T("header_pages"), arrHeaderPages, arrHeaderPageNames, wxPAGE_ALL);
    WXS_ENUM(wxsHtmlEasyPrinting, m_iFooterPages, _("Footer Pages"), _T("footer_pages"), arrHeaderPages, arrHeaderPageNames, wxPAGE_ALL);
}

