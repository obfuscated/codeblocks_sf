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
// RCS-ID: $Id: version.cpp 26 2008-01-03 17:34:18Z Pecan $

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif

#include "Version.h"

   #if LOGGING
	wxLogWindow*    m_pLog;
   #endif

// ----------------------------------------------------------------------------
AppVersion::AppVersion()
// ----------------------------------------------------------------------------
{
    //ctor
    m_version = VERSION;
}

// ----------------------------------------------------------------------------
AppVersion::~AppVersion()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
//  Commit  0.1.3 2007/06/2
//          01) Intial working Browse Tracker
//          02) Added ProjectLoadingHook to get around ProjectActivate and ProjectOpen event bugs
//              Dont store Editor ptrs while Project_Open event is occuring
//              BUG: CB EVT_PROJECT_ACTIVATE is occuring before EVT_PROJECT_OPEN
//              BUG: Editor Activate events occur before Project_Open and Project_Activated
//          03) Add Clear Menu item to clear recorded Editors
//          04) Navigation popup dialog window
//          05) create Makefile.am BrowseTracker-unix.cbp and .cbplugin(s)
//          06) changed "Opened tabs" to "Browsed Tabs"
//          07) Honor the --personality arg when finding .ini file
// ----------------------------------------------------------------------------
//  Commit  0.1.8 2007/07/11
//          08) changes for sdk RegisterEventSink
// ----------------------------------------------------------------------------
//  Commit  0.1.10 2007/08/2
//          09) fixes for editor activation changes caused by wxFlatNotebook 2.2
//          10) fix for non-focused dialog in wxGTK284
// ----------------------------------------------------------------------------
//  Commit  0.1.11 2007/08/29
//          11) fixes for sdk 1.11.12. SDK removed Manger::GetAppWindow()
// ----------------------------------------------------------------------------
//          0.1.12 2007/11/18
//          11) use OnIdle to focus the new activated editor. Else its visible but dead.
//              SDK.editormanager::OnUpdateUI used to do this, but now it's gone.
//  commit  1.2.1 2007/11/27
//          1) implement browse markers within each editor. Cursor position is memorized
//             one per page on each left mouse key "up". User can force the browse mark
//             by holding ctrl+left-mouse-up. (Alt-mouse-up) seems to be hogged by the
//             frame manager.
//             Alt+Up & Alt+Dn keys cycle through the memorized BrowseMarks
// ----------------------------------------------------------------------------
//  commit  1.2.3 2007/11/29
//          2) re-add previous browse mark clobbered by copy/paste so user
//             can (most likely) return to paste insertion area.
//          3) fixed editor activation when multiple projects close
// ----------------------------------------------------------------------------
//  commit  1.2.5 2007/11/30
//          4) correct case of some filenames
//          5) Add menu items "Set BrowseMark", "Clear BrowseMark"
// ----------------------------------------------------------------------------
//  commit  1.2.15 2007/12/4
//          6) remove redundancy of initial editor BrowseMark
//          7) record "previous position" forward; allowing easy copy/paste operations
//          8) add "Clear All BrowseMarks" menu item for a single editor
//          9) don't show navigation dlg when no active cbEditors
//         11) Clear previous marks on line when setting a new one (unless Ctrl held)
//         12) when current browsemark off screen, go to "current", not "previous/next"
//         13) save BrowseMarks in circular queue. More understandable interface.
//         14) switch Ctrl-LeftMouse click to delete browse marks on line.
//         15) allow multiple browse marks on page, but only one per line
// ----------------------------------------------------------------------------
//  commit 1.2.24 2007/12/8
//         16) ignore recording BrowseMarks on duplicate editor activations
//             eg. activations from double clicking search results panel
//         17) fixed GetCurrentScreenPositions() when doc less than screen size
//         18) honor WXK_RETURN to dismiss selector dialog
//         19) honor WXK_RETURN to dismiss wxListBox in dialog
//         20) convert editor pointers to circular queue
//         21) compress active browsed editor array for better availability
//         22) correct active editor order when using selection dlg
//         23) fix index overflow crash in BrowseMarks.cpp::ClearMarks()
//         24) implement visable browseMarks as "..."
// ----------------------------------------------------------------------------
//  commit  1.2.29 2007/12/9
//          25) add sort and "Sort BrowseMarks" menu  command
//          26) On ClearAllMarkers(), tell scintilla to also clear visible BrowseMarks.
//          27) shadow scintilla BrowseMarks when lines added/deleted; keeps marks in user set order
//          28) append BrowseTracker menu to the context popup menu
//          29) remove OnEditorActivated() code causing linux to re-set deleted markers
// ----------------------------------------------------------------------------
//  Commit  1.2.44 2007/12/16
//          30) Ctrl+LeftMouse click on non-marked line clears *all* markers;
//              Ctrl+LeftMouse click on marked line clears the line markers;
//          31) Added BrowseTracker ProjectData class
//          32) Added BrowseTrackerLayout class
//          33) Fixed: Crash when no project and file opened at StartUp
//          34) Persistent Browse_Marks vis Load/Save layout file.
//          35) Restore BrowseMarks when a previously active editor closed/reopened
//          36) Toggle BrowseMark with Ctrl+LeftMouseClick
//          37) Convert BookMarks class to use full file names as index to save
//              Browse/Book marks of unactivated files in project
//          39) Persistent Book_Marks via Load/Save Layout file "<projectName>.bmarks"
//          39) Fix: Browse/Book marks not saved when fileShortName used
//          40) Save Browse/Book marks when CB exited w/o closing projects
//          41) Ignore Browse/Book marks for files outside projects in EVT_EDITOR_CLOSE
//          42) Mouse must be down 250 millisecs to toggle BrowseMark. Better control.
//          43) Ignore Left-Double-clicks that read like a long left click.
//          44) Fix non-pch includes for linux
// ----------------------------------------------------------------------------
//  Commit  1.2.48 2007/12/17
//          45) Fixed layout loading in multi-project workspace
//          46) Fixed frozen dialog popup in multi-project workspace
//          47) Decoupled ProjectData from dependency on EditorBase indexes.
//          48) Added Search for ProjectData/cbProject by fullPath
// ----------------------------------------------------------------------------
//  Commit  1.2.49 2007/12/21
//          49) Add Settings dialog and support for Ctrl-Left_Mouse toggling
//              and Ctrl-Left_Mouse Double click clearAll.
//              See Menu/Browse Tracker/Settings
//          50) Changed Cfg.cpp labels back to default colors
// ----------------------------------------------------------------------------
//  Commit 1.2.57 2008/01/2
//         51) Added: user request: Hidden BrowseMarks
//         52) Fixed: Editor selection dialog stalls after "Clear All"
//         53) Added: User choice of BrowseMarks, BookMarks, or HiddenMarks
//         54) Fixed: Don't set BrowseMark on doubleclick
//         55) On clear all: Also clear BookMarks when being used as BrowseMarks
//         56) Remove use of LoadingProjectHook since it's not called if project
//              file has no xml "extensions" node.
//         57) Remove use of IsLoading() since SDK actually turns it OFF during
//              editor loading and activation.
// ----------------------------------------------------------------------------
//  Commit 1.2.58 2008/01/2
//         58) #include for cbStyledTextCtrl.h CB refactoring
// ----------------------------------------------------------------------------
//  Commit 1.2.64 2008/01/4
//         59) update Makefile.am
//         60) Re-instated use of ProjectLoadingHook. Hook is skipped when extra </unit>
//              in project.cbp after file adds. Re-saving project.cbp solves problem.
//         61) Finish code to shadow Book marks as BrowseMark.
//         62) Refactored ClearLineBrowseMarks/ClearLineBookMarks/ToggleBook_Mark
//         63) Don't initiate a BrowseMark on initial load of source file.
//         64) Correct linux cbstyledtextctrl.h spelling
// ----------------------------------------------------------------------------
//  Commit 1.2.66 2008/01/13
//         65) Use m_bProjectIsLoading in OnProjectOpened() to avoid scanning editors
//         66) Don't set marks on mouse drags
//         66) Release ProjectLoadingHook in OnRelease()
// ----------------------------------------------------------------------------
//  Commit 1.2.68 2008/01/24
//         67) guard against null project pointer when importing projects
//             Sdk is issueing project events without a project pointer.
//             Eg. importing Visual Studio Solution project
//         68) OnProjectOpen: turn off ProjectLoading flag when no project pointer
// ----------------------------------------------------------------------------
//  Commit 1.2.70 2008/02/13
//         69) Fixed: Setting a BookMark via the margin context menu is not seen by BT 2008/01/25
//         70) rework sizers on settings dlg
// ----------------------------------------------------------------------------
//  Bugs
//          When searching workspace w/o open project, BT is comatose.
//              Should say "no project" or some such.
//          Requires CB to be restarted after Install before Alt-Left/Right work.
//          When CB reloads a changed editor, the marks are missing
//          In one fell swoop: uninstall BrowseTracker, reInstall it,
//              click on a project. CB::OnProjectHook call crashes.
// ----------------------------------------------------------------------------
//  ToDo    All
//          Config dialog: Max tracked editors Max tracked lines etc
//          Navigation toolbar arrows
//          Shadow the menuitem cmdkey definitions w/ wxMenuItem->GetAccel()
//          Selection/history of Marks via dlg
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
