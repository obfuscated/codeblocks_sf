/** \file wxsmessagedialog.h
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

#ifndef WXSMESSAGEDIALOG_H
#define WXSMESSAGEDIALOG_H

#include "../wxstool.h"

/** \brief Class for wxMessageDialog dialogue. */
class wxsMessageDialog: public wxsTool
{
    public:

        wxsMessageDialog(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

        wxString     m_sCaption;            //!< The dialogue's caption.
        wxString     m_sMessage;            //!< The dialogue message.
};

#endif      //  WXSMESSAGEDIALOG_H
