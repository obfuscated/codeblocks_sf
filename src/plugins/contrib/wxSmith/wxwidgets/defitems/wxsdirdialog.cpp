/** \file wxsdirdialog.cpp
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

#include "wxsdirdialog.h"
#include "../wxsitemresdata.h"
#include <wx/dirdlg.h>

namespace
{
    wxsRegisterItem<wxsDirDialog> Reg(_T("DirDialog"),wxsTTool,_T("Dialogs"),190,false);

    WXS_ST_BEGIN(wxsDirDialogStyles,_T("wxDD_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxDirDialog")
        WXS_ST(wxDD_DEFAULT_STYLE)
        WXS_ST(wxDD_DIR_MUST_EXIST)
        WXS_ST(wxDD_CHANGE_DIR)
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

wxsDirDialog::wxsDirDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info,0,wxsDirDialogStyles)
{
    m_Message = _("Select directory");
}

void wxsDirDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/dirdlg.h>"),GetInfo().ClassName,hfInPCH);
            #if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("%C(%W, %t, %t, %T, %P, %S, %N);\n"),m_Message.wx_str(),m_DefaultPath.wx_str());
            #else
            Codef(_T("%C(%W, %t, %t, %T, %P, %S, %N);\n"),m_Message.c_str(),m_DefaultPath.c_str());
            #endif
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDirDialog::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

void wxsDirDialog::OnEnumToolProperties(long Flags)
{
    WXS_SHORT_STRING(wxsDirDialog,m_Message,_("Message"),_T("message"),_T(""),false);
    WXS_SHORT_STRING(wxsDirDialog,m_DefaultPath,_("Default path"),_T("default_path"),_T(""),false);
}
