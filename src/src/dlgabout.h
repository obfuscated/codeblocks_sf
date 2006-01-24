#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dialog.h> // inheriting class's header file
#include <wx/xrc/xmlres.h>

class dlgAbout : public wxDialog
{
	public:
		// class constructor
		dlgAbout(wxWindow* parent);
		// class destructor
		~dlgAbout();
};

#endif // DLGABOUT_H
