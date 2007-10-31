#ifndef COMPILERMESSAGES_H
#define COMPILERMESSAGES_H

#include "simplelistlog.h"

class CompilerErrors;
class wxArrayString;
class wxCommandEvent;

class CompilerMessages : public SimpleListLog
{
	public:
		CompilerMessages(int numCols, int widths[], const wxArrayString& titles);
		virtual ~CompilerMessages();
        virtual void SetCompilerErrors(CompilerErrors* errors){ m_pErrors = errors; }
        virtual void FocusError(int nr);
	private:
        void OnClick(wxCommandEvent& event);
        void OnDoubleClick(wxCommandEvent& event);
        void OnRightClick(wxCommandEvent& event);
        void OnCopyToClipboard(wxCommandEvent& event);

        wxString AssembleMessage(int id);

        CompilerErrors* m_pErrors;

        DECLARE_EVENT_TABLE()
};

#endif // COMPILERMESSAGES_H

