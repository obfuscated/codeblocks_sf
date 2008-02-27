/*
	This file is part of Browse Tracker, a plugin for Code::Blocks
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
// RCS-ID: $Id: BrowseTracker.cpp 22 2007-12-18 14:41:58Z Pecan $
#ifndef BROWSETRACKERCFG_H
#define BROWSETRACKERCFG_H

#include "ConfigDlg.h"
class wxWindow;
class wxFileConfig;

// ----------------------------------------------------------------------------
class BrowseTrackerCfg
// ----------------------------------------------------------------------------
{

    // Shell class for wxFormBuilder generated CfgDlg.cpp/h
    public:
        BrowseTrackerCfg();
        virtual ~BrowseTrackerCfg();
        void GetUserOptions( wxString cfgFullPath);
        void ReadUserOptions(wxString configFullPath);
        void SaveUserOptions(wxString configFullPath);


    ConfigDlg m_configDlg;

    wxFileConfig* pCfgFile;
    int m_BrowseMarksEnabled;
    int m_UserMarksStyle;
    int m_ToggleKey;
	int m_LeftMouseDelay;
	int m_ClearAllKey;

    protected:
    ConfigDlg* m_pConfigDlg;

    private:

};



#endif // BROWSETRACKERCFG_H
