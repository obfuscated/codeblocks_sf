/**************************************************************************//**
 * \file			wxslinearmeter.h
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
#ifndef WXSLINEARMETER_H
#define WXSLINEARMETER_H

#include "wxwidgets/wxswidget.h"


class wxsLinearMeter : public wxsWidget
{
	public:
		wxsLinearMeter(wxsItemResData* Data);
		virtual ~wxsLinearMeter();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

		long						m_iValue;								//!< The meter's initial value.
		long						m_iRangeMin;						//!< The meter's minimum range value.
		long						m_iRangeMax;					//!< The meter's maximum range value.
		bool						m_bHorizontal;					//!< The meter's orientation, horizontal or vertical.
		bool						m_bShowVal;						//!< Whether to show the current value.
		bool						m_bShowLimits;					//!< Whether to show the limit values.
		wxsColourData	m_cdActiveBarColour;		//!< The meter's bar colour.
		wxsColourData	m_cdPassiveBarColour;	//!< The meter's background colour.
		wxsColourData	m_cdBorderColour;			//!< The meter's border colour.
		wxsColourData	m_cdLimitTextColour;		//!< The colour that the limit value have.
		wxsColourData	m_cdValueTextColour;		//!< The meter's text colour.
		wxsColourData	m_cdTagColour;					//!< The meter's tag colour.
		wxsFontData		m_fnt;									//!< The meter's display font.

        // Dynamic properties.
        /*! \brief Tag value enum.
         */
        struct TagDesc{
            wxPGId 	id;														//!< Tag property ID.
            int			val;														//!< Tag value.
        };
        WX_DEFINE_ARRAY(TagDesc*, TagArray);

        TagArray m_arrTags;											//!< Array of tag description structs.
        wxPGId m_TagCountId;										//!< The ID of the tag count property.

        void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId id);
        bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
		void InsertPropertyForTag(wxsPropertyGridManager* Grid,int Position);
		bool HandleChangeInTag(wxsPropertyGridManager* Grid,wxPGId id,int Position);
};

#endif // WXSLINEARMETER_H
