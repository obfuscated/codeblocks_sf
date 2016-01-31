/**************************************************************************//**
 * \file            wxsbmpcheckbox.cpp
 * \author    Gary Harris
 * \date        19/4/2010.
 *
 * This file is part of wxSmithKWIC.
 *
 * wxSmithKWIC - an add-on for wxSmith, Code::Blocks' GUI editor.                    \n
 * Copyright (C) 2010 Gary Harris.
 *
 * wxSmithKWIC is free software: you can redistribute it and/or modify
 * it under the terms of the KWIC License
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * KWIC License for more details.
 *
 * You should have received a copy of the KWIC License along with this
 * program.  If not, see <http://www.koansoftware.com/kwic/kwic-license.htm>.
 *
 *****************************************************************************/
#include "wxsbmpcheckbox.h"
#include "wx/KWIC/BmpCheckBox.h"

namespace
{
    // Loading images from xpm files
#include "images/bmpchk16.xpm"
#include "images/bmpchk32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsBmpCheckbox> Reg(
        _T("kwxBmpCheckBox"),                                 // Class name
        wxsTWidget,                                                    // Item type
        _T("KWIC License"),                                       // License
        _T("Andrea V. & Marco Cavallini"),                   // Author
        _T("m.cavallini@koansoftware.com"),              // Author's email
        _T("http://www.koansoftware.com/kwic/"),        // Item's homepage
        _T("KWIC"),                                                     // Category in palette
        80,                                                                // Priority in palette
        _T("BmpCheckbox"),                                       // Base part of names for new items
        wxsCPP,                                                        // List of coding languages supported by this item
        1, 0,                                                              // Version
        wxBitmap(bmpchk32_xpm),                               // 32x32 bitmap
        wxBitmap(bmpchk16_xpm),                               // 16x16 bitmap
        true);                                                            // We do not allow this item inside XRC files

    WXS_EV_BEGIN(wxsBmpCheckboxEvents)
        WXS_EVI(EVT_BMPCHECKBOX_CLICKED, kwxEVT_BITBUTTON_CLICK, wxCommandEvent, Clicked)
    WXS_EV_END()

}

/*! \brief Constructor.
 *
 * \param Data wxsItemResData*    Pointer to a resource data object.
 *
 */
wxsBmpCheckbox::wxsBmpCheckbox(wxsItemResData *Data) :
    wxsWidget(
        Data,
        &Reg.Info,
        wxsBmpCheckboxEvents,
        NULL,
        flVariable | flId | flPosition | flSize | flEnabled | flFocused | flHidden | flToolTip | flHelpText | flSubclass | flMinMaxSize),
        m_bBorder(true),
        m_iBorderStyle(wxDOT),
        m_bChecked(false)
{
}

/*! \brief Destructor.
 */
wxsBmpCheckbox::~wxsBmpCheckbox()
{
}

/*! \brief Build the code that creates the control.
 *
 * \return void
 *
 */
void wxsBmpCheckbox::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
        {
            AddHeader(_T("\"wx/KWIC/BmpCheckBox.h\""), GetInfo().ClassName);
            // Write different code depending on whether the bitmaps are set or wxNullBitmap is used.
            wxString sOn, sOff, sOnSel, sOffSel;
            if(m_bdOn.IsEmpty()){
                sOn = wxT("wxNullBitmap");
            }
            else{
                sOn = wxT("*pbmpOn");
                Codef(_T("wxBitmap *pbmpOn = new %i;\n"), &m_bdOn, _T("wxART_OTHER"));
            }
            if(m_bdOff.IsEmpty()){
                sOff = wxT("wxNullBitmap");
            }
            else{
                sOff = wxT("*pbmpOff");
                Codef(_T("wxBitmap *pbmpOff = new %i;\n"), &m_bdOff, _T("wxART_OTHER"));
            }
            if(m_bdOnSel.IsEmpty()){
                sOnSel = wxT("wxNullBitmap");
            }
            else{
                sOnSel = wxT("*pbmpOnSel");
                Codef(_T("wxBitmap *pbmpOnSel = new %i;\n"), &m_bdOnSel, _T("wxART_OTHER"));
            }
            if(m_bdOffSel.IsEmpty()){
                sOffSel = wxT("wxNullBitmap");
            }
            else{
                sOffSel = wxT("*pbmpOffSel");
                Codef(_T("wxBitmap *pbmpOffSel = new %i;\n"), &m_bdOffSel, _T("wxART_OTHER"));
            }
#if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("%C(%W,%I, %s, %s, %s, %s, %P,%S, %s);\n"), sOn.wx_str(), sOff.wx_str(), sOnSel.wx_str(), sOffSel.wx_str(), _T("wxBORDER_NONE"));
#else
            Codef(_T("%C(%W,%I, %s, %s, %s, %s, %P,%S, %s);\n"), sOn.c_str(), sOff.c_str(), sOnSel.c_str(), sOffSel.c_str(), _T("wxBORDER_NONE"));
#endif
            // The defaults are border on and wxDOT.
            if(!m_bBorder || (m_bBorder && m_iBorderStyle != wxDOT)){
                Codef(_T("%ASetBorder(%b, %d);\n"), m_bBorder, m_iBorderStyle);
            }
            if(m_bChecked){
                Codef(_T("%ASetState(true);\n"));
            }

            BuildSetupWindowCode();
            break;
        }
        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsBmpCheckbox::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Build the code that creates the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                    Flags used when creating the preview.
 * \return wxObject                        The control preview object.
 *
 */
wxObject *wxsBmpCheckbox::OnBuildPreview(wxWindow *parent, long flags)
{
    wxBitmap *pbmpOn = new wxBitmap(m_bdOn.GetPreview(wxDefaultSize));
    wxBitmap *pbmpOff = new wxBitmap(m_bdOff.GetPreview(wxDefaultSize));
    wxBitmap *pbmpOnSel = new wxBitmap(m_bdOnSel.GetPreview(wxDefaultSize));
    wxBitmap *pbmpOffSel = new wxBitmap(m_bdOffSel.GetPreview(wxDefaultSize));
    kwxBmpCheckBox *preview = new kwxBmpCheckBox(parent, GetId(), *pbmpOn, *pbmpOff, *pbmpOnSel, *pbmpOffSel, Pos(parent), Size(parent), wxBORDER_NONE);

    // The defaults are border on and wxDOT.
    if(!m_bBorder || (m_bBorder && m_iBorderStyle != wxDOT)){
        preview->SetBorder(m_bBorder, m_iBorderStyle);
    }
    if(m_bChecked){
        preview->SetState(true);
    }

    return SetupWindow(preview, flags);
}

/*! \brief Enumerate the control's custom properties.
 *
 * \param Flags long    Flags used when creating the control.
 * \return void
 *
 */
void wxsBmpCheckbox::OnEnumWidgetProperties(cb_unused long Flags)
{
    static const long         arrBorderStyles[] = {wxSOLID, wxDOT, wxLONG_DASH, wxSHORT_DASH, wxDOT_DASH};                                                            //!< Border styles array.
    static const wxChar    *arrStyleNames[]  = { _("wxSOLID"), _("wxDOT"), _("wxLONG_DASH"), _("wxSHORT_DASH"), _("wxDOT_DASH"), NULL };        //!< Border style names array.

    WXS_BOOL(wxsBmpCheckbox, m_bBorder, _("Show Border"), _T("show_border"), true)
    WXS_ENUM(wxsBmpCheckbox, m_iBorderStyle, _("Border Style"), _T("border_style"), arrBorderStyles, arrStyleNames, wxDOT);
    WXS_BOOL(wxsBmpCheckbox, m_bChecked, _("Checked"), _T("checked"), false)
    WXS_BITMAP(wxsBmpCheckbox, m_bdOn, _("On Bitmap"),_T("bitmap_on"), _T("wxART_OTHER"))
    WXS_BITMAP(wxsBmpCheckbox, m_bdOff, _("Off Bitmap"),_T("bitmap_off"), _T("wxART_OTHER"))
    WXS_BITMAP(wxsBmpCheckbox, m_bdOnSel, _("On Selected Bitmap"),_T("bitmap_on_selected"), _T("wxART_OTHER"))
    WXS_BITMAP(wxsBmpCheckbox, m_bdOffSel, _("Off Selected Bitmap"),_T("bitmap_off_selected"), _T("wxART_OTHER"))
}

