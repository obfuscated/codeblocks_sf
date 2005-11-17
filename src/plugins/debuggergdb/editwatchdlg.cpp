#include <sdk.h>
#include "editwatchdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/radiobox.h>

EditWatchDlg::EditWatchDlg(Watch* w, wxWindow* parent)
    : m_Watch(_T(""))
{
    //ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditWatch"));

    if (w)
        m_Watch = *w;
    XRCCTRL(*this, "txtKeyword", wxTextCtrl)->SetValue(m_Watch.keyword);
    XRCCTRL(*this, "rbFormat", wxRadioBox)->SetSelection((int)m_Watch.format);
}

EditWatchDlg::~EditWatchDlg()
{
    //dtor
}

void EditWatchDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        m_Watch.keyword = XRCCTRL(*this, "txtKeyword", wxTextCtrl)->GetValue();
        m_Watch.format = (WatchFormat)XRCCTRL(*this, "rbFormat", wxRadioBox)->GetSelection();
    }
    wxDialog::EndModal(retCode);
}
