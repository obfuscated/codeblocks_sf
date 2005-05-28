#ifndef SEARCHRESULTSLOG_H
#define SEARCHRESULTSLOG_H

#include "simplelistlog.h"

class SearchResultsLog : public SimpleListLog
{
	public:
		SearchResultsLog(wxNotebook* parent, const wxString& title, int numCols, int widths[], const wxArrayString& titles);
		virtual ~SearchResultsLog();
		void FocusEntry(size_t index);
	protected:
        void OnClick(wxCommandEvent& event);
        void OnDoubleClick(wxCommandEvent& event);
        void SyncEditor(int selIndex);
	private:
        DECLARE_EVENT_TABLE()
};

#endif // SEARCHRESULTSLOG_H
