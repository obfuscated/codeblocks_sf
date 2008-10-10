#ifndef INCREMENTALSEARCHCONFDLG_H
#define INCREMENTALSEARCHCONFDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"

//(*Headers(IncrementalSearchConfDlg)
//*)

class IncrementalSearchConfDlg : public cbConfigurationPanel
{
	public:

		IncrementalSearchConfDlg(wxWindow* parent);
		~IncrementalSearchConfDlg();

	private:

      wxString GetTitle() const { return _("Incremental search settings"); }
      wxString GetBitmapBaseName() const { return _("incsearch"); }
      void OnApply(){SaveSettings();}
      void OnCancel(){}
      void OnChooseColour(wxCommandEvent& event);
      void SaveSettings();

		DECLARE_EVENT_TABLE()
};

#endif // INCREMENTALSEARCHCONFDLG_H
