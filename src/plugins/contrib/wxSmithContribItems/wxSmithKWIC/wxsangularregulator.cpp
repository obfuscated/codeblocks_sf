/**************************************************************************//**
 * \file		wxsangularregulator.cpp
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
#include "wxsangularregulator.h"
#include "wx/KWIC/AngularRegulator.h"

namespace
{
    // Loading images from xpm files
#include "images/angreg16.xpm"
#include "images/angreg32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsAngularRegulator> Reg(
        _T("kwxAngularRegulator"),                     			// Class name
        wxsTWidget,                            								// Item type
        _T("KWIC License"),                       					// License
        _T("Andrea V. & Marco Cavallini"),               	// Author
        _T("m.cavallini@koansoftware.com"),  			// Author's email
        _T("http://www.koansoftware.com/kwic/"),    	// Item's homepage
        _T("KWIC"),                         									// Category in palette
        90,                                    										// Priority in palette
        _T("AngularRegulator"),                           			// Base part of names for new items
        wxsCPP,                                								// List of coding languages supported by this item
        1, 0,                                  										// Version
        wxBitmap(angreg32_xpm),               				// 32x32 bitmap
        wxBitmap(angreg16_xpm),               				// 16x16 bitmap
        true);                                										// We do not allow this item inside XRC files

    WXS_EV_BEGIN(wxsAngularRegulatorEvents)
        WXS_EVI(EVT_ANGULARREG_CHANGED, kwxEVT_ANGREG_CHANGE, wxCommandEvent, Changed)
    WXS_EV_END()
}

/*! \brief Constructor.
 *
 * \param Data wxsItemResData*	Pointer to a resource data object.
 *
 */
wxsAngularRegulator::wxsAngularRegulator(wxsItemResData *Data) :
    wxsWidget(
        Data,
        &Reg.Info,
        wxsAngularRegulatorEvents,
        NULL,
        flVariable | flId | flPosition | flSize | flEnabled | flFocused | flHidden | flToolTip | flHelpText | flSubclass | flMinMaxSize),
    m_iValue(0),
    m_iRangeMin(0),
    m_iRangeMax(220),
    m_iAngleMin(-20),
    m_iAngleMax(200)
{
}

/*! \brief Destructor.
 */
wxsAngularRegulator::~wxsAngularRegulator()
{
    for(size_t i = 0;i < m_arrTags.Count();i++){
        delete m_arrTags[i];
    }
    m_arrTags.Clear();
}

/*! \brief Build the code that creates the control.
 *
 * \return void
 *
 */
void wxsAngularRegulator::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
		{
			AddHeader(_T("\"wx/KWIC/AngularRegulator.h\""), GetInfo().ClassName);
			Codef(_T("%C(%W,%I,%P,%S, %s);\n"), wxT("wxBORDER_NONE"));

			Codef(_T("%ASetRange(%d, %d);\n"), m_iRangeMin, m_iRangeMax);
			Codef(_T("%ASetAngle(%d, %d);\n"), m_iAngleMin, m_iAngleMax);
			wxString ss = m_cdExternalCircleColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()) Codef(_T("%ASetExtCircleColour(%s);\n"), ss.wx_str());
			ss = m_cdInternalCircleColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()) Codef(_T("%ASetIntCircleColour(%s);\n"), ss.wx_str());
			ss = m_cdKnobBorderColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()) Codef(_T("%ASetKnobBorderColour(%s);\n"), ss.wx_str());
			ss = m_cdKnobColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()) Codef(_T("%ASetKnobColour(%s);\n"), ss.wx_str());
			ss = m_cdLimitTextColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()) Codef(_T("%ASetLimitsColour(%s);\n"), ss.wx_str());
			ss = m_cdTagColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()) Codef(_T("%ASetTagsColour(%s);\n"), ss.wx_str());
			for(size_t i = 0; i < m_arrTags.Count(); i++){
				TagDesc *Desc = m_arrTags[i];
				Codef(_T("\t%AAddTag(%d);\n"), Desc->val);
			}
			// Value needs to be set after other params for correct display and, in this case,
			// should always be set to ensure that the knob is drawn at the correct location.
			// If the value is not set the knob is drawn in the centre of the control.
			Codef(_T("%ASetValue(%d);\n"), m_iValue);

			BuildSetupWindowCode();
			break;
		}
        default:
            wxsCodeMarks::Unknown(_T("wxsAngularRegulator::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Build the code that creates the control preview.
 *
 * \param parent wxWindow*	The parent window.
 * \param flags long					Flags used when creating the preview.
 * \return wxObject						The control preview object.
 *
 */
wxObject *wxsAngularRegulator::OnBuildPreview(wxWindow *parent, long flags)
{
    kwxAngularRegulator *preview = new kwxAngularRegulator(parent, GetId(), Pos(parent), Size(parent), wxBORDER_NONE);

	preview->SetRange(m_iRangeMin, m_iRangeMax);
	preview->SetAngle(m_iAngleMin, m_iAngleMax);
    wxColour clr = m_cdExternalCircleColour.GetColour();
    if(clr.IsOk()){
        preview->SetExtCircleColour(clr);
    }
    clr = m_cdInternalCircleColour.GetColour();
    if(clr.IsOk()){
        preview->SetIntCircleColour(clr);
    }
    clr = m_cdKnobBorderColour.GetColour();
    if(clr.IsOk()){
        preview->SetKnobBorderColour(clr);
    }
    clr = m_cdKnobColour.GetColour();
    if(clr.IsOk()){
        preview->SetKnobColour(clr);
    }
    clr = m_cdLimitTextColour.GetColour();
    if(clr.IsOk()){
        preview->SetLimitsColour(clr);
    }
    clr = m_cdTagColour.GetColour();
    if(clr.IsOk()){
        preview->SetTagsColour(clr);
    }
	for(size_t i = 0; i < m_arrTags.Count(); i++){
		TagDesc *Desc = m_arrTags[i];
		preview->AddTag(Desc->val);
	}
    // Value needs to be set after other params for correct display and, in this case,
    // should always be set to ensure that the knob is drawn at the correct location.
    // If the value is not set the knob is drawn in the centre of the control.
	preview->SetValue(m_iValue);

    return SetupWindow(preview, flags);
}

/*! \brief Enumerate the control's custom properties.
 *
 * \param Flags long	Flags used when creating the control.
 * \return void
 *
 */
void wxsAngularRegulator::OnEnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsAngularRegulator, m_iValue, _("Value"), _T("value"), 0)
    WXS_LONG(wxsAngularRegulator, m_iRangeMin, _("Range Minimum"), _T("range_min"), 0)
    WXS_LONG(wxsAngularRegulator, m_iRangeMax, _("Range Maximum"), _T("range_max"), 220)
    WXS_LONG(wxsAngularRegulator, m_iAngleMin, _("Angle Minimum"), _T("angle_min"), -20)
    WXS_LONG(wxsAngularRegulator, m_iAngleMax, _("Angle Maximum"), _T("angle_max"), 200)
    WXS_COLOUR(wxsAngularRegulator, m_cdExternalCircleColour, _("External Circle Colour"), _T("external_circlecolour"))
    WXS_COLOUR(wxsAngularRegulator, m_cdInternalCircleColour, _("Internal Circle Colour"), _T("internal_circle_colour"))
    WXS_COLOUR(wxsAngularRegulator, m_cdKnobBorderColour, _("Knob Border Colour"), _T("knob_border_colour"))
    WXS_COLOUR(wxsAngularRegulator, m_cdKnobColour, _("Knob Colour"), _T("knob_colour"))
    WXS_COLOUR(wxsAngularRegulator, m_cdLimitTextColour, _("Range Tag Colour"), _T("range_tag_colour"))
    WXS_COLOUR(wxsAngularRegulator, m_cdTagColour, _("Tag Colour"), _T("tag_colour"))
}

// Dynamic properties.
//=================
/*! \brief Add extra control properties.
 *
 * \param Grid wxsPropertyGridManager*	A PropertyGridManager object.
 * \return void
 *
 */
void wxsAngularRegulator::OnAddExtraProperties(wxsPropertyGridManager *Grid)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
	m_TagCountId = Grid->GetGrid()->Insert(_("External Circle Colour"), NEW_IN_WXPG14X wxIntProperty(_("Number Of Tags"), wxPG_LABEL, (int)m_arrTags.Count()));
    for(int i = 0;i < (int)m_arrTags.Count();i++){
        InsertPropertyForTag(Grid, i);
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
void wxsAngularRegulator::OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId id)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    if(id == m_TagCountId){
        int OldValue = (int)m_arrTags.Count();
        int NewValue = Grid->GetPropertyValueAsInt(id);

        if(NewValue < 0){
            NewValue = 0;
            Grid->SetPropertyValue(id, NewValue);
        }

        if(NewValue > OldValue){
            // We have to generate new entries
            for(int i = OldValue;i < NewValue;i++){
                m_arrTags.Add(new TagDesc());
                InsertPropertyForTag(Grid, i);
            }
        }
        else if(NewValue < OldValue){
            // We have to remove some entries
            for(int i = NewValue;i < OldValue;i++){
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_arrTags[i]->id);
#else
                Grid->Delete(m_arrTags[i]->id);
#endif
                delete m_arrTags[i];
            }

            m_arrTags.RemoveAt(NewValue, OldValue - NewValue);
        }

        NotifyPropertyChange(true);
        return;
    }

    for(int i = 0;i < (int)m_arrTags.Count();i++){
        if(HandleChangeInTag(Grid, id, i)) return;
    }
    wxsWidget::OnExtraPropertyChanged(Grid, id);
}

/*! \brief Read XML control data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 		bool						Whether this is an XRC file.
 * \param IsExtra 		bool						Whether the data is extra information not conforming to the XRC standard.
 * \return bool											Success or failure.
 *
 */
bool wxsAngularRegulator::OnXmlRead(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0;i < m_arrTags.Count();i++){
        delete m_arrTags[i];
    }
    m_arrTags.Clear();

	int i = 1;
	while(1){
		wxString s = wxString::Format(wxT("tag_%d_value"), i);
		TiXmlElement *TagElem = Element->FirstChildElement(s.mb_str());
		if(!TagElem){
			break;
		}

		TagDesc *Desc = new TagDesc;
		long iVal;
		wxString sVal(TagElem->GetText(), wxConvUTF8);
		sVal.ToLong(&iVal);
		Desc->val = iVal;
		m_arrTags.Add(Desc);
		i++;
	}

    return wxsWidget::OnXmlRead(Element, IsXRC, IsExtra);
}

/*! \brief Write XML data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 		bool						Whether this is an XRC file.
 * \param IsExtra 		bool						Whether the data is extra information not conforming to the XRC standard.
 * \return bool											Success or failure.
 *
 */
bool wxsAngularRegulator::OnXmlWrite(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0;i < m_arrTags.Count();i++){
        TagDesc *Desc = m_arrTags[i];
        wxString s = wxString::Format(wxT("tag_%d_value"), i + 1);
        TiXmlElement *msg = new TiXmlElement(s.mb_str());
        msg->LinkEndChild(new TiXmlText(wxString::Format(wxT("%d"), Desc->val).mb_str()));
		Element->LinkEndChild(msg);
    }

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

/*! \brief	Insert a new tag value property.
 *
 * \param Grid 			wxsPropertyGridManager*	A PropertyGridManager object.
 * \param Position 	int												The position of this item in the tag array.
 * \return void
 *
 */
void wxsAngularRegulator::InsertPropertyForTag(wxsPropertyGridManager *Grid, int Position)
{
    TagDesc *Desc = m_arrTags[Position];
    wxString sTagName = wxString::Format(_("Tag %d Value"), Position + 1);

	Desc->id = Grid->GetGrid()->Insert(_("External Circle Colour"), NEW_IN_WXPG14X wxIntProperty(sTagName, wxPG_LABEL, Desc->val));
}

/*! \brief Check whether a tag value property changed.
 *
 * \param Grid 			wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 				wxPGId										The property's ID.
 * \param Position 	int												The position of this item in the tag array.
 * \return bool			True if a change was recorded, false otherwise.
 *
 */
bool wxsAngularRegulator::HandleChangeInTag(wxsPropertyGridManager *Grid, wxPGId id, int Position)
{
    TagDesc *Desc = m_arrTags[Position];
    bool Changed = false;
//    bool Global = id == Desc->id;

//    if(Global)
	if(Desc->id == id){
        Desc->val = Grid->GetPropertyValueAsInt(id);
        Changed = true;
    }

    if(Changed){
        NotifyPropertyChange(true);
        return true;
    }

    return false;
}

