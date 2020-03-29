/** \file wxssymbolpickerdialog.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2008 Ron Collins
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

#include "wxssymbolpickerdialog.h"
#include "../wxsitemresdata.h"
#include <wx/richtext/richtextsymboldlg.h>

//------------------------------------------------------------------------------

namespace
{
    wxsRegisterItem<wxsSymbolPickerDialog> Reg(
        _T("SymbolPickerDialog"),       // Class base name
        wxsTTool,                       // Item type
        _T("Dialogs"),                  // Category in palette
        60,                             // Priority in palette
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsSymbolPickerDialogStyles,_T("wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX"))
        WXS_ST_CATEGORY("wxSymbolPickerDialog")
        WXS_ST(wxSTAY_ON_TOP)
        WXS_ST(wxCAPTION)
        WXS_ST(wxDEFAULT_DIALOG_STYLE)
        WXS_ST(wxSYSTEM_MENU)
        WXS_ST(wxRESIZE_BORDER)
        WXS_ST(wxCLOSE_BOX)
        WXS_ST(wxDIALOG_NO_PARENT)
        WXS_ST(wxTAB_TRAVERSAL)
        WXS_ST(wxMAXIMIZE_BOX)
        WXS_ST(wxMINIMIZE_BOX)
        WXS_ST(wxFRAME_SHAPED)
        WXS_EXST(wxDIALOG_EX_CONTEXTHELP)
        WXS_EXST(wxDIALOG_EX_METAL)
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

//------------------------------------------------------------------------------

wxsSymbolPickerDialog::wxsSymbolPickerDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info,0,wxsSymbolPickerDialogStyles, (flVariable|flId|flSubclass|flExtraCode))
{
}

//------------------------------------------------------------------------------

void wxsSymbolPickerDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddHeader(_T("<wx/richtext/richtextsymboldlg.h>"),GetInfo().ClassName, 0);
            Codef(_T("%C( %t, %t, %t, %W, %I, %t, %P, %S, %T);\n"), _T(""), _T(""), _T(""), _T("Title") );
            BuildSetupWindowCode();
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsSymbolPickerDialog::OnBuildCreatingCode"),GetLanguage());
    }
}

//------------------------------------------------------------------------------

void wxsSymbolPickerDialog::OnEnumToolProperties(cb_unused long Flags)
{
}
