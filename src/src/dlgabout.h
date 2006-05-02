#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <wx/dialog.h> // inheriting class's header file

class wxWindow;

class dlgAbout : public wxDialog
{
	public:
		// class constructor
		dlgAbout(wxWindow* parent);
		// class destructor
		~dlgAbout();
};

#endif // DLGABOUT_H
