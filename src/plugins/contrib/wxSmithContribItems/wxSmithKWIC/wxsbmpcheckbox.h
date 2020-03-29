/**************************************************************************//**
 * \file			wxsbmpcheckbox.h
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
#ifndef WXSBMPCHECKBOX_H
#define WXSBMPCHECKBOX_H

#include "wxwidgets/wxswidget.h"


class wxsBmpCheckbox : public wxsWidget
{
	public:
		wxsBmpCheckbox(wxsItemResData* Data);
		virtual ~wxsBmpCheckbox();
	protected:
	private:
        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

		wxsBitmapData			m_bdOn;					//!< The bitmap shown when the control is checked.
		wxsBitmapData			m_bdOff;					//!< The bitmap shown when the control is unchecked.
		wxsBitmapData			m_bdOnSel;				//!< The bitmap shown when the control is checked and the mouse is over it.
		wxsBitmapData			m_bdOffSel;				//!< The bitmap shown when the control is unchecked and the mouse is over it.
		bool								m_bBorder;				//!< Whether to show the control border.
		int									m_iBorderStyle;		//!< The border style.
		bool								m_bChecked;			//!< Whether the control is checked or not.
};

#endif // WXSBMPCHECKBOX_H
