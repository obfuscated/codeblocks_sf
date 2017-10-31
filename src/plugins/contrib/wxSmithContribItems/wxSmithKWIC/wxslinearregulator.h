/**************************************************************************//**
 * \file			wxslinearregulator.h
 * \author	Gary Harris
 * \date		19/4/2010.
 *
 * This file is part of wxSmithKWIC.
 *
 * wxSmithKWIC - an add-on for wxSmith, Code::Blocks' GUI editor.					\n
 * Copyright (C) 2010 Gary Harris.
 *
 * wxSmithKWIC is free software: you can redistribute it and/or modify
 * it under the terms of the KWIC License.
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
#ifndef WXSLINEARREGULATOR_H
#define WXSLINEARREGULATOR_H

#include "wxwidgets/wxswidget.h"


class wxsLinearRegulator : public wxsWidget
{
	public:
		wxsLinearRegulator(wxsItemResData* Data);
		virtual ~wxsLinearRegulator();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

		long						m_iValue;								//!< The regulator's initial value.
		long						m_iRangeMin;						//!< The regulator's minimum range value.
		long						m_iRangeMax;					//!< The regulator's maximum range value.
		bool						m_bHorizontal;					//!< The regulator's orientation, horizontal or vertical.
		bool						m_bShowVal;						//!< Whether to show the current value.
		bool						m_bShowLimits;					//!< Whether to show the limit values.
		wxsColourData	m_cdActiveBarColour;		//!< The regulator's bar colour.
		wxsColourData	m_cdPassiveBarColour;	//!< The regulator's background colour.
		wxsColourData	m_cdBorderColour;			//!< The regulator's border colour.
		wxsColourData	m_cdLimitTextColour;		//!< The colour of the limit values.
		wxsColourData	m_cdValueTextColour;		//!< The regulator's text colour.
		wxsColourData	m_cdTagColour;					//!< The regulator's tag colour.
		wxsFontData		m_fnt;									//!< The regulator's display font.

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

#endif // WXSLINEARREGULATOR_H
