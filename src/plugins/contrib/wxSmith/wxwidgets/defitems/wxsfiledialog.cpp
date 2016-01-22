/** \file wxsfiledialog.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsfiledialog.h"
#include "../wxsitemresdata.h"
#include <wx/filedlg.h>

namespace
{
    wxsRegisterItem<wxsFileDialog> Reg(_T("FileDialog"),wxsTTool,_T("Dialogs"),180,false);

    WXS_ST_BEGIN(wxsFileDialogStyles,_T("wxFD_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxFileDialog")
        WXS_ST(wxFD_DEFAULT_STYLE)
        WXS_ST(wxFD_OPEN)
        WXS_ST(wxFD_SAVE)
        WXS_ST(wxFD_OVERWRITE_PROMPT)
        WXS_ST(wxFD_FILE_MUST_EXIST)
        WXS_ST(wxFD_MULTIPLE)
        WXS_ST(wxFD_CHANGE_DIR)
        WXS_ST(wxFD_PREVIEW)
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

wxsFileDialog::wxsFileDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info,0,wxsFileDialogStyles)
{
    m_Message = _("Select file");
}

void wxsFileDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/filedlg.h>"),GetInfo().ClassName,hfInPCH);
            if ( m_Wildcard.empty() )
            {
                Codef(_T("%C(%W, %t, %t, %t, wxFileSelectorDefaultWildcardStr, %T, %P, %S, %N);\n"),
                      m_Message.wx_str(),
                      m_DefaultDir.wx_str(),
                      m_DefaultFile.wx_str());
            }
            else
            {
                Codef(_T("%C(%W, %t, %t, %t, %t, %T, %P, %S, %N);\n"),
                      m_Message.wx_str(),
                      m_DefaultDir.wx_str(),
                      m_DefaultFile.wx_str(),
                      m_Wildcard.wx_str());
            }
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFileDialog::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

void wxsFileDialog::OnEnumToolProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsFileDialog,m_Message,_("Message"),_T("message"),_T(""),false);
    WXS_SHORT_STRING(wxsFileDialog,m_DefaultDir,_("Default directory"),_T("default_dir"),_T(""),false);
    WXS_SHORT_STRING(wxsFileDialog,m_DefaultFile,_("Default file"),_T("default_file"),_T(""),false);
    WXS_SHORT_STRING(wxsFileDialog,m_Wildcard,_("Wildcard"),_T("wildcard"),_T(""),false);
}
