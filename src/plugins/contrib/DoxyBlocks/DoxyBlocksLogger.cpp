/**************************************************************************//**
 * \file      DoxyBlocksLogger.cpp
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
#include "DoxyBlocksLogger.h"

#include <cbplugin.h>
#include <pluginmanager.h>
#include <prep.h>

#include <wx/sizer.h>

/*! \brief Update the window's settings.
 *
 * \return void
 *
 */
void DoxyBlocksLogger::UpdateSettings()
{
    TextCtrlLogger::UpdateSettings();
}

/*! \brief Create the control and assign a known ID.
 *
 * \param parent wxWindow*    The parent window.
 * \return virtual wxWindow*    The newly created log window.
 *
 */
wxWindow* DoxyBlocksLogger::CreateControl(wxWindow* parent)
{
    panel = new wxPanel(parent);

    TextCtrlLogger::CreateControl(panel);
    control->SetId(ID_LOG_DOXYBLOCKS);

    sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(control, 1, wxEXPAND, 0);
    panel->SetSizer(sizer);

    return panel;
}

/*! \brief Open a URL that was clicked in the log window.
 *
 * \param urlStart               long    The starting position of the URL string in the line of text.
 * \param urlEnd                 long    The ending position of the URL string in the line of text.
 * \param bUseInternalViewer     bool    Whether to use the internal HTML viewer.
 * \return void
 *
 */
void DoxyBlocksLogger::OpenLink(long urlStart, long urlEnd, bool bUseInternalViewer)
{
    if(!control){
        return;
    }
    wxString url = control->GetRange(urlStart, urlEnd);
    if (platform::windows && url.StartsWith(_T("file://"))){
        url.Remove(0, 7);
    }
    if(bUseInternalViewer){
        cbMimePlugin* p = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(url);
        p->OpenFile(url);
    }
    else{
        wxLaunchDefaultBrowser(url);
    }
}
