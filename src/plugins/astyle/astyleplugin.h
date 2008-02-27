/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ASTYLEPLUGIN_H
#define ASTYLEPLUGIN_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK

class AStylePlugin : public cbToolPlugin
{
  public:
    AStylePlugin();
    ~AStylePlugin();
    int Configure();
    int GetConfigurationGroup() const { return cgEditor; }
    cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
    int Execute();
    void OnAttach(); // fires when the plugin is attached to the application
    void OnRelease(bool appShutDown); // fires when the plugin is released from the application
};

#endif // ASTYLEPLUGIN_H
