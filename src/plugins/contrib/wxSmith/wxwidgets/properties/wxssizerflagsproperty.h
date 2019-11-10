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

#ifndef WXSSIZERFLAGSPROPERTY_H
#define WXSSIZERFLAGSPROPERTY_H

#include "../../properties/wxsproperties.h"

/** \brief Property responsible for editing sizer flags;
 *
 * Sizer flags are: border flags, alignment and other flags: expand, shaped,
 * fixed min size.
 *
 * \note This property uses standard names for property grid / data.
 */
class wxsSizerFlagsProperty: public wxsProperty
{
    public:

        static const long BorderTop             = 0x00001;
        static const long BorderBottom          = 0x00002;
        static const long BorderLeft            = 0x00004;
        static const long BorderRight           = 0x00008;
        static const long BorderAll             = 0x00010;
        static const long BorderPrevAll         = 0x00020;
        static const long Expand                = 0x00040;
        static const long Shaped                = 0x00080;
        static const long FixedMinSize          = 0x00100;
        static const long AlignLeft             = 0x00200;
        static const long AlignRight            = 0x00400;
        static const long AlignTop              = 0x00800;
        static const long AlignBottom           = 0x01000;
        static const long AlignCenterVertical   = 0x02000;
        static const long AlignCenterHorizontal = 0x04000;
        static const long AlignNot              = 0x08000;
        static const long ParentAlignVertical   = 0x10000;
        static const long ParentAlignHorizontal = 0x20000;

        /** \brief Ctor
         *  \param Offset   offset to long handling border flags
         *  \param Priority         priority of this property
         */
        wxsSizerFlagsProperty(long Offset,int Priority);

        /** \brief Returning type of this property */
        virtual const wxString GetTypeName() { return _T("SizerFlags"); }

        /** \brief Getting string representation of flags */
        static wxString GetString(long Flags);

        /** \brief Getting wxWidgets-ready flags */
        static long GetWxFlags(long Flags);

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

    private:

        static long ParseString(const wxString& String);
        static void FixFlags(long& Flags);

        long GetParentOrientation(TiXmlElement* Element);

        long Offset;

        static const long BorderMask      = BorderTop|BorderBottom|BorderLeft|BorderRight;
        static const long AlignHMask      = AlignLeft|AlignRight|AlignCenterHorizontal;
        static const long AlignVMask      = AlignTop|AlignBottom|AlignCenterVertical;
        static const long AlignCMask      = AlignCenterHorizontal|AlignCenterVertical;
        static const long ParentAlignMask = ParentAlignVertical|ParentAlignHorizontal;
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring sizer flags property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of long integer variable inside class used to keep flags
 *  \param Priority priority of this property
 */
#define WXS_SIZERFLAGS_P(ClassName,VarName,Priority) \
    { static wxsSizerFlagsProperty _Property(wxsOFFSET(ClassName,VarName),Priority); \
      Property(_Property); }

/** \} */

#endif
