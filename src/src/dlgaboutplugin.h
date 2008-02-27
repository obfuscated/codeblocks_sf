/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DLGABOUTPLUGIN_H
#define DLGABOUTPLUGIN_H

#include <wx/dialog.h> // inheriting class's header file

struct PluginInfo;
class wxWindow;
/*
 * No description
 */
class dlgAboutPlugin : public wxDialog
{
	public:
		// class constructor
		dlgAboutPlugin(wxWindow* parent, const PluginInfo* pi);
		// class destructor
		~dlgAboutPlugin();
};

#endif // DLGABOUTPLUGIN_H
