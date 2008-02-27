/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

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
