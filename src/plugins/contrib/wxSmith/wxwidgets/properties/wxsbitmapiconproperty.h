/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSBITMAPICONPROPERTY_H
#define WXSBITMAPICONPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../wxscodercontext.h"

#include <wx/artprov.h>

/** \brief Structure holding bitmap configuration
 *
 * Id determines whether we use art provider or not,
 * if both Id and FileName are empty, no bitmap is given
 */

struct wxsBitmapIconData
{
    wxString Id;
    wxString Client;
    wxString FileName;
    wxString CodeText;

    /** \brief Generating preview for this bitmap / icon */
    wxBitmap GetPreview(const wxSize& Size,const wxString& DefaultClient = wxART_OTHER);

    /** \brief Generating code building this bitmap / icon.
     *
     * Empty code is returned when there's no valid bitmap.
     */
    wxString BuildCode(bool NoResize,const wxString& SizeCode,wxsCoderContext* Context,const wxString& DefaultClient = wxART_OTHER);

    /** \brief Checking if this is empty bitmap/icon */
    bool IsEmpty();
};

typedef wxsBitmapIconData wxsBitmapData;
typedef wxsBitmapIconData wxsIconData;

/** \brief Property for editing arrays of strings with checked option
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsBitmapIconProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor
         *  \param PGName           name of property in Property Grid
         *  \param DataName         name of property in data stuctures
         *  \param Offset           offset of wxsBitmapIconData structure (returned from wxsOFFSET macro)
         *  \param DefaultClient    name of default art provider client
         *  \param Priority         priority of this property
         */
		wxsBitmapIconProperty(
            const wxString& PGName,
            const wxString& DataName,
            long Offset,
            const wxString& DefaultClient = _T("wxART_OTHER"),
            int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxBitmapIcon"); }

        /** \brief Showing editor for this property */
        virtual bool ShowEditor(wxsPropertyContainer* Object);

    protected:

        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual wxString GetStr(wxsPropertyContainer* Object);

	private:

        long Offset;
        wxString DefaultClient;
};

typedef wxsBitmapIconProperty wxsBitmapProperty;
typedef wxsBitmapIconProperty wxsIconProperty;


/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring bitmap property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxBitmapData inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DefaultArtClient default art provider client (use _T("wxART_OTHER") if not sure)
 */
#define WXS_BITMAP(ClassName,VarName,PGName,DataName,DefaultArtClient) \
    { static wxsBitmapProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),DefaultArtClient); \
      Property(_Property); }

/** \brief Macro automatically declaring bitmap property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxBitmapData inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DefaultArtClient default art provider client (use _T("wxART_OTHER") if not sure)
 *  \param Priority priority of this property
 */
#define WXS_BITMAP_P(ClassName,VarName,PGName,DataName,DefaultArtClient,Priority) \
    { static wxsBitmapProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),DefaultArtClient,Priority); \
      Property(_Property); }

/** \brief Macro automatically declaring icon property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxIconData inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DefaultArtClient default art provider client (use _T("wxART_OTHER") if not sure)
 */
#define WXS_ICON(ClassName,VarName,PGName,DataName,DefaultArtClient) \
    { static wxsIconProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),DefaultArtClient); \
      Property(_Property); }

/** \brief Macro automatically declaring icon property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxIconData inside class
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DefaultArtClient default art provider client (use _T("wxART_OTHER") if not sure)
 *  \param Priority priority of this property
 */
#define WXS_ICON_P(ClassName,VarName,PGName,DataName,DefaultArtClient,Priority) \
    { static wxsIconProperty _Property(PGName,DataName,wxsOFFSET(ClassName,VarName),DefaultArtClient,Priority); \
      Property(_Property); }

/** \} */


#endif
