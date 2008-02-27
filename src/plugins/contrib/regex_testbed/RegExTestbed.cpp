/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "RegExTestbed.h"
#include "regexdlg.h"

#ifndef CB_PRECOMP
    #include <globals.h>
#endif

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<RegExTestbed> reg(_T("RegExTestbed"));
}

// constructor
RegExTestbed::RegExTestbed()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("RegExTestbed.zip")))
    {
        NotifyMissingFile(_T("RegExTestbed.zip"));
    }
}

// destructor
RegExTestbed::~RegExTestbed()
{
}

void RegExTestbed::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void RegExTestbed::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int RegExTestbed::Execute()
{
    try
    {
        RegExDlg dlg(0, -1);
        PlaceWindow(&dlg);
        dlg.ShowModal();
    }
    catch (...)
    {
        cbMessageBox(_("A fatal error occured while creating the window..."),
                        _("Error"), wxICON_ERROR);
        return -1;
    }
	return 0;
}
