/*
* This file is part of wxSmithFoldbar plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
*/

#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "wxsSmithSTC.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<wxsSmithSTC> reg(_T("wxsSmithSTC"));
}



// constructor
wxsSmithSTC::wxsSmithSTC()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    /*
    if(!Manager::LoadResource(_T("wxSmithContribItems.zip")))
    {
        NotifyMissingFile(_T("wxSmithContribItems.zip"));
    }
    */
}

// destructor
wxsSmithSTC::~wxsSmithSTC()
{
}

void wxsSmithSTC::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void wxsSmithSTC::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}
