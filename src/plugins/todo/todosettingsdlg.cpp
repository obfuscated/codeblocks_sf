#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "configmanager.h"
#include "manager.h"
#endif
#include "todosettingsdlg.h"

ToDoSettingsDlg::ToDoSettingsDlg()
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, 0L, _T("ToDoSettingsDlg"));
	bool checked = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("auto_refresh"), true);
	XRCCTRL(*this, "chkAutoRefresh", wxCheckBox)->SetValue(checked);
}

ToDoSettingsDlg::~ToDoSettingsDlg()
{
	//dtor
}

void ToDoSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        bool checked = XRCCTRL(*this, "chkAutoRefresh", wxCheckBox)->GetValue();
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("auto_refresh"), checked);
    }

    wxDialog::EndModal(retCode);
}
