/**************************************************************************//**
 * \file			wxsangularregulator.h
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
#ifndef WXSANGULARREGULATOR_H
#define WXSANGULARREGULATOR_H

#include "wxwidgets/wxswidget.h"


class wxsAngularRegulator : public wxsWidget
{
	public:
		wxsAngularRegulator(wxsItemResData* Data);
		virtual ~wxsAngularRegulator();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

		long						m_iValue;									//!< The regulator's initial value.
		long						m_iRangeMin;							//!< The regulator's minimum range value.
		long						m_iRangeMax;						//!< The regulator's maximum range value.
		long						m_iAngleMin;							//!< The regulator's minimum angle value.
		long						m_iAngleMax;							//!< The regulator's maximum angle value.
		wxsColourData	m_cdExternalCircleColour;	//!< The regulator's bar colour.
		wxsColourData	m_cdInternalCircleColour;	//!< The regulator's background colour.
		wxsColourData	m_cdKnobBorderColour;		//!< The regulator's border colour.
		wxsColourData	m_cdKnobColour;					//!< The regulator's border colour.
		wxsColourData	m_cdLimitTextColour;			//!< The colour of the limit values.
		wxsColourData	m_cdTagColour;						//!< The regulator's tag colour.

        // Dynamic properties.
        /*! \brief Tag value enum.
         */
        struct TagDesc{
            wxPGId 	id;															//!< Tag property ID.
            int			val;															//!< Tag value.
        };
        WX_DEFINE_ARRAY(TagDesc*, TagArray);

        TagArray m_arrTags;												//!< Array of tag description structs.
        wxPGId m_TagCountId;											//!< The ID of the tag count property.

        void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId id);
        bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
		void InsertPropertyForTag(wxsPropertyGridManager* Grid,int Position);
		bool HandleChangeInTag(wxsPropertyGridManager* Grid,wxPGId id,int Position);

};

#endif // WXSANGULARREGULATOR_H
