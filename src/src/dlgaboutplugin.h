
#ifndef DLGABOUTPLUGIN_H
#define DLGABOUTPLUGIN_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dialog.h> // inheriting class's header file
#include <wx/xrc/xmlres.h>
#include "../sdk/cbplugin.h"

/*
 * No description
 */
class dlgAboutPlugin : public wxDialog
{
	public:
		// class constructor
		dlgAboutPlugin(wxWindow* parent, PluginInfo* pi);
		// class destructor
		~dlgAboutPlugin();
};

#endif // DLGABOUTPLUGIN_H
