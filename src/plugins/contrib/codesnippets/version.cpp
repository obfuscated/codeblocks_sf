/*
	This file is part of Code Snippets, a plugin for Code::Blocks
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
// RCS-ID: $Id: version.cpp 33 2007-04-03 02:45:43Z Pecan $

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif

#include "version.h"

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
//  Commit  1.1.3   2007/03/20
//          01) Add version changelog & debug logging
//          02) Create local svn; add svn props and filter line endings
//          03) Allow running as app or plugin
// ----------------------------------------------------------------------------
//  Commit  1.1.10  2007/03/21
//           4) AddTextToClipboard() & hide "Apply" from App version context menu
//           5) Add portability
//           6) Open secondary windows over parent window (App version)
//           7) Added Settings Dlg to App version
//           8) Added Main menu file open/Save (App version)
//           9) Update About menu text (App version)
//          10) Added Properties to context menu
// ----------------------------------------------------------------------------
//  Commit  1.1.17 2007/03/22
//          11) Add property examination/edit
//          12) Unused "Description" field of properties removed
//          13) Add new Text & File icons
//          14) Show first level of items on startup
//			15) Honor target source line indentation
//			    wxLeaner: http://forums.codeblocks.org/index.php/topic,5375.new.html#new
//          16) Added WX_PRECOMP to all cpp files
//          17) Check for externally changed file
// ----------------------------------------------------------------------------
//  Commit  1.1.51 2007/04/2
//          18) change Version class to AppVersion to avoid CB conflict
//          19) Added check for externally changed file to plugin version
//          20) DeDocked & DeAllocated resources when Snippet window is closed
//          21) Generate .exe along side dll in CodeSnippets-Lmsw.cbp
//          22) Add DragnDrop Tree Items within ctrl and Item/category conversion
//          23) Added context menu "Convert to Category"
//          24) Add snippet external storage management (text & files)
//          25) Allow user to do Save as well as SaveAs
//          26) Added Drag inward to edit control
//          27) Fixed DragnDrop overwriting instead of inserting.
//          28) Changed property editor to wxScintill from wxTextCtrl
//          29) Corrected wxScintilla/Edit horizontal scroll length
//          30) Set scintilla defaults as close to CodeBlccks defaults as possible
//          31) Position child windows as close to parent as wxWidgets allows
//          32) Fixed edit dlg buttons to align center horizontal
//          33) Added basic dragscrolling scintilla editor
//          34) Made edit snippets dialog non-modal to allow dragNdrop/copy/cut/paste
//          35) Made properties dialog non-modal to allow dragNdrop/copy/cut/paste
//          36) Call RequestUserAttention() when app closing & edit|properties win open
//          37) Diaable View Menu when codesnippet modeless dlg in foreground
//          38) Moved EditSnippet() & EditProperties to CodeSnippetsTreeCtrl()
//          39) chenged Edit menu item to use default scintilla editor
//          40) save external file in EditSnippetDlg()
//          41) When using external CodeSnippets, EditorDlg Help button should not show
//          42) FileLinks hand off file to scintilla editor, but leave link info in properties
//          43) EditSnippet moved to CodeSnippetsTreeCtrl
//          44) Added About to RootID context menu
//          45) Fixed Bug: Search options not remembered
//          46) Memorize floating window position on Menu->View->Hide
//          47) Move floating window to last Menu->View->Hide position
//          48) Rearrange menu needs sorting for better sense
//          49) Place Edit dialog to last close position & size
//          50) Fized Bug: mis-shown/mis-placed view. View Snippets. Open Edit. Drag Snippet floating wind.
//              It turns transparent and stays that way. Close Edit. View Closes but Menu is still active.
//              View Snippets. Dock is now coverering CodeBlocks bec dock now has codeblocks size in .ini.
//              Looks like wxAUI bug. Saying window is shown when it's not.
//          51) Fixed Bug: When window is docked, we're recording CodeBlocks posn & size
//          52) Removed initial Linux floating window to avoid wxAUI menu bug.(cf."Other" category)
//          53) Updated codesnippets.cbp, readme.html, codesnippets.am
// ----------------------------------------------------------------------------
// Published Changes for Version 1.1:
//    Ability to save Snippets as external files
//    Ability to edit externally saved Snippet file
//    New Text vs File Link icons
//    Ability to Rearrange tree with Drag and Drop
//        Drag Snippet to Category
//        Drag Category to other Category
//        Drag Snippet to Snippet creates new Category
//        Drag Snippets/Categories outward and inward.
//    Convert To Category context menu facility
//    Added drag snippet item outward (MSW) w/ both text/file drag formats
//    Dragging snippet File Link to CB opens the file.
//    Changed text editor to Scintilla
//    Added DragScrolling to Editor
//    Allow Editor/Properties DragNDrop in both directions
//    Expand first level Snippets on Startup
//    Check for externally modified XML file
//    Free resources & save index when Snippet window closes
//    Portability
//    User specified window placement
//    User specified XML placement
//    User specified external editor
//    Save XML index file menu entry
//    About info and context menu entry
//    Snippet Properties Dialog
//    Work arounds to avoid wxAUI bugs on MSW/LINUX (cf. version.cpp)
//
// ----------------------------------------------------------------------------
//  ToDo    All
//          User should be reminded to save. On first exec, data is lost.
//          Allow Settings dlg to override all .ini locations
//          Bug: Minimizing edit dlg does not minimize, only moves it.
//          Hide/show search box
//          Bug: new snippet label is not sorted into place
//          Bug: Horizonal scrollBar doesnt update until pulled then released
//          Add dragscrolling configuration to settingsdlg & snippetsconfig
//          snippetsconfig.cpp should read/keep/save *all* config items for all classes
//          externally modified files w/default editor should warn user
//          Add ".Trash" facility for external file removal
//          Bug: crash in file modification detection routines
//              Open/view Open/app Close/view Close/App MsgBox then crash
//          Copy and paste whole catagories (for when out of view) like dragging them.
//          Add Ctrl-S save ability to Editor
//          Put file name beside "Name" when editing File Link
//          Option: double click invokes editor
//          Bug: on Linux, the executable dir is being reported incorrectly
//          Help should invoke browser for wiki article
//          Investigate Linux problem dragging item out of window (alternate X11 solution)
//  Todo    Plugin
//          Find work around for wxAUI floating window vs menu right click bug
//          Option to wxExecute CodeSnippets vs Docking Snippets
//  Other
//          Bug: Nasty wxAUI Linux behavior. Left chick on CB main menu docks floating windows.
//               Floating window won't dock when dragged to CB. Have to left click some main menu item.
// ----------------------------------------------------------------------------
//

