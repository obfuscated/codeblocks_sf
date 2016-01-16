/**************************************************************************//**
 * \file			wxsangularmeter.h
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
#ifndef WXSANGULARMETER_H
#define WXSANGULARMETER_H

#include "wxwidgets/wxswidget.h"


class wxsAngularMeter : public wxsWidget
{
	public:
		wxsAngularMeter(wxsItemResData* Data);
		virtual ~wxsAngularMeter();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

		long						m_iValue;						//!< The meter's initial value.
		long						m_iNumTicks;				//!< The meter's number of ticks.
		bool						m_bShowVal;				//!< Whether to show the current value.
		long						m_iRangeMin;				//!< The meter's minimum range value.
		long						m_iRangeMax;				//!< The meter's maximum range value.
		long						m_iAngleMin;				//!< The meter's minimum angle value.
		long						m_iAngleMax;				//!< The meter's maximum angle value.
		long						m_iNumSectors;			//!< The meter's number of sectors.
		wxsColourData	m_cdNeedleColour;			//!< The colour that the pointer will have.
		wxsColourData	m_cdBorderColour;			//!< The meter's border colour.
		wxsColourData	m_cdBackgroundColour;	//!< The meter's background colour.
		wxsFontData		m_fnt;								//!< The meter's display font.

        // Dynamic properties.
        /*! \brief Sector enum.
         */
        struct SectorDesc{
            wxPGId id;											//!< Tag property ID.
			wxColour	colour;									//!< Tag value.
//			wxsColourData	cd;
        };
        WX_DEFINE_ARRAY(SectorDesc*, SectorArray);

        SectorArray m_arrSectors;							//!< Array of sector description structs.
        wxPGId m_SectorCountId;							//!< The ID of the sector count property.

        void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId id);
        bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
		void InsertPropertyForSector(wxsPropertyGridManager* Grid,int Position);
		bool HandleChangeInSector(wxsPropertyGridManager* Grid,wxPGId id,int Position);

};

#endif // WXSANGULARMETER_H
