/** \file wxsprintdialog.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010  Gary Harris
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

#ifndef WXSPRINTDIALOG_H
#define WXSPRINTDIALOG_H

#include "../wxstool.h"

/** \brief Class for wxPrintDialog dialogue. */
class wxsPrintDialog: public wxsTool
{
    public:

        wxsPrintDialog(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

        bool        m_bEnableHelp;                //!< Enable the help button.
        bool        m_bEnablePageNumbers;    //!< Enable the page number block.
        bool        m_bEnablePrintToFile;        //!< Enable the print to file block.
        bool        m_bEnableSelection;            //!< Enable the selection block.
        bool        m_bCollate;                        //!< Whether to collate copies.
        bool        m_bPrintToFile;                    //!< Whether to print to file.
        bool        m_bSelection;                    //!< Whether to print the selection Results are implementation-dependent.
        long        m_iFromPage;                    //!< The page to print from.
        long        m_iToPage;                        //!< The page to print to.
        long        m_iMinPage;                        //!< The minimum page no. to print from.
        long        m_iMaxPage;                    //!< The maximum page no. to print to.
        long        m_iNoCopies;                    //!< The number of copies to print.
};

#endif
