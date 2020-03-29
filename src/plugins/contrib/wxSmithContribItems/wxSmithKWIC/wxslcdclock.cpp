/**************************************************************************//**
 * \file
 * \author    Gary Harris
 * \date
 *
 * This file is part of wxSmithKWIC.
 *
 * DoxyBlocks - doxygen integration for Code::Blocks.                    \n
 * Copyright (C) 2010 Gary Harris.
 *
 * wxSmithKWIC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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
#include "wxslcdclock.h"
#include "wx/KWIC/LCDClock.h"

namespace
{
    // Loading images from xpm files
    #include "images/lcd16.xpm"
    #include "images/lcd32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsLCDClock> Reg(
        _T("kwxLCDClock"),                                             // Class name
        wxsTWidget,                                                            // Item type
        _T("KWIC License"),                                           // License
        _T("Andrea V. & Marco Cavallini"),                   // Author
        _T("m.cavallini@koansoftware.com"),              // Author's email
        _T("http://www.koansoftware.com/kwic/"),        // Item's homepage
        _T("KWIC"),                                                             // Category in palette
        60,                                                                            // Priority in palette
        _T("LCDClock"),                                                   // Base part of names for new items
        wxsCPP,                                                                // List of coding languages supported by this item
        1, 0,                                                                          // Version
        wxBitmap(lcd32_xpm),                                       // 32x32 bitmap
        wxBitmap(lcd16_xpm),                                       // 16x16 bitmap
        true);                                                                        // We do not allow this item inside XRC files
}

/*! \brief Constructor.
 *
 * \param Data wxsItemResData*    Pointer to a resource data object.
 *
 */
wxsLCDClock::wxsLCDClock(wxsItemResData* Data) :
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable|flId|flPosition|flSize|flEnabled|flFocused|flHidden|flColours|flToolTip|flHelpText|flSubclass|flMinMaxSize),
        m_iNumDigits(6)
{
}

/*! \brief Destructor.
 */
wxsLCDClock::~wxsLCDClock()
{
}

/*! \brief Build the code that creates the control.
 *
 * \return void
 *
 */
void wxsLCDClock::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("\"wx/KWIC/LCDClock.h\""),GetInfo().ClassName);
            Codef(_T("%C(%W,%P,%S);\n"));

            // 6 digits is the default value.
            if (m_iNumDigits && m_iNumDigits != 6){
                Codef(_T("%ASetNumberDigits(%d);\n"), static_cast<int>(m_iNumDigits));
            }
            wxString ss = m_cdLightColour.BuildCode( GetCoderContext() );
            if (!ss.IsEmpty()) Codef(_T("%ASetLightColour(%s);\n"), ss.wx_str());

            ss = m_cdGrayColour.BuildCode( GetCoderContext() );
            if (!ss.IsEmpty()) Codef(_T("%ASetGrayColour(%s);\n"), ss.wx_str());

            BuildSetupWindowCode();
            break;
        }
        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsLCDClock::OnBuildCreatingCode"),GetLanguage());
    }
}

/*! \brief Build the code that creates the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                    Flags used when creating the preview.
 * \return wxObject                        The control preview object.
 *
 */
wxObject* wxsLCDClock::OnBuildPreview(wxWindow* parent, long flags)
{
   kwxLCDClock* preview = new kwxLCDClock(parent,Pos(parent),Size(parent));

    // 6 digits is the default value.
    if(m_iNumDigits && m_iNumDigits != 6){
        preview->SetNumberDigits(m_iNumDigits);
    }
    wxColour cc = m_cdLightColour.GetColour();
    if(cc.IsOk()){
        preview->SetLightColour(cc);
    }
    cc = m_cdGrayColour.GetColour();
    if(cc.IsOk()){
        preview->SetGrayColour(cc);
    }

    return SetupWindow(preview,flags);
}

/*! \brief Enumerate the control's custom properties.
 *
 * \param Flags long    Flags used when creating the control.
 * \return void
 *
 */
void wxsLCDClock::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_LONG(wxsLCDClock, m_iNumDigits, _("Number of digits"), _T("num_digits"), 0)
    WXS_COLOUR(wxsLCDClock, m_cdLightColour , _("Active segments"), _T("active_colour"))
    WXS_COLOUR(wxsLCDClock, m_cdGrayColour, _("Inactive segments"), _T("inactive_colour"))
}

