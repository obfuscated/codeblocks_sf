/**************************************************************************//**
 * \file			wxsbmpswitcher.h
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
#ifndef WXSBMPSWITCHER_H
#define WXSBMPSWITCHER_H

#include "wxwidgets/wxswidget.h"


class wxsBmpSwitcher : public wxsWidget
{
	public:
		wxsBmpSwitcher(wxsItemResData* Data);
		virtual ~wxsBmpSwitcher();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        // Dynamic properties.
        /*! \brief Bitmap enum.
         */
        struct BmpDesc{
            wxPGId 		id;														//!< Tag property ID.
            wxString	sPath;												//!< The path to the image file.
        };
        WX_DEFINE_ARRAY(BmpDesc*, BmpArray);

        long			m_iState;												//!< The control's state i.e. which bitmap is visible.
        BmpArray 	m_arrBmps;											//!< The array of bitmap description structs.
        wxPGId 		m_BmpCountId;									//!< The ID of the bitmap count property.

        void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId id);
        bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
		void InsertPropertyForBmp(wxsPropertyGridManager* Grid,int Position);
		bool HandleChangeInBmp(wxsPropertyGridManager* Grid,wxPGId id,int Position);
};

#endif // WXSBMPSWITCHER_H
