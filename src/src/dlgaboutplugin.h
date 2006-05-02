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
