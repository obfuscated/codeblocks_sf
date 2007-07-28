/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "sdk_events.h"
    #include "cbproject.h"
    #include "editorbase.h"
    #include "cbplugin.h"
#endif


IMPLEMENT_DYNAMIC_CLASS(CodeBlocksEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(CodeBlocksDockEvent, wxEvent)

// app events
const wxEventType cbEVT_APP_STARTUP_DONE = wxNewEventType();
const wxEventType cbEVT_APP_START_SHUTDOWN = wxNewEventType();
const wxEventType cbEVT_APP_UPDATE_TITLE = wxNewEventType();
// plugin events
const wxEventType cbEVT_PLUGIN_ATTACHED = wxNewEventType();
const wxEventType cbEVT_PLUGIN_RELEASED = wxNewEventType();
const wxEventType cbEVT_PLUGIN_INSTALLED = wxNewEventType();
const wxEventType cbEVT_PLUGIN_UNINSTALLED = wxNewEventType();
// editor events
const wxEventType cbEVT_EDITOR_CLOSE = wxNewEventType();
const wxEventType cbEVT_EDITOR_OPEN = wxNewEventType();
const wxEventType cbEVT_EDITOR_ACTIVATED = wxNewEventType();
const wxEventType cbEVT_EDITOR_DEACTIVATED = wxNewEventType();
const wxEventType cbEVT_EDITOR_SAVE = wxNewEventType();
const wxEventType cbEVT_EDITOR_MODIFIED = wxNewEventType();
const wxEventType cbEVT_EDITOR_TOOLTIP = wxNewEventType();
const wxEventType cbEVT_EDITOR_TOOLTIP_CANCEL = wxNewEventType();
const wxEventType cbEVT_EDITOR_BREAKPOINT_ADD = wxNewEventType();
const wxEventType cbEVT_EDITOR_BREAKPOINT_EDIT = wxNewEventType();
const wxEventType cbEVT_EDITOR_BREAKPOINT_DELETE = wxNewEventType();
const wxEventType cbEVT_EDITOR_UPDATE_UI = wxNewEventType();
// project events
const wxEventType cbEVT_PROJECT_CLOSE = wxNewEventType();
const wxEventType cbEVT_PROJECT_OPEN = wxNewEventType();
const wxEventType cbEVT_PROJECT_SAVE = wxNewEventType();
const wxEventType cbEVT_PROJECT_ACTIVATE = wxNewEventType();
const wxEventType cbEVT_PROJECT_FILE_ADDED = wxNewEventType();
const wxEventType cbEVT_PROJECT_FILE_REMOVED = wxNewEventType();
const wxEventType cbEVT_PROJECT_POPUP_MENU = wxNewEventType();
const wxEventType cbEVT_PROJECT_TARGETS_MODIFIED = wxNewEventType();
const wxEventType cbEVT_PROJECT_RENAMED = wxNewEventType();
const wxEventType cbEVT_WORKSPACE_CHANGED = wxNewEventType();
// build targets events
const wxEventType cbEVT_BUILDTARGET_ADDED = wxNewEventType();
const wxEventType cbEVT_BUILDTARGET_REMOVED = wxNewEventType();
const wxEventType cbEVT_BUILDTARGET_RENAMED = wxNewEventType();
const wxEventType cbEVT_BUILDTARGET_SELECTED = wxNewEventType();
// pipedprocess events
const wxEventType cbEVT_PIPEDPROCESS_STDOUT = wxNewEventType();
const wxEventType cbEVT_PIPEDPROCESS_STDERR = wxNewEventType();
const wxEventType cbEVT_PIPEDPROCESS_TERMINATED = wxNewEventType();
// thread-pool events
const wxEventType cbEVT_THREADTASK_STARTED = wxNewEventType();
const wxEventType cbEVT_THREADTASK_ENDED = wxNewEventType();
const wxEventType cbEVT_THREADTASK_ALLDONE = wxNewEventType();
// request app to dock/undock a window
const wxEventType cbEVT_ADD_DOCK_WINDOW = wxNewEventType();
const wxEventType cbEVT_REMOVE_DOCK_WINDOW = wxNewEventType();
const wxEventType cbEVT_SHOW_DOCK_WINDOW = wxNewEventType();
const wxEventType cbEVT_HIDE_DOCK_WINDOW = wxNewEventType();
// request app to switch view layout
const wxEventType cbEVT_SWITCH_VIEW_LAYOUT = wxNewEventType();
// app notifies that a new layout has been applied
const wxEventType cbEVT_SWITCHED_VIEW_LAYOUT = wxNewEventType();
// app notifies that a docked window has been hidden/shown
const wxEventType cbEVT_DOCK_WINDOW_VISIBILITY = wxNewEventType();
// app notifies that the menubar is started being (re)created
const wxEventType cbEVT_MENUBAR_CREATE_BEGIN = wxNewEventType();
// app notifies that the menubar (re)creation ended
const wxEventType cbEVT_MENUBAR_CREATE_END = wxNewEventType();
// compiler-related events
const wxEventType cbEVT_COMPILER_STARTED = wxNewEventType();
const wxEventType cbEVT_COMPILER_FINISHED = wxNewEventType();
// debugger-related events
const wxEventType cbEVT_DEBUGGER_STARTED = wxNewEventType();
const wxEventType cbEVT_DEBUGGER_PAUSED = wxNewEventType();
const wxEventType cbEVT_DEBUGGER_FINISHED = wxNewEventType();
