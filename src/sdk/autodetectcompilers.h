#ifndef AUTODETECTCOMPILERS_H
#define AUTODETECTCOMPILERS_H

#include <wx/dialog.h>

class AutoDetectCompilers : public wxDialog
{
	public:
		AutoDetectCompilers(wxWindow* parent);
		virtual ~AutoDetectCompilers();
		
		void OnDefaultClick(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	protected:
	private:
        DECLARE_EVENT_TABLE()
};

#endif // AUTODETECTCOMPILERS_H
