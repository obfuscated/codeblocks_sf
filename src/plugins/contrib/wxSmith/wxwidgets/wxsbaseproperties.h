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

#ifndef WXSBASEPROPERTIES_H
#define WXSBASEPROPERTIES_H

#include "wxsexproperties.h"
#include "../wxsadvqpp.h"
#include "../wxscodinglang.h"

/** \brief Structure holding most commonly used properties
 *
 * This class doesn't support style since it require too many information
 * from base class.
 *
 * \note All availability flags used inside this class are declared in wxsItem
 */
class wxsBaseProperties: public wxsPropertyContainer
{
    public:

        bool            m_ParentFromArg;   ///< \brief Use parent from constructor's argument
        wxString        m_VarName;         ///< \brief Variable name
        bool            m_IsMember;        ///< \brief Switch between local and global variable
        wxString        m_IdName;          ///< \brief Name of identifier
        bool            m_IdFromArg;       ///< \brief Set to true if we should consider taking id from constructor's argument
        wxString        m_Subclass;        ///< \brief Subclass (class used instead of original base class)
        wxsPositionData m_Position;        ///< \brief Data for position in case of custom value
        bool            m_PositionFromArg; ///< \brief If set to true, we should consider taking position from constructor's argument
        wxsSizeData     m_Size;            ///< \brief Data for size in case of custom value
        bool            m_SizeFromArg;     ///< \brief If set to true, we should consider taking position from constructor's argument
        wxString        m_Validator;       ///< \brief Specific validator, setup by the user (not wxDefaultValidator)
        wxsSizeData     m_MinSize;         ///< \brief Mininum size (not available in XRC)
        wxsSizeData     m_MaxSize;         ///< \brief Maximum size (not available in XRC)
        bool            m_Enabled;         ///< \brief If false, widget is disabled (true by default)
        bool            m_Focused;         ///< \brief If true, widget is focused (false by default)
        bool            m_Hidden;          ///< \brief If true, widget is hidden (false by default)
        wxsColourData   m_Fg;              ///< \brief Foreground colour when using custom colour
        wxsColourData   m_Bg;              ///< \brief Background colour when using custom colour
        wxsFontData     m_Font;            ///< \brief Font
        wxString        m_ToolTip;         ///< \brief Tooltip
        wxString        m_HelpText;        ///< \brief Help text
        wxString        m_ExtraCode;       ///< \brief User's extra item initialization code

        // TODO: Convert these to long long
        long m_StyleBits;                  ///< \brief Internal representation of styles as bit field
        long m_ExStyleBits;                ///< \brief Internal representation of extra styles as bit field
        const wxsStyleSet* m_StyleSet;     ///< \brief Styleset used in style operations

        /** \brief Constructor - it will be used to set default values */
        wxsBaseProperties():
            m_ParentFromArg(true),
            m_IsMember(true),
            m_IdName(wxEmptyString),
            m_IdFromArg(false),
            m_PositionFromArg(false),
            m_SizeFromArg(false),
            m_Validator(wxEmptyString),
            m_Enabled(true),
            m_Focused(false),
            m_Hidden(false),
            m_Fg(wxsCOLOUR_DEFAULT),
            m_Bg(wxsCOLOUR_DEFAULT),
            m_ToolTip(wxEmptyString),
            m_HelpText(wxEmptyString),
            m_ExtraCode(wxEmptyString),
            m_StyleBits(0),
            m_ExStyleBits(0),
            m_StyleSet(0)
        {}

        /** \brief Function generating QPPChild Panel for most frequently used
         *         properties.
         */
        void AddQPPChild(wxsAdvQPP* QPP,long Flags);

        /** \brief Adding some special xml read of data */
        void SpecialXmlRead(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Adding some special xml write of data */
        void SpecialXmlWrite(TiXmlElement* Elem,bool IsXRC,bool IsExtra,long Flags,const wxString& ClassName);

    protected:

        virtual void OnEnumProperties(long Flags);
};

#endif
