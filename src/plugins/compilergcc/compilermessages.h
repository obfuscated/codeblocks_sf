#ifndef COMPILERMESSAGES_H
#define COMPILERMESSAGES_H

#include <simplelistlog.h>

class CompilerErrors;

class CompilerMessages : public SimpleListLog
{
	public:
		CompilerMessages(wxNotebook* parent, const wxString& title, int numCols, int widths[], const wxArrayString& titles);
		virtual ~CompilerMessages();
        virtual void SetCompilerErrors(CompilerErrors* errors){ m_pErrors = errors; }
        virtual void FocusError(int nr);
	protected:
        void OnClick(wxCommandEvent& event);
        CompilerErrors* m_pErrors;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // COMPILERMESSAGES_H

