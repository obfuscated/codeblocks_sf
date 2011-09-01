/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef AUTOSAVE_H
#define AUTOSAVE_H

#include <wx/intl.h>
#include <wx/string.h>

#include "cbplugin.h" // the base class we 're inheriting
#include "configurationpanel.h"

class wxTimer;
class wxTimerEvent;

class Autosave : public cbPlugin
{
    wxTimer *timer1;
    wxTimer *timer2;

	public:
		Autosave();
		~Autosave();
		int Configure();
		int GetConfigurationPriority() const{ return  50; }
		int GetConfigurationGroup() const{ return  cgUnknown; }
		void BuildMenu(wxMenuBar* /*menuBar*/){ return ; }
		void BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/ = 0){ return ; }
		bool BuildToolBar(wxToolBar* /*toolBar*/){ return false; }
		void OnAttach(); // fires when the plugin is attached to the application
		void Start();
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
		virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        void OnTimer(wxTimerEvent& event);
    DECLARE_EVENT_TABLE()
};


class AutosaveConfigDlg : public cbConfigurationPanel
{
    Autosave* plugin;

	public:
		AutosaveConfigDlg(wxWindow* parent, Autosave* plug);
		virtual ~AutosaveConfigDlg(){};

        virtual wxString GetTitle() const { return _("Autosave"); }
        virtual wxString GetBitmapBaseName() const { return _T("autosave"); }
        virtual void OnApply(){ SaveSettings(); }
        virtual void OnCancel(){}
	private:
        void LoadSettings();
        void SaveSettings();
};

#endif // AUTOSAVE_H

