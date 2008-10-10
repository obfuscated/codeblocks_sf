#ifndef INCREMENTALSEARCHCONFDLG_H
#define INCREMENTALSEARCHCONFDLG_H

#include <sdk.h> // Code::Blocks SDK
#include "configurationpanel.h"

#ifndef CB_PRECOMP
    #include <wx/wx.h>
#endif

//(*Headers(IncrementalSearchConfDlg)
//*)

class IncrementalSearchConfDlg : public cbConfigurationPanel
{
	public:

		IncrementalSearchConfDlg(wxWindow* parent);
		virtual ~IncrementalSearchConfDlg();

	protected:

      virtual wxString GetTitle() const { return _("Incremental search settings"); }
      virtual wxString GetBitmapBaseName() const { return _("incsearch"); }
      virtual void OnApply(){SaveSettings();};
      virtual void OnCancel(){}
      void OnChooseColour(wxCommandEvent& event);
      void SaveSettings();

	private:


		DECLARE_EVENT_TABLE()
};

#endif
