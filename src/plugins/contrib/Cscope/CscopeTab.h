#ifndef CSCOPE_TAB
#define CSCOPE_TAB

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
//#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/gauge.h>
#include <wx/panel.h>

#include "loggers.h"

#include "CscopeEntryData.h"


class CscopeConfig;

class CscopeTab : public wxPanel
{
protected:
    wxListCtrl *m_pListCtrl;
    wxStaticText* m_statusMessage;
    wxGauge* m_gauge;

    CscopeResultTable *m_table;

protected:
	void ClearResults(void);

private:
    void OnListItemActivated(wxListEvent &event);


public:
    void CopyContentsToClipboard(bool selectionOnly);
	CscopeTab( wxWindow* parent, CscopeConfig *cfg );
	virtual ~CscopeTab();
	void BuildTable(CscopeResultTable *table);
	void Clear();
	void SetMessage(const wxString &msg, int percent);

private:
	CscopeConfig *m_cfg;
};

#endif // CSCOPE_TAB
