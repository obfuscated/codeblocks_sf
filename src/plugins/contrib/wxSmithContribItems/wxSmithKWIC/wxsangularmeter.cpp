/**************************************************************************//**
 * \file		wxsangularmeter.cpp
 * \author	Gary Harris
 * \date		19/4/2010.
 *
 * This file is part of wxSmithKWIC.
 *
 * wxSmithKWIC - an add-on for wxSmith, Code::Blocks' GUI editor.					\n
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
#include "wxsangularmeter.h"
#include "wx/KWIC/AngularMeter.h"

namespace
{
    // Loading images from xpm files
#include "images/angmet16.xpm"
#include "images/angmet32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsAngularMeter> Reg(
        _T("kwxAngularMeter"),                     				// Class name
        wxsTWidget,                            						// Item type
        _T("KWIC License"),                       				// License
        _T("Andrea V. & Marco Cavallini"),               	// Author
        _T("m.cavallini@koansoftware.com"),  			// Author's email
        _T("http://www.koansoftware.com/kwic/"),    	// Item's homepage
        _T("KWIC"),                         							// Category in palette
        100,                                    							// Priority in palette
        _T("AngularMeter"),                           				// Base part of names for new items
        wxsCPP,                                						// List of coding languages supported by this item
        1, 0,                                  							// Version
        wxBitmap(angmet32_xpm),               				// 32x32 bitmap
        wxBitmap(angmet16_xpm),               				// 16x16 bitmap
        true);                                							// We do not allow this item inside XRC files
}

/*! \brief Constructor.
 *
 * \param Data wxsItemResData*	Pointer to a resource data object.
 *
 */
wxsAngularMeter::wxsAngularMeter(wxsItemResData *Data) :
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable | flId | flPosition | flSize | flEnabled | flFocused | flHidden | flToolTip | flHelpText | flSubclass | flMinMaxSize),
    m_iValue(0),
    m_iNumTicks(0),
    m_bShowVal(true),
    m_iRangeMin(0),
    m_iRangeMax(220),
    m_iAngleMin(-20),
    m_iAngleMax(200),
    m_iNumSectors(1)
{
	// There must be 1 sector to start with.
    SectorDesc *Desc = new SectorDesc;
    Desc->colour = *wxWHITE;
	m_arrSectors.Add(Desc);
}

/*! \brief Destructor.
 */
wxsAngularMeter::~wxsAngularMeter()
{
    for(size_t i = 0; i < m_arrSectors.Count(); i++){
        delete m_arrSectors[i];
    }
    m_arrSectors.Clear();
}

/*! \brief Build the code that creates the control.
 *
 * \return void
 *
 */
void wxsAngularMeter::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
		{
			AddHeader(_T("\"wx/KWIC/AngularMeter.h\""), GetInfo().ClassName);
			Codef(_T("%C(%W,%I,%P,%S);\n"));

			if(m_iNumTicks > 0){
				Codef(_T("%ASetNumTick(%d);\n"), m_iNumTicks);
			}
			// Default range is 0-220.
			if(m_iRangeMin != 0 || m_iRangeMax != 220){
				Codef(_T("%ASetRange(%d, %d);\n"), m_iRangeMin, m_iRangeMax);
			}
			// Default angles are -20, 200.
			if(m_iAngleMin != -20 || m_iAngleMax != 200){
				Codef(_T("%ASetAngle(%d, %d);\n"), m_iAngleMin, m_iAngleMax);
			}
			m_iNumSectors = m_arrSectors.Count();
			// Default number of sectors is 1.
			if(m_iNumSectors > 1){
				Codef(_T("%ASetNumSectors(%d);\n"), m_iNumSectors);
			}
			for(size_t i = 0; i < m_arrSectors.Count(); i++){
				SectorDesc *Desc = m_arrSectors[i];
				wxString sClr = wxString::Format(wxT("wxColour(%d, %d, %d)"), Desc->colour.Red(),  Desc->colour.Green(), Desc->colour.Blue());
#if wxCHECK_VERSION(2, 9, 0)
				Codef(_T("\t%ASetSectorColor(%d, %s);\n"), i, sClr.wx_str());
#else
				Codef(_T("\t%ASetSectorColor(%d, %s);\n"), i, sClr.c_str());
#endif
			}
			if(!m_bShowVal){
				Codef(_T("%ADrawCurrent(false);\n"));
			}
			wxString ss = m_cdNeedleColour.BuildCode(GetCoderContext());
#if wxCHECK_VERSION(2, 9, 0)
			if(!ss.IsEmpty()) Codef(_T("%ASetNeedleColour(%s);\n"), ss.wx_str());
#else
			if(!ss.IsEmpty()) Codef(_T("%ASetNeedleColour(%s);\n"), ss.c_str());
#endif
			ss = m_cdBackgroundColour.BuildCode(GetCoderContext());
#if wxCHECK_VERSION(2, 9, 0)
			if(!ss.IsEmpty()) Codef(_T("%ASetBackColour(%s);\n"), ss.wx_str());
#else
			if(!ss.IsEmpty()) Codef(_T("%ASetBackColour(%s);\n"), ss.c_str());
#endif
			ss = m_cdBorderColour.BuildCode(GetCoderContext());
#if wxCHECK_VERSION(2, 9, 0)
			if(!ss.IsEmpty()) Codef(_T("%ASetBorderColour(%s);\n"), ss.wx_str());
#else
			if(!ss.IsEmpty()) Codef(_T("%ASetBorderColour(%s);\n"), ss.c_str());
#endif
			ss = GetCoderContext()->GetUniqueName(_T("AngularMeterFont"));
			wxString sFnt = m_fnt.BuildFontCode(ss, GetCoderContext());
			if(sFnt.Len() > 0)
			{
#if wxCHECK_VERSION(2, 9, 0)
				Codef(_T("%s"), sFnt.wx_str());
				Codef(_T("%ASetTxtFont(%s);\n"), ss.wx_str());
#else
				Codef(_T("%s"), sFnt.c_str());
				Codef(_T("%ASetTxtFont(%s);\n"), ss.c_str());
#endif
			}
			// Value needs to be set after other params for correct display.
			if(m_iValue){
				Codef(_T("%ASetValue(%d);\n"), m_iValue);
			}

			BuildSetupWindowCode();
			break;
		}
        default:
            wxsCodeMarks::Unknown(_T("wxsAngularMeter::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Build the code that creates the control preview.
 *
 * \param parent wxWindow*	The parent window.
 * \param flags long					Flags used when creating the preview.
 * \return wxObject						The control preview object.
 *
 */
wxObject *wxsAngularMeter::OnBuildPreview(wxWindow *parent, long flags)
{
    kwxAngularMeter *preview = new kwxAngularMeter(parent, GetId(), Pos(parent), Size(parent));

    if(m_iNumTicks > 0){
        preview->SetNumTick(m_iNumTicks);
    }
    // Default range is 0-220.
    if(m_iRangeMin != 0 || m_iRangeMax != 220){
        preview->SetRange(m_iRangeMin, m_iRangeMax);
    }
    // Default angles are -20, 200.
    if(m_iAngleMin != -20 || m_iAngleMax != 200){
        preview->SetAngle(m_iAngleMin, m_iAngleMax);
    }
	m_iNumSectors = m_arrSectors.Count();
    // Default number of sectors is 1.
    if(m_iNumSectors > 1){
        preview->SetNumSectors(m_iNumSectors);
    }
	for(size_t i = 0; i < m_arrSectors.Count(); i++){
		SectorDesc *Desc = m_arrSectors[i];
		preview->SetSectorColor(i, Desc->colour);
	}
    if(!m_bShowVal){
    	preview->DrawCurrent(false);
    }
    wxColour cc = m_cdNeedleColour.GetColour();
    if(cc.IsOk()){
        preview->SetNeedleColour(cc);
    }
    cc = m_cdBackgroundColour.GetColour();
    if(cc.IsOk()){
        preview->SetBackColour(cc);
    }
    cc = m_cdBorderColour.GetColour();
    if(cc.IsOk()){
        preview->SetBorderColour(cc);
    }
    wxFont fnt = m_fnt.BuildFont();
    if(fnt.IsOk()){
        preview->SetTxtFont(fnt);
    }
    // Value needs to be set after other params for correct display.
    if(m_iValue){
        preview->SetValue(m_iValue);
    }

    return SetupWindow(preview, flags);
}

/*! \brief Enumerate the control's custom properties.
 *
 * \param Flags long	Flags used when creating the control.
 * \return void
 *
 */
void wxsAngularMeter::OnEnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsAngularMeter, m_iValue, _("Value"), _T("value"), 0)
    WXS_BOOL(wxsAngularMeter, m_bShowVal, _("Show Value"), _T("show_value"), true)
    WXS_LONG(wxsAngularMeter, m_iNumTicks, _("Number Of Ticks"), _T("num_ticks"), 0)
    WXS_LONG(wxsAngularMeter, m_iRangeMin, _("Range Minimum"), _T("range_min"), 0)
    WXS_LONG(wxsAngularMeter, m_iRangeMax, _("Range Maximum"), _T("range_max"), 220)
    WXS_LONG(wxsAngularMeter, m_iAngleMin, _("Angle Minimum"), _T("angle_min"), -20)
    WXS_LONG(wxsAngularMeter, m_iAngleMax, _("Angle Maximum"), _T("angle_max"), 200)
    WXS_COLOUR(wxsAngularMeter, m_cdNeedleColour, _("Needle Colour"), _T("needle_colour"))
    WXS_COLOUR(wxsAngularMeter, m_cdBorderColour, _("Border Colour"), _T("border_colour"))
    WXS_COLOUR(wxsAngularMeter, m_cdBackgroundColour, _("Background Colour"), _T("background_colour"))
    WXS_FONT(wxsAngularMeter, m_fnt, _("Font"), _T("font"))
}

// Dynamic properties.
//=================
/*! \brief Add extra control properties.
 *
 * \param Grid wxsPropertyGridManager*	A PropertyGridManager object.
 * \return void
 *
 */
void wxsAngularMeter::OnAddExtraProperties(wxsPropertyGridManager *Grid)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    m_SectorCountId =  Grid->GetGrid()->Insert(_("Needle Colour"), NEW_IN_WXPG14X wxIntProperty(_("Number Of Sectors"), wxPG_LABEL, (int)m_arrSectors.Count()));
    for(int i = 0; i < (int)m_arrSectors.Count(); i++){
        InsertPropertyForSector(Grid, i);
    }
    wxsWidget::OnAddExtraProperties(Grid);
}

/*! \brief One of the control's extra properties changed.
 *
 * \param Grid 	wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 		wxPGId										The property's ID.
 * \return void
 *
 */
void wxsAngularMeter::OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId id)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    if(id == m_SectorCountId){
        int OldValue = (int)m_arrSectors.Count();
        int NewValue = Grid->GetPropertyValueAsInt(id);

        // There must be at least one sector.
        if(NewValue < 1){
            NewValue = 1;
            Grid->SetPropertyValue(id, NewValue);
        }

        if(NewValue > OldValue){
            // We have to generate new entries
            for(int i = OldValue; i < NewValue; i++){
                m_arrSectors.Add(new SectorDesc());
                // Default to white.
                m_arrSectors[i]->colour = *wxWHITE;
                InsertPropertyForSector(Grid, i);
            }
        }
        else if(NewValue < OldValue){
            // We have to remove some entries
            for(int i = NewValue; i < OldValue; i++){
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_arrSectors[i]->id);
#else
                Grid->Delete(m_arrSectors[i]->id);
#endif
                delete m_arrSectors[i];
            }

            m_arrSectors.RemoveAt(NewValue, OldValue - NewValue);
        }

        NotifyPropertyChange(true);
        return;
    }

    for(int i = 0; i < (int)m_arrSectors.Count(); i++){
        if(HandleChangeInSector(Grid, id, i)) return;
    }
    wxsWidget::OnExtraPropertyChanged(Grid, id);
}

/*! \brief Read XML control data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 	bool					Whether this is an XRC file.
 * \param IsExtra 	bool					Whether the data is extra information not conforming to the XRC standard.
 * \return bool								Success or failure.
 *
 */
bool wxsAngularMeter::OnXmlRead(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0; i < m_arrSectors.Count(); i++){
        delete m_arrSectors[i];
    }
    m_arrSectors.Clear();

	int i = 1;
	while(1){
		wxString s = wxString::Format(wxT("sector_%d_colour"), i);
		TiXmlElement *SectorElem = Element->FirstChildElement(s.mb_str());
		if(!SectorElem){
			break;
		}

		SectorDesc *Desc = new SectorDesc;
		wxString sClr = wxString(SectorElem->GetText(), wxConvUTF8);
		sClr.Remove(0, 1);
		long iClr;
		sClr.ToLong(&iClr, 16);
		Desc->colour = wxColour((unsigned char) ((iClr & 0xFF0000) >> 16) ,
											  (unsigned char) ((iClr & 0x00FF00) >> 8),
											  (unsigned char) ((iClr & 0x0000FF)));
		m_arrSectors.Add(Desc);

		i++;
	}

    return wxsWidget::OnXmlRead(Element, IsXRC, IsExtra);
}

/*! \brief Write XML data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 	bool					Whether this is an XRC file.
 * \param IsExtra 	bool					Whether the data is extra information not conforming to the XRC standard.
 * \return bool								Success or failure.
 *
 */
bool wxsAngularMeter::OnXmlWrite(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0;i < m_arrSectors.Count();i++){
        SectorDesc *Desc = m_arrSectors[i];
        wxString s = wxString::Format(wxT("sector_%d_colour"), i + 1);
        TiXmlElement *msg = new TiXmlElement(s.mb_str());
		wxString sClrHex = wxString::Format(_T("#%02X%02X%02X"),
																(unsigned int)Desc->colour.Red(),
																(unsigned int)Desc->colour.Green(),
																(unsigned int)Desc->colour.Blue());
        msg->LinkEndChild(new TiXmlText(sClrHex.mb_str()));
		Element->LinkEndChild(msg);
    }

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

/*! \brief	Insert a new sector colour property.
 *
 * \param Grid 		wxsPropertyGridManager*	A PropertyGridManager object.
 * \param Position 	int									The position of this item in the sector array.
 * \return void
 *
 */
void wxsAngularMeter::InsertPropertyForSector(wxsPropertyGridManager *Grid, int Position)
{
    SectorDesc *Desc = m_arrSectors[Position];
    wxString SectorName = wxString::Format(_("Sector %d Colour"), Position + 1);
    wxString SectorDataName = wxString::Format(_("sector_%d_colour"), Position + 1);

	Desc->id = Grid->GetGrid()->Insert(_("Needle Colour"), NEW_IN_WXPG14X wxSystemColourProperty(SectorName, wxPG_LABEL, Desc->colour));

//    WXS_COLOUR(wxsAngularMeter, m_arrSectors[Position], SectorName, SectorDataName)

//    Desc->id = Grid->Append(wxsColourProperty(SectorName, SectorDataName, wxsOFFSET(wxsAngularMeter, m_arrSectors[Position])));
//		wxsColourProperty(
//            const wxString& PGName,
//            const wxString& DataName,
//            long ValueOffset,
//            int Priority=100);
}

/*! \brief Check whether a sector colour property changed.
 *
 * \param Grid 			wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 				wxPGId										The property's ID.
 * \param Position 	int												The position of this item in the tag array.
 * \return bool			True if a change was recorded, false otherwise.
 *
 */
bool wxsAngularMeter::HandleChangeInSector(wxsPropertyGridManager *Grid, wxPGId id, int Position)
{
    SectorDesc *Desc = m_arrSectors[Position];
    bool Changed = false;
//    bool Global = id == Desc->id;

//    if(Global)
	if(Desc->id == id){

#if wxCHECK_PROPGRID_VERSION(1, 3, 0)
		wxString sPropType = Grid->GetPropertyValueType(id);
		if(sPropType.IsSameAs(wxT("wxColourPropertyValue"))){
#else
		if(Grid->IsPropertyValueType(id, CLASSINFO(wxColourPropertyValue))){
#endif
			wxColourPropertyValue* pcolval = wxDynamicCast(Grid->GetPropertyValueAsWxObjectPtr(id), wxColourPropertyValue);
			Desc->colour = pcolval->m_colour;
		}
        Changed = true;
    }

    if(Changed){
        NotifyPropertyChange(true);
        return true;
    }

    return false;
}
