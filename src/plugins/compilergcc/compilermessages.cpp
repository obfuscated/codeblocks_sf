#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <messagemanager.h>
#include "compilererrors.h"
#include "compilermessages.h"

/*int idMessagesList = wxNewId();*/

BEGIN_EVENT_TABLE(CompilerMessages, SimpleListLog)

END_EVENT_TABLE()

CompilerMessages::CompilerMessages(wxNotebook* parent, const wxString& title, int numCols, int widths[], const wxArrayString& titles)
    : SimpleListLog(parent, title, numCols, widths, titles)
{
	//ctor
    int id = m_pList->GetId();
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnClick);
    Connect(id, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnDoubleClick);
}

CompilerMessages::~CompilerMessages()
{
	//dtor
}

void CompilerMessages::FocusError(int nr)
{
    m_pList->SetItemState(nr, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    m_pList->EnsureVisible(nr);
}

void CompilerMessages::OnClick(wxCommandEvent& event)
{
    // a compiler message has been clicked
    // go to the relevant file/line
    if (m_pList->GetSelectedItemCount() == 0 || !m_pErrors)
        return;

    // find selected item index
    int index = m_pList->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);

    // call the CompilerErrors* ptr; it 'll do all the hard work ;)
    m_pErrors->GotoError(index);
}

void CompilerMessages::OnDoubleClick(wxCommandEvent& event)
{
    // a compiler message has been double-clicked
    // go to the relevant file/line
    if (m_pList->GetSelectedItemCount() == 0 || !m_pErrors)
        return;

    // find selected item index
    int index = m_pList->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);

    // call the CompilerErrors* ptr; it 'll do all the hard work ;)
    wxString error = m_pErrors->GetErrorString(index);
    wxMessageBox(error, _("Compiler warning/error"));
}
