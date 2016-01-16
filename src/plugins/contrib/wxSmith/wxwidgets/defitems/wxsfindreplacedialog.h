/** \file wxsfindreplacedialog.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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

#ifndef WXSFINDREPLACEDIALOG_H
#define WXSFINDREPLACEDIALOG_H

#include "../wxstool.h"

/** \brief Class for wxFindReplaceDialog dialogue.
    \note In order for events to connect properly you must set the ID of your wxFindReplaceDialog to -1 in wxSmith.
*/
class wxsFindReplaceDialog: public wxsTool
{
    public:

        wxsFindReplaceDialog(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

        wxString m_sCaption;    //!< The dialogue caption.
};

#endif      //  WXSFINDREPLACEDIALOG_H
