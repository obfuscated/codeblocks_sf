#ifndef AUTODETECTCOMPILERS_H
#define AUTODETECTCOMPILERS_H

#include <wx/dialog.h>

class wxCommandEvent;
class wxUpdateUIEvent;

class AutoDetectCompilers : public wxDialog
{
	public:
		AutoDetectCompilers(wxWindow* parent);
		virtual ~AutoDetectCompilers();
		
		void OnDefaultClick(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        DECLARE_EVENT_TABLE()
};

#endif // AUTODETECTCOMPILERS_H
