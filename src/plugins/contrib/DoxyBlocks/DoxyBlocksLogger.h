/**************************************************************************//**
 * \file      DoxyBlocksLogger.h
 * \author    Gary Harris
 * \date      27/3/10
 *
 * DoxyBlocks - doxygen integration for Code::Blocks. \n
 * Copyright (C) 2010 Gary Harris.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef DOXYBLOCKSLOGGER_H_INCLUDED
#define DOXYBLOCKSLOGGER_H_INCLUDED

#include <loggers.h>

// forward decls
class wxBoxSizer;
class wxPanel;
class wxWindow;

const int ID_LOG_DOXYBLOCKS =  wxID_HIGHEST + 9060; //!< The all-important control ID.

/*! \brief DoxyBlocks log window class.
 *
 * All of this is just so I can obtain an ID and respond to mouse clicks on URL strings in the log window. :-)
 */
class DoxyBlocksLogger : public TextCtrlLogger
{
    wxPanel    *panel; //!< The log's panel.
    wxBoxSizer *sizer; //!< The panel's sizer.
public:
    /*! \brief Constructor
     */
    DoxyBlocksLogger() : TextCtrlLogger(false), panel(0), sizer(0) {}

    void UpdateSettings();
    virtual wxWindow* CreateControl(wxWindow* parent);
    void OpenLink(long urlStart, long urlEnd, bool bUseInternalViewer);
};

#endif // DOXYBLOCKSLOGGER_H_INCLUDED
