#include "occurrencespanel.h"

//(*InternalHeaders(OccurrencesPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(OccurrencesPanel)
const long OccurrencesPanel::ID_LISTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OccurrencesPanel,wxPanel)
	//(*EventTable(OccurrencesPanel)
	//*)
END_EVENT_TABLE()

OccurrencesPanel::OccurrencesPanel(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(OccurrencesPanel)
	wxBoxSizer* BoxSizer1;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ListCtrl1 = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_LIST|wxLC_AUTOARRANGE|wxLC_SORT_ASCENDING, wxDefaultValidator, _T("ID_LISTCTRL1"));
	BoxSizer1->Add(ListCtrl1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

OccurrencesPanel::~OccurrencesPanel()
{
	//(*Destroy(OccurrencesPanel)
	//*)
}

