#ifndef EDITORCONFIGUI_H
#define EDITORCONFIGUI_H

//(*Headers(EditorConfigUI)
#include <wx/panel.h>
class wxChoice;
class wxFlexGridSizer;
class wxSpinEvent;
class wxSpinCtrl;
class wxBoxSizer;
class wxStaticText;
class wxCheckBox;
//*)

class wxEvtHandler;

class cbProject;

#include "configurationpanel.h"

#include "EditorConfigCommon.h"

class EditorConfigUI : public cbConfigurationPanel
{
	public:

		EditorConfigUI(wxWindow* parent, wxEvtHandler* eh, cbProject* prj, const TEditorSettings& es);
		virtual ~EditorConfigUI();

		//(*Declarations(EditorConfigUI)
		wxCheckBox* chkUseTabs;
		wxSpinCtrl* spnIndent;
		wxCheckBox* chkActive;
		wxSpinCtrl* spnTabWidth;
		wxCheckBox* chkTabIndents;
		wxChoice* choEOLMode;
		//*)

  virtual wxString GetTitle() const          { return _("EditorConfig options"); };
  virtual wxString GetBitmapBaseName() const { return _T("generic-plugin");      };

	protected:

		//(*Identifiers(EditorConfigUI)
		static const long ID_CHK_ACTIVE;
		static const long ID_CHK_USE_TABS;
		static const long ID_CHK_TAB_INDENTS;
		static const long ID_SPN_TAB_WIDTH;
		static const long ID_SPN_INDENT;
		static const long ID_CHO_EOL_MODE;
		//*)

    wxEvtHandler* m_NotifiedWindow;
    cbProject*    m_Project;

	private:

		//(*Handlers(EditorConfigUI)
		void OnActiveClick(wxCommandEvent& event);
		//*)

    virtual void OnApply();
    virtual void OnCancel() { ; }

		void DoActive(bool en);

		DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIGUI_H
