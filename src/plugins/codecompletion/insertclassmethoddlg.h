#ifndef INSERTCLASSMETHODDLG_H
#define INSERTCLASSMETHODDLG_H

#include <wx/dialog.h>
#include <wx/checklst.h>
#include "parser/parser.h"

class InsertClassMethodDlg : public wxDialog
{
	public:
		InsertClassMethodDlg(wxWindow* parent, Parser* parser, const wxString& filename);
		virtual ~InsertClassMethodDlg();

        wxArrayString GetCode(); // return an array of checked methods
	protected:
        void FillClasses();
        void FillMethods();
        void DoFillMethodsFor(wxCheckListBox* clb,
                                Token* parentToken,
                                const wxString& ns,
                                bool includePrivate,
                                bool includeProtected,
                                bool includePublic);
        void OnClassesChange(wxCommandEvent& event);
        void OnCodeChange(wxCommandEvent& event);
        void OnFilterChange(wxCommandEvent& event);
	private:
        Parser* m_pParser;
        bool m_Decl;
        wxString m_Filename;
        DECLARE_EVENT_TABLE();
};

#endif // INSERTCLASSMETHODDLG_H

