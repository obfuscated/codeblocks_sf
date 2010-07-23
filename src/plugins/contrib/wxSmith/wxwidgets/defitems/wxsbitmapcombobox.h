/** \file wxsbitmapcombobox.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2010 Gary Harris
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef WXSBITMAPCOMBOBOX_H
#define WXSBITMAPCOMBOBOX_H

#include "../wxswidget.h"
#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY(wxsBitmapData, BmpComboBitmapDataArray);

/** \brief Class for wxsBitmapComboBox widget */
class wxsBitmapComboBox: public wxsWidget
{
    public:

        wxsBitmapComboBox(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        virtual void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id);
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

		void UpdateArraySizes(int size);

        wxArrayString 									m_arrChoices;			//!< Array of entries for the choice list.
        long 													m_defaultSelection;	//!< The item selected by default.
		BmpComboBitmapDataArray		m_arrBmps;					//!< Array of bitmaps for entries.

		WX_DEFINE_ARRAY(wxPGId, IdsArray);							//!< Define an array of property IDs.

		int											m_iNumImages;						//!< The number of images.
		wxPGId     								m_idNumImages;					//!< The ID of the field containing the number of images.
		wxPGId     								m_idImages;							//!< The ID of the image parent field.
		wxArrayString						m_arrImagePaths;					//!< An array of paths to images.
		IdsArray   								m_arrBitmapIds;						//!< An array of IDs of the image path fields.
};

#endif
