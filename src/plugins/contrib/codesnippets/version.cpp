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
// RCS-ID: $Id: version.cpp 71 2007-04-28 21:13:28Z Pecan $

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
//  Commit  1.1.54 2007/04/3
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
//          50) Fixed Bug: mis-shown/mis-placed view. View Snippets. Open Edit. Drag Snippet floating wind.
//              It turns transparent and stays that way. Close Edit. View Closes but Menu is still active.
//              View Snippets. Dock is now coverering CodeBlocks bec dock now has codeblocks size in .ini.
//              Looks like wxAUI bug. Saying window is shown when it's not.
//          51) Fixed Bug: When window is docked, we're recording CodeBlocks posn & size
//          52) Removed initial Linux floating window to avoid wxAUI menu bug.(cf."Other" category)
//          53) Updated codesnippets.cbp, readme.html, codesnippets.am
//          54) Fix unix error. Add const to use of wxWX2MBbuf cbU2C(const wxString& str);
//  Commit  1.1.55 2007/04/10
//          55) Missing event.Skip() in OnIdle() caused debugger to hang.
//  Commit  1.1.56 2007/04/12
//          56) Avoid file changed dialog when .xml does not exist
// ----------------------------------------------------------------------------
//  Commit  1.1.72 2007/04/13
//          55) Fix:App: Not finding MSW .XML & .ini file when CodeSnippets in camel case
//          56) Initial conversion of editor dlg to EditSnippetsFrame() and wxCode editor
//          57) SetMarginWidth according to max num lines in file/snippet
//          58) Add OnScroll routines to correct horizontal scroll errors
//          59) Add Editor menu items FindNext, FindPrev, correct saveFile logic
//          60) Added context menu Copy/Paste of Snippets/Categories to any items.
//          61) Plg: Placement: Center top left corner of child over parent.
//          62) another const for snippetdlg
//          63) Add ".trash" facility for snippet removal
//          64) double click invokes edit, shift double click invokes apply
//          65) Corrected messageBox for Edit use.
//          66) EditSnippetFrame(): reset the undo history to avoid undoing to a blank page
//          67) CenterChildOnParent() moved to editConfig()
//          68) CenterChildOnParent() FindReplace dialog
//          69) "56)" commit above, Edit::Replace, Edit::GoTo, Edit::MatchBrace, messageBoxs, SetDeaultItem
//          70) ::wxMkDir(m_ConfigFolder) if it doesn't exist. CB bug.
//              cf:http://forums.codeblocks.org/index.php?topic=5619.0;topicseen
//          71) Place editor dlg windows (save file warning etc) over Snippet window
//          72) Guard GetFileModificationTime() with if wxFileExists()
//              Correct spelling of /dev/null in linux code
// ----------------------------------------------------------------------------
//  Commit  1.1.73 2007/04/13
//          73) Block re-entry to code via wxCloseEvent(s) and IsTreeBusy()
// ----------------------------------------------------------------------------
//  Commit  1.1.75 2007/04/14
//          74 ) Remove AMD64 ambiguity on finddlg wxComboBox call
//          75 ) Again: Remove AMD64 gentoo ambiguity on finddlg wxComboBox call
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
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//  Commit  1.2.03 2007/04/17
//          01) Add multi-edit session support
//          02) Add DragnDrop of File Links to/from CodeBlocks (MSW)
//          03) cascade subsequent edit windows, else they stack.
//          04-09 reserved for fixes
// ----------------------------------------------------------------------------
//  Commit  1.2.15 2007/04/20
//          10) changed some wxMessageBox(s) to messageBox
//          11) If file already open in editor, just focus it. (use wxTreeItemId)
//          12) messageBox(s) identify themselves as CodeSnippets msgs
//          13) Prevent window size of 0,0
//          14) Prevent multiple instances of CodeSnippets
//          15) Add DragnDrop of File Links to/from CodeBlocks (GTK)
//          16 - 19) reserved for fixes
// ----------------------------------------------------------------------------
//  Commit  1.2.29 2007/04/23
//          20) Removed old editSnippetDlg class
//          21) Enabled Properties edit cut/copy/paste/undo/redo/selectAll
//          22) Fixed: can't force close/switch when menu item active
//          23) Add Index File backup facility
//          24) stahta01 changes 2007/4/21 for wxGTK2.8
//          25) Add ".trash" option to delete external file (ask to delete file)
//          26) Corrected messageBox placement for floating vs docked window (again)
//          27) Fixed Termination/OnActivate crash
//          28) Fixed bug on Linux, the executable folder wx being reported incorrectly
//          29-32) reserved
// ----------------------------------------------------------------------------
//  Commit  1.2.42 2007/04/27
//          33) Add dragscrolling options to settingsdlg() & snippetsconfig()
//          34) snippetsconfig() globally manages all config items for all classes
//          35) Fixes to allow docked window to restart at  previous position
//          35) More hacking to work around wxAUI docked/floating windows deficiencies
//          35) Added settings preference for window type & mouse scrolling
//          36) Corrections for linux settings preferences
//          37) Corrected path for codesnippets; add wxLearner gtkx.h includes for *nix
//          38) Fixes to OnUpdateUI routine when switching between window types
//          39) Verfied wx2.8.3 fixed closing Docked window crash
//          40) Added wxProcess::Exists() to check on external snippets window
//          41) Fixed wx2.8.3 RightClick not selecting tree item
//          42) Fixed external snippet storage folder not being recorded
//          43-49) reserved
// ----------------------------------------------------------------------------
//  Commit  1.2.51 2007/04/28
//          50) On startup, wait for user to open external window with view/snippets menu
//          51) Fixed CB fullscreen editor focus. Make editors modal while they have focus.
//          52-55) reserved
// ----------------------------------------------------------------------------
//  Commit  1.2.60
//          56) Restore statusbar text on idle
//          57) Pass independent window a keepAlive Pid. ping it to save data.
//          58) Add plgn communications to independent windows via memory mapped file
//          59) Clean up temp memory mapped file on termination
// ----------------------------------------------------------------------------
//  ToDo    All
//          Hide/show search box
//          a renamed "new snippet" label is not sorted into place
//              Maybe it shouldn't be?
//          Help should invoke browser for wiki article
//          Update wiki for Re-arranging Tree items/multi-editing/.trash
//              backup, wrapper execution, DragnDrop
//          Plg: Floating Tree pops over editor when moving mouse into CB window. Very Annoying.
//              Because wxAUI uses wxMiniFrame with stay-on-top, else it'll get lost
//              since miniFrames aren't placed on the task bar.
//          "XML load error" doesn't pop up when loading a "txt" file. Why?
//          MSW path may already have ";" tacked to end. What happens?
//          "File has changed" msg pops up even when file has NOT changed.
//          Add option for ToolTips and max chars shown
//          #ifdef out linux options dlg spacer (can't with wxFormBuilder)
//              Maybe just a line instead of a spacer will work
//          Verify:If independent snippets, plg shouldnt save conf OR xml
//          Since making editors modal, have to move into the editor, then out
//              again to re-enable Snippets tree. Even External win does this.
//              Try going modal ONLY if CB is in full screen.
//          Closing the Floating windoe while editor is modal freezes CB.
//              Couldn't re-create. But OnClose should un-modal all wins.
//  Other
//          Bug: Nasty wxAUI Linux behavior. Left chick on CB main menu docks floating windows.
//               Floating window won't dock when dragged to CB. Have to left click some main menu item.
//          Wierd but Ok Department: Dragging a file within .trash asks to "Delete file?"
//              But who'll do that? Because OnEndTreeItemDrag() calls RemoveItem() from .trash;
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
