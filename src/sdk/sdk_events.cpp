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
* $Id$
* $Date$
*/

#include "sdk_precomp.h"
#include "sdk_events.h"

IMPLEMENT_DYNAMIC_CLASS(CodeBlocksEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(CodeBlocksDockEvent, wxEvent)

// app events
DEFINE_CB_EVENT_TYPE(cbEVT_APP_STARTUP_DONE)
DEFINE_CB_EVENT_TYPE(cbEVT_APP_START_SHUTDOWN)
// plugin events
DEFINE_CB_EVENT_TYPE(cbEVT_PLUGIN_ATTACHED)
DEFINE_CB_EVENT_TYPE(cbEVT_PLUGIN_RELEASED)
// editor events
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_CLOSE)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_OPEN)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_ACTIVATED)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_DEACTIVATED)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_SAVE)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_AUTOCOMPLETE)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_CALLTIP)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_TOOLTIP)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_TOOLTIP_CANCEL)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_USERLIST_SELECTION)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_ADD)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_EDIT)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_BREAKPOINT_DELETE)
DEFINE_CB_EVENT_TYPE(cbEVT_EDITOR_UPDATE_UI)
// project events
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_CLOSE)
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_OPEN)
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_SAVE)
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_ACTIVATE)
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_FILE_ADDED)
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_FILE_REMOVED)
DEFINE_CB_EVENT_TYPE(cbEVT_PROJECT_POPUP_MENU)
// pipedprocess events
DEFINE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_STDOUT)
DEFINE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_STDERR)
DEFINE_CB_EVENT_TYPE(cbEVT_PIPEDPROCESS_TERMINATED)
// thread-pool events
DEFINE_CB_EVENT_TYPE(cbEVT_THREADTASK_STARTED)
DEFINE_CB_EVENT_TYPE(cbEVT_THREADTASK_ENDED)
DEFINE_CB_EVENT_TYPE(cbEVT_THREADTASK_ALLDONE)
// request app to dock/undock a window
DEFINE_CB_EVENT_TYPE(cbEVT_ADD_DOCK_WINDOW)
DEFINE_CB_EVENT_TYPE(cbEVT_REMOVE_DOCK_WINDOW)
DEFINE_CB_EVENT_TYPE(cbEVT_SHOW_DOCK_WINDOW)
