#ifndef SEARCHRESULTSLOG_H
#define SEARCHRESULTSLOG_H

#include "simplelistlog.h"

class SearchResultsLog : public SimpleListLog
{
	public:
		SearchResultsLog(int numCols, int widths[], const wxArrayString& titles);
		virtual ~SearchResultsLog();
		void FocusEntry(size_t index);
		void SetBasePath(const wxString base){ m_Base = base; }
	protected:
        void OnClick(wxCommandEvent& event);
        void OnDoubleClick(wxCommandEvent& event);
        void SyncEditor(int selIndex);

        wxString m_Base;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // SEARCHRESULTSLOG_H
