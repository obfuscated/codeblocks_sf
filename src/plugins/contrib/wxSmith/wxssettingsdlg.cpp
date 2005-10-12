#include "wxssettingsdlg.h"

BEGIN_EVENT_TABLE(wxsSettingsDlg,wxDialog)
//(*EventTable(wxsSettingsDlg)
//*)
END_EVENT_TABLE()

wxsSettingsDlg::wxsSettingsDlg(wxWindow* parent,wxWindowID id):
    wxDialog(parent,id,_T(""),wxDefaultPosition,wxDefaultSize)
{
    //(*Initialize(wxsSettingsDlg)
    wxXmlResource::Get()->LoadDialog(this,parent,_T("wxsSettingsDlg"));
    Notebook1 = XRCCTRL(*this,"ID_NOTEBOOK1",wxNotebook);
    Panel1 = XRCCTRL(*this,"ID_PANEL1",wxPanel);
    SpinCtrl1 = XRCCTRL(*this,"ID_SPINCTRL1",wxSpinCtrl);
    ComboBox1 = XRCCTRL(*this,"ID_COMBOBOX1",wxComboBox);
    Button1 = XRCCTRL(*this,"ID_BUTTON1",wxButton);
    Button2 = XRCCTRL(*this,"ID_BUTTON2",wxButton);
    //*)
}

wxsSettingsDlg::~wxsSettingsDlg()
{
}

