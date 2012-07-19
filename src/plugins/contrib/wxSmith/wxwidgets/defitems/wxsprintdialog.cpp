/** \file wxsprintdialog.cpp
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

#include "wxsprintdialog.h"
#include "../wxsitemresdata.h"
#include <wx/printdlg.h>

namespace
{
    wxsRegisterItem<wxsPrintDialog> Reg(_T("PrintDialog"), wxsTTool, _T("Dialogs"), 110, false);
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsPrintDialog::wxsPrintDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info),
    m_bEnableHelp(false),
    m_bEnablePageNumbers(true),
    m_bEnablePrintToFile(true),
    m_bEnableSelection(false),
    m_bCollate(false),
    m_bPrintToFile(false),
    m_bSelection(false),
    m_iFromPage(0),
    m_iToPage(0),
    m_iMinPage(0),
    m_iMaxPage(0),
    m_iNoCopies(1)
{
}

/*! \brief Create the dialogue.
 *
 * \return void
 *
 */
void wxsPrintDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/printdlg.h>"),GetInfo().ClassName,hfInPCH);

            wxString sDataName = GetCoderContext()->GetUniqueName(_T("printDialogData"));
            #if wxCHECK_VERSION(2, 9, 0)
            AddDeclaration(wxString::Format(wxT("wxPrintDialogData  *%s;"), sDataName.wx_str()));
            Codef(_T("\t%s = new wxPrintDialogData;\n"), sDataName.wx_str());
            #else
            AddDeclaration(wxString::Format(wxT("wxPrintDialogData  *%s;"), sDataName.c_str()));
            Codef(_T("\t%s = new wxPrintDialogData;\n"), sDataName.c_str());
            #endif

            if(m_bEnableHelp){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->EnableHelp(%b);\n"), sDataName.wx_str(), m_bEnableHelp);
                #else
                Codef(_T("\t%s->EnableHelp(%b);\n"), sDataName.c_str(), m_bEnableHelp);
                #endif
            }
            if(!m_bEnablePageNumbers){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->EnablePageNumbers(%b);\n"), sDataName.wx_str(), m_bEnablePageNumbers);
                #else
                Codef(_T("\t%s->EnablePageNumbers(%b);\n"), sDataName.c_str(), m_bEnablePageNumbers);
                #endif
            }
            if(!m_bEnablePrintToFile){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->EnablePrintToFile(%b);\n"), sDataName.wx_str(), m_bEnablePrintToFile);
                #else
                Codef(_T("\t%s->EnablePrintToFile(%b);\n"), sDataName.c_str(), m_bEnablePrintToFile);
                #endif
            }
            if(m_bEnableSelection){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->EnableSelection(%b);\n"), sDataName.wx_str(), m_bEnableSelection);
                #else
                Codef(_T("\t%s->EnableSelection(%b);\n"), sDataName.c_str(), m_bEnableSelection);
                #endif
                if(m_bSelection){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->SetSelection(%b);\n"), sDataName.wx_str(), m_bSelection);
                    #else
                    Codef(_T("\t%s->SetSelection(%b);\n"), sDataName.c_str(), m_bSelection);
                    #endif
                }
            }
            if(m_bCollate){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetCollate(%b);\n"), sDataName.wx_str(), m_bCollate);
                #else
                Codef(_T("\t%s->SetCollate(%b);\n"), sDataName.c_str(), m_bCollate);
                #endif
            }
            if(m_iFromPage > 0){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetFromPage(%d);\n"), sDataName.wx_str(), m_iFromPage);
                #else
                Codef(_T("\t%s->SetFromPage(%d);\n"), sDataName.c_str(), m_iFromPage);
                #endif
            }
            if(m_iToPage > 0){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetToPage(%d);\n"), sDataName.wx_str(), m_iToPage);
                #else
                Codef(_T("\t%s->SetToPage(%d);\n"), sDataName.c_str(), m_iToPage);
                #endif
            }
            if(m_iMinPage > 0){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetMinPage(%d);\n"), sDataName.wx_str(), m_iMinPage);
                #else
                Codef(_T("\t%s->SetMinPage(%d);\n"), sDataName.c_str(), m_iMinPage);
                #endif
            }
            if(m_iMaxPage > 0){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetMaxPage(%d);\n"), sDataName.wx_str(), m_iMaxPage);
                #else
                Codef(_T("\t%s->SetMaxPage(%d);\n"), sDataName.c_str(), m_iMaxPage);
                #endif
            }
            if(m_iNoCopies > 1){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetNoCopies(%d);\n"), sDataName.wx_str(), m_iNoCopies);
                #else
                Codef(_T("\t%s->SetNoCopies(%d);\n"), sDataName.c_str(), m_iNoCopies);
                #endif
            }

            #if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("%C(%W, %v);\n"), sDataName.wx_str());
            #else
            Codef(_T("%C(%W, %v);\n"), sDataName.c_str());
            #endif
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsPrintDialog::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief Enumerate the dialogue's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsPrintDialog::OnEnumToolProperties(long Flags)
{
    WXS_BOOL(wxsPrintDialog, m_bEnableHelp, _("Enable Help"), _T("enable_help"), false)
    WXS_BOOL(wxsPrintDialog, m_bEnablePageNumbers, _("Enable Page Numbers"), _T("enable_page_numbers"), true)
    WXS_LONG(wxsPrintDialog, m_iFromPage,  _("From Page"), _T("from_page"), 0)
    WXS_LONG(wxsPrintDialog, m_iToPage,  _("To Page"), _T("to_page"), 0)
    WXS_LONG(wxsPrintDialog, m_iMinPage,  _("Min. Page"), _T("min_page"), 0)
    WXS_LONG(wxsPrintDialog, m_iMaxPage,  _("Max. Page"), _T("max_page"), 0)
    WXS_LONG(wxsPrintDialog, m_iNoCopies,  _("Number of Copies"), _T("number_of_copies"), 1)
    WXS_BOOL(wxsPrintDialog, m_bCollate, _("Collate"), _T("collate"), false)
    WXS_BOOL(wxsPrintDialog, m_bEnablePrintToFile, _("Enable Print To File"), _T("enable_print_to_file"), true)
    WXS_BOOL(wxsPrintDialog, m_bPrintToFile, _("Print To File"), _T("print_to_file"), false)
    WXS_BOOL(wxsPrintDialog, m_bEnableSelection, _("Enable Selection"), _T("enable_selection"), false)
    WXS_BOOL(wxsPrintDialog, m_bSelection, _("Selection"), _T("selection"), false)
}
