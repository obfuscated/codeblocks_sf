/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CB_SEARCHRESULTSLOG_H
#define CB_SEARCHRESULTSLOG_H

#include <wx/string.h>
#include "loggers.h"

class wxArrayString;
class wxCommandEvent;

class DLLIMPORT cbSearchResultsLog : public ListCtrlLogger, public wxEvtHandler
{
	public:
		cbSearchResultsLog(const wxArrayString& titles, wxArrayInt& widths);
		~cbSearchResultsLog() override;
		void FocusEntry(size_t index);
		void SetBasePath(const wxString base){ m_Base = base; }

		wxWindow* CreateControl(wxWindow* parent) override;
	protected:
        void OnDoubleClick(wxCommandEvent& event);
        void SyncEditor(int selIndex);

        wxString m_Base;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // CB_SEARCHRESULTSLOG_H
