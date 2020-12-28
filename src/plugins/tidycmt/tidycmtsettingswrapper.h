/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TIDYCMTSETTINGSWRAPPER_H
#define TIDYCMTSETTINGSWRAPPER_H

class TidyCmt;
class TidyCmtSettings;

#include "configurationpanel.h"
#include "tidycmtconfig.h"

class TidyCmtSettingsWrapper : public cbConfigurationPanel
{
	public:

		TidyCmtSettingsWrapper(wxWindow* parent, TidyCmt* plugin, const TidyCmtConfig& tcc);
		virtual ~TidyCmtSettingsWrapper() { ; }

    virtual wxString GetTitle() const          { return _T("tidycmt");        }
    virtual wxString GetBitmapBaseName() const { return _T("generic-plugin"); }

	protected:

    TidyCmt*         m_Plugin;
    TidyCmtSettings* m_TidyCmtSettings;

	private:

    virtual void OnApply();
    virtual void OnCancel() { ; }

		DECLARE_EVENT_TABLE()
};

#endif // TIDYCMTSETTINGSWRAPPER_H
