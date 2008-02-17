/*
	This file is part of RubyIdbg, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: RubyIdbg.cpp 23 2007-12-21 16:51:03Z Pecan $
#ifndef TOOLBOX_H
#define TOOLBOX_H

class wxAuiManager;

// ----------------------------------------------------------------------------
class ToolBox
// ----------------------------------------------------------------------------
{
    public:
        ToolBox();
        ~ToolBox();
        wxWindow* FindWindowRecursively(const wxWindow* pwin, const wxEvtHandler* evtHandler, bool show);
        wxWindow* FindWindowRecursively(const wxWindow* parent, const wxString& partialLabel);
        wxWindow* FindWindowByEvtHandler(wxEvtHandler* evtHandler, bool show);
        wxWindow* WinExists(wxString partialLabel);

        wxWindow* ShowWindowsRecursively(const wxWindow* parent, int level);
        wxWindow* ShowWindowsAndEvtHandlers();
        void ShowWindowEvtHandlers(const wxWindow* win);
        bool SearchEvtHandlersFor(const wxWindow* win, const wxEvtHandler* evtHandler);
        wxEvtHandler* SearchEvtHandlersForClass( const wxString className);
        wxWindow* GetWindowFromAuiManager( wxString className );

    protected:
    private:
        wxAuiManager* m_pAuiMgr;
};



#endif // TOOLBOX_H
