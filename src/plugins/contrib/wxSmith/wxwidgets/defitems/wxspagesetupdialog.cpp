/** \file wxspagesetupdialog.cpp
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

#include "wxspagesetupdialog.h"
#include "../wxsitemresdata.h"
#include <wx/printdlg.h>

namespace
{
    wxsRegisterItem<wxsPageSetupDialog> Reg(_T("PageSetupDialog"), wxsTTool, _T("Dialogs"), 130, false);
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*    The control's resource data.
 *
 */
wxsPageSetupDialog::wxsPageSetupDialog(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info),
    m_bEnableHelp(false),
    m_bEnableMargins(true),
    m_bEnableOrientation(true),
    m_bEnablePaper(true),
    m_bDefaultInfo(false),
    m_bDefaultMinMargins(false),
    m_bEnablePrinter(true),
    m_iMarginLeft(-1),
    m_iMarginTop(-1),
    m_iMarginRight(-1),
    m_iMarginBottom(-1),
    m_iMarginMinLeft(-1),
    m_iMarginMinTop(-1),
    m_iMarginMinRight(-1),
    m_iMarginMinBottom(-1),
    m_iPaperID(wxPAPER_NONE),
    m_iPaperWidth(-1),
    m_iPaperHeight(-1)
{
    // Set up the array of paper IDs.
    arrPaperIDs.Add(wxT("wxPAPER_NONE"));
    arrPaperIDs.Add(wxT("wxPAPER_LETTER"));
    arrPaperIDs.Add(wxT("wxPAPER_LEGAL"));
    arrPaperIDs.Add(wxT("wxPAPER_A4"));
    arrPaperIDs.Add(wxT("wxPAPER_CSHEET"));
    arrPaperIDs.Add(wxT("wxPAPER_DSHEET"));
    arrPaperIDs.Add(wxT("wxPAPER_ESHEET"));
    arrPaperIDs.Add(wxT("wxPAPER_LETTERSMALL"));
    arrPaperIDs.Add(wxT("wxPAPER_TABLOID"));
    arrPaperIDs.Add(wxT("wxPAPER_LEDGER"));
    arrPaperIDs.Add(wxT("wxPAPER_STATEMENT"));
    arrPaperIDs.Add(wxT("wxPAPER_EXECUTIVE"));
    arrPaperIDs.Add(wxT("wxPAPER_A3"));
    arrPaperIDs.Add(wxT("wxPAPER_A4SMALL"));
    arrPaperIDs.Add(wxT("wxPAPER_A5"));
    arrPaperIDs.Add(wxT("wxPAPER_B4"));
    arrPaperIDs.Add(wxT("wxPAPER_B5"));
    arrPaperIDs.Add(wxT("wxPAPER_FOLIO"));
    arrPaperIDs.Add(wxT("wxPAPER_QUARTO"));
    arrPaperIDs.Add(wxT("wxPAPER_10X14"));
    arrPaperIDs.Add(wxT("wxPAPER_11X17"));
    arrPaperIDs.Add(wxT("wxPAPER_NOTE"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_9"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_10"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_11"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_12"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_14"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_DL"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_C5"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_C3"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_C4"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_C6"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_C65"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_B4"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_B5"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_B6"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_ITALY"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_MONARCH"));
    arrPaperIDs.Add(wxT("wxPAPER_ENV_PERSONAL"));
    arrPaperIDs.Add(wxT("wxPAPER_FANFOLD_US"));
    arrPaperIDs.Add(wxT("wxPAPER_FANFOLD_STD_GERMAN"));
    arrPaperIDs.Add(wxT("wxPAPER_FANFOLD_LGL_GERMAN"));
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsPageSetupDialog::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/printdlg.h>"),GetInfo().ClassName,hfInPCH);

            wxString sDataName = GetCoderContext()->GetUniqueName(_T("pageSetupDialogData"));
            #if wxCHECK_VERSION(2, 9, 0)
            AddDeclaration(wxString::Format(wxT("wxPageSetupDialogData  *%s;"), sDataName.wx_str()));
            Codef(_T("\t%s = new wxPageSetupDialogData;\n"), sDataName.wx_str());
            #else
            AddDeclaration(wxString::Format(wxT("wxPageSetupDialogData  *%s;"), sDataName.c_str()));
            Codef(_T("\t%s = new wxPageSetupDialogData;\n"), sDataName.c_str());
            #endif

            // These functions are Windows only.
            if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
                if(m_bEnableHelp){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->EnableHelp(%b);\n"), sDataName.wx_str(), m_bEnableHelp);
                    #else
                    Codef(_T("\t%s->EnableHelp(%b);\n"), sDataName.c_str(), m_bEnableHelp);
                    #endif
                }
                if(!m_bEnableMargins){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->EnableMargins(%b);\n"), sDataName.wx_str(), m_bEnableMargins);
                    #else
                    Codef(_T("\t%s->EnableMargins(%b);\n"), sDataName.c_str(), m_bEnableMargins);
                    #endif
                }
                if(!m_bEnableOrientation){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->EnableOrientation(%b);\n"), sDataName.wx_str(), m_bEnableOrientation);
                    #else
                    Codef(_T("\t%s->EnableOrientation(%b);\n"), sDataName.c_str(), m_bEnableOrientation);
                    #endif
                }
                if(!m_bEnablePaper){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->EnablePaper(%b);\n"), sDataName.wx_str(), m_bEnablePaper);
                    #else
                    Codef(_T("\t%s->EnablePaper(%b);\n"), sDataName.c_str(), m_bEnablePaper);
                    #endif
                }
                if(m_bDefaultInfo){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->SetDefaultInfo(%b);\n"), sDataName.wx_str(), m_bDefaultInfo);
                    #else
                    Codef(_T("\t%s->SetDefaultInfo(%b);\n"), sDataName.c_str(), m_bDefaultInfo);
                    #endif
                }
                if(m_bDefaultMinMargins){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->SetDefaultMinMargins(%b);\n"), sDataName.wx_str(), m_bDefaultMinMargins);
                    #else
                    Codef(_T("\t%s->SetDefaultMinMargins(%b);\n"), sDataName.c_str(), m_bDefaultMinMargins);
                    #endif
                }
            }

            if(!m_bEnablePrinter){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->EnablePrinter(%b);\n"), sDataName.wx_str(), m_bEnablePrinter);
                #else
                Codef(_T("\t%s->EnablePrinter(%b);\n"), sDataName.c_str(), m_bEnablePrinter);
                #endif
            }
            if(m_iMarginLeft > -1 && m_iMarginTop > -1){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetMarginTopLeft(wxPoint(%d, %d));\n"), sDataName.wx_str(), m_iMarginLeft, m_iMarginTop);
                #else
                Codef(_T("\t%s->SetMarginTopLeft(wxPoint(%d, %d));\n"), sDataName.c_str(), m_iMarginLeft, m_iMarginTop);
                #endif
            }
            if(m_iMarginRight > -1 && m_iMarginBottom > -1){
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetMarginBottomRight(wxPoint(%d, %d));\n"), sDataName.wx_str(), m_iMarginRight, m_iMarginBottom);
                #else
                Codef(_T("\t%s->SetMarginBottomRight(wxPoint(%d, %d));\n"), sDataName.c_str(), m_iMarginRight, m_iMarginBottom);
                #endif
            }
            // These functions are Windows only.
            if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
                if(m_iMarginMinLeft > -1 && m_iMarginMinTop > -1){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->SetMinMarginTopLeft(wxPoint(%d, %d));\n"), sDataName.wx_str(), m_iMarginMinLeft, m_iMarginMinTop);
                    #else
                    Codef(_T("\t%s->SetMinMarginTopLeft(wxPoint(%d, %d));\n"), sDataName.c_str(), m_iMarginMinLeft, m_iMarginMinTop);
                    #endif
                }
                if(m_iMarginMinRight > -1 && m_iMarginMinBottom > -1){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->SetMinMarginBottomRight(wxPoint(%d, %d));\n"), sDataName.wx_str(), m_iMarginMinRight, m_iMarginMinBottom);
                    #else
                    Codef(_T("\t%s->SetMinMarginBottomRight(wxPoint(%d, %d));\n"), sDataName.c_str(), m_iMarginMinRight, m_iMarginMinBottom);
                    #endif
                }
            }
            // If PaperId = wxPAPER_NONE and paper size = wxDefaultSize, the default system settings will be used.
            if(m_iPaperID != wxPAPER_NONE){
                // Set paper wxPAPER_NONE is not set.
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("\t%s->SetPaperId(%s);\n"), sDataName.wx_str(), arrPaperIDs[m_iPaperID].wx_str());
                #else
                Codef(_T("\t%s->SetPaperId(%s);\n"), sDataName.c_str(), arrPaperIDs[m_iPaperID].c_str());
                #endif
            }
            else{
                // Set paper size if wxPAPER_NONE is set.
                if(m_iPaperWidth != -1 && m_iPaperHeight != -1){
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("\t%s->SetPaperSize(wxSize(%d, %d));\n"), sDataName.wx_str(), m_iPaperWidth, m_iPaperHeight);
                    #else
                    Codef(_T("\t%s->SetPaperSize(wxSize(%d, %d));\n"), sDataName.c_str(), m_iPaperWidth, m_iPaperHeight);
                    #endif
                }
            }

            #if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("%C(%W, %s);\n"), sDataName.wx_str());
            #else
            Codef(_T("%C(%W, %s);\n"), sDataName.c_str());
            #endif
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsPageSetupDialog::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsPageSetupDialog::OnEnumToolProperties(long Flags)
{
    /*! \brief Paper IDs.
     */
    static const long PaperIDs[] = {
        wxPAPER_NONE,
        wxPAPER_LETTER,
        wxPAPER_LEGAL,
        wxPAPER_A4,
        wxPAPER_CSHEET,
        wxPAPER_DSHEET,
        wxPAPER_ESHEET,
        wxPAPER_LETTERSMALL,
        wxPAPER_TABLOID,
        wxPAPER_LEDGER,
        wxPAPER_STATEMENT,
        wxPAPER_EXECUTIVE,
        wxPAPER_A3,
        wxPAPER_A4SMALL,
        wxPAPER_A5,
        wxPAPER_B4,
        wxPAPER_B5,
        wxPAPER_FOLIO,
        wxPAPER_QUARTO,
        wxPAPER_10X14,
        wxPAPER_11X17,
        wxPAPER_NOTE,
        wxPAPER_ENV_9,
        wxPAPER_ENV_10,
        wxPAPER_ENV_11,
        wxPAPER_ENV_12,
        wxPAPER_ENV_14,
        wxPAPER_ENV_DL,
        wxPAPER_ENV_C5,
        wxPAPER_ENV_C3,
        wxPAPER_ENV_C4,
        wxPAPER_ENV_C6,
        wxPAPER_ENV_C65,
        wxPAPER_ENV_B4,
        wxPAPER_ENV_B5,
        wxPAPER_ENV_B6,
        wxPAPER_ENV_ITALY,
        wxPAPER_ENV_MONARCH,
        wxPAPER_ENV_PERSONAL,
        wxPAPER_FANFOLD_US,
        wxPAPER_FANFOLD_STD_GERMAN,
        wxPAPER_FANFOLD_LGL_GERMAN
    };
    /*! \brief Paper ID names.
     */
    static const wxChar* PaperIDNames[]  = {
        _("Use specific dimensions"),
        _("Letter, 8 1/2 by 11 inches"),
        _("Legal, 8 1/2 by 14 inches"),
        _("A4 Sheet, 210 by 297 millimeters"),
        _("C Sheet, 17 by 22 inches"),
        _("D Sheet, 22 by 34 inches"),
        _("E Sheet, 34 by 44 inches"),
        _("Letter Small, 8 1/2 by 11 inches"),
        _("Tabloid, 11 by 17 inches"),
        _("Ledger, 17 by 11 inches"),
        _("Statement, 5 1/2 by 8 1/2 inches"),
        _("Executive, 7 1/4 by 10 1/2 inches"),
        _("A3 sheet, 297 by 420 millimeters"),
        _("A4 small sheet, 210 by 297 millimeters"),
        _("A5 sheet, 148 by 210 millimeters"),
        _("B4 sheet, 250 by 354 millimeters"),
        _("B5 sheet, 182-by-257-millimeter paper"),
        _("Folio, 8-1/2-by-13-inch paper"),
        _("Quarto, 215-by-275-millimeter paper"),
        _("10-by-14-inch sheet"),
        _("11-by-17-inch sheet"),
        _("Note, 8 1/2 by 11 inches"),
        _("#9 Envelope, 3 7/8 by 8 7/8 inches"),
        _("#10 Envelope, 4 1/8 by 9 1/2 inches"),
        _("#11 Envelope, 4 1/2 by 10 3/8 inches"),
        _("#12 Envelope, 4 3/4 by 11 inches"),
        _("#14 Envelope, 5 by 11 1/2 inches"),
        _("DL Envelope, 110 by 220 millimeters"),
        _("C5 Envelope, 162 by 229 millimeters"),
        _("C3 Envelope, 324 by 458 millimeters"),
        _("C4 Envelope, 229 by 324 millimeters"),
        _("C6 Envelope, 114 by 162 millimeters"),
        _("C65 Envelope, 114 by 229 millimeters"),
        _("B4 Envelope, 250 by 353 millimeters"),
        _("B5 Envelope, 176 by 250 millimeters"),
        _("B6 Envelope, 176 by 125 millimeters"),
        _("Italy Envelope, 110 by 230 millimeters"),
        _("Monarch Envelope, 3 7/8 by 7 1/2 inches"),
        _("6 3/4 Envelope, 3 5/8 by 6 1/2 inches"),
        _("US Std Fanfold, 14 7/8 by 11 inches"),
        _("German Std Fanfold, 8 1/2 by 12 inches"),
        _("German Legal Fanfold, 8 1/2 by 13 inches"),
        NULL
    };


    // These functions are Windows only.
    if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
        WXS_BOOL(wxsPageSetupDialog, m_bDefaultInfo, _("Default Info"), _T("default_info"), false)
        WXS_BOOL(wxsPageSetupDialog, m_bEnableHelp, _("Enable Help"), _T("enable_help"), false)
        WXS_BOOL(wxsPageSetupDialog, m_bEnableMargins, _("Enable Margins"), _T("enable_margins"), true)
    }
    WXS_LONG(wxsPageSetupDialog, m_iMarginLeft,  _("Margin Left (mm)"), _T("margin_left"), -1)
    WXS_LONG(wxsPageSetupDialog, m_iMarginTop,  _("Margin Top (mm)"), _T("margin_top"), -1)
    WXS_LONG(wxsPageSetupDialog, m_iMarginRight,  _("Margin Right (mm)"), _T("margin_right"), -1)
    WXS_LONG(wxsPageSetupDialog, m_iMarginBottom,  _("Margin Bottom (mm)"), _T("margin_bottom"), -1)
    // These functions are Windows only.
    if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
        WXS_BOOL(wxsPageSetupDialog, m_bDefaultMinMargins, _("Default Min. Margins"), _T("default_min_margins"), false)
        WXS_LONG(wxsPageSetupDialog, m_iMarginMinLeft,  _("Min. Margin Left (mm)"), _T("min_margin_left"), -1)
        WXS_LONG(wxsPageSetupDialog, m_iMarginMinTop,  _("Min. Margin Top (mm)"), _T("min_margin_top"), -1)
        WXS_LONG(wxsPageSetupDialog, m_iMarginMinRight,  _("Min. Margin Right (mm)"), _T("min_margin_right"), -1)
        WXS_LONG(wxsPageSetupDialog, m_iMarginMinBottom,  _("Min. Margin Bottom (mm)"), _T("min_margin_bottom"), -1)
    }
    // These functions are Windows only.
    if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
        WXS_BOOL(wxsPageSetupDialog, m_bEnableOrientation, _("Enable Orientation"), _T("enable_orientation"), true)
        WXS_BOOL(wxsPageSetupDialog, m_bEnablePaper, _("Enable Paper"), _T("enable_paper"), true)
    }
    WXS_ENUM(wxsPageSetupDialog, m_iPaperID, _("Paper ID"), _T("paper_id"), PaperIDs, PaperIDNames, wxPAPER_NONE)
    WXS_LONG(wxsPageSetupDialog, m_iPaperWidth,  _("Paper Width"), _T("paper_width"), -1)
    WXS_LONG(wxsPageSetupDialog, m_iPaperHeight,  _("Paper Height"), _T("paper_height"), -1)
    WXS_BOOL(wxsPageSetupDialog, m_bEnablePrinter, _("Enable Printer"), _T("enable_printer"), true)
}
