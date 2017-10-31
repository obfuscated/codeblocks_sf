/**************************************************************************//**
 * \file			wxslcddisplay.h
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
#ifndef WXSLCDDISPLAY_H
#define WXSLCDDISPLAY_H

#include "wxwidgets/wxswidget.h"


class wxsLCDDisplay : public wxsWidget
{
	public:
		wxsLCDDisplay(wxsItemResData* Data);
		virtual ~wxsLCDDisplay();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

		long						m_iNumDigits;			//!< The number of digits displayed.
		wxsColourData	m_cdLightColour;		//!< The colour which the lighted parts of the display should have.
		wxsColourData	m_cdGrayColour;		//!< The colour that the greyed parts of the display will have.
		wxString				m_sValue;					//!< The display's initial value.
};

#endif // WXSLCDDISPLAY_H
