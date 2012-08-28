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

#ifndef WXSFLAGS_H
#define WXSFLAGS_H

/** \brief Flags used for property filtering, while generating code and while generating preview */
namespace wxsFlags
{
    // Property filtering flags
    const long flVariable  = 0x0000001;  ///< \brief Item is using variable
    const long flId        = 0x0000002;  ///< \brief Item is using identifier
    const long flPosition  = 0x0000004;  ///< \brief Item is using position
    const long flSize      = 0x0000008;  ///< \brief Item is using size
    const long flEnabled   = 0x0000010;  ///< \brief Item is using Enabled property
    const long flFocused   = 0x0000020;  ///< \brief Item is using Focused property
    const long flHidden    = 0x0000040;  ///< \brief Item is using Hidden property
    const long flColours   = 0x0000080;  ///< \brief Item is using colour properties (Fg and Bg)
    const long flToolTip   = 0x0000100;  ///< \brief Item is using tooltips
    const long flFont      = 0x0000200;  ///< \brief Item is using font
    const long flHelpText  = 0x0000400;  ///< \brief Item is using help text
    const long flSubclass  = 0x0000800;  ///< \brief Item is using subclassing
    const long flMinMaxSize= 0x0001000;  ///< \brief Item is using SetMinSize / SetMaxSize functions
    const long flExtraCode = 0x0002000;  ///< \brief Item is using extra item initialization code
    const long flValidator = 0x0004000;  ///< \brief Item is using specific validator (not wxDefaultValidator)

    const long flFile      = 0x8000000;  ///< \brief Edition in file mode
    const long flSource    = 0x4000000;  ///< \brief Edition in source mode
    const long flMixed     = 0x2000000;  ///< \brief Edition in mixed mode
    const long flFwdDeclar = 0x1000000;  ///< \brief Using forward declarations where possible
    const long flPchFilter = 0x0800000;  ///< \brief Use filtering of PCH files (include some headers only when there's no PCH)
    const long flPointer   = 0x0400000;  ///< \brief Flag set when current item is as pointer
    const long flLocal     = 0x0200000;  ///< \brief Flag set when current item is used only locally while building resource's content (is not member of class)
    const long flRoot      = 0x0100000;  ///< \brief Flag set when current item is root item of resource

    const long flWidget    = flVariable
                           | flId
                           | flPosition
                           | flSize
                           | flEnabled
                           | flFocused
                           | flHidden
                           | flColours
                           | flToolTip
                           | flFont
                           | flHelpText
                           | flSubclass
                           | flMinMaxSize
                           | flExtraCode
                           | flValidator; ///< \brief Default properties flags used by widgets for convenience

    const long flContainer = flVariable
                           | flId
                           | flPosition
                           | flSize
                           | flEnabled
                           | flFocused
                           | flHidden
                           | flColours
                           | flToolTip
                           | flFont
                           | flHelpText
                           | flSubclass
                           | flMinMaxSize
                           | flExtraCode; ///< \brief Default properties flags used by container widgets for convenience

    // Preview flags
    const long pfExact     = 0x0000001;  ///< \brief Notify to create exact preview (without any editor-like goodies)

    // Header flags
    const long hfLocal     = 0x0000001;  ///< \brief Header is needed only locally (while manually generating resource)
    const long hfInPCH     = 0x0000002;  ///< \brief This header has been included in default pch file
    const long hfForbidFwd = 0x0000004;  ///< \brief This header (when included in resource's header file) can not be replaced by forward declaration
}

#endif
