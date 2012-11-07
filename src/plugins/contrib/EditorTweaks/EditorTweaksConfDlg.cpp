#include "EditorTweaksConfDlg.h"
#include "configmanager.h"

//(*InternalHeaders(EditorTweaksConfDlg)
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(EditorTweaksConfDlg)
//*)

BEGIN_EVENT_TABLE(EditorTweaksConfDlg,wxPanel)
	//(*EventTable(EditorTweaksConfDlg)
	//*)
END_EVENT_TABLE()

EditorTweaksConfDlg::EditorTweaksConfDlg(wxWindow* parent)
{
	//(*Initialize(EditorTweaksConfDlg)
	wxXmlResource::Get()->LoadObject(this,parent,_T("EditorTweaksConfDlg"),_T("wxPanel"));
	SpinCtrl1 = (wxSpinCtrl*)FindWindow(XRCID("ID_SPINCTRL1"));
	//*)
	SpinCtrl1->SetRange(1, 100);

	ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("EditorTweaks"));
    int maxSavedAlignerEntries = cfg->ReadInt(_T("/aligner/max_saved_entries"),4);
    SpinCtrl1->SetValue(maxSavedAlignerEntries);

}

EditorTweaksConfDlg::~EditorTweaksConfDlg()
{
	//(*Destroy(EditorTweaksConfDlg)
	//*)
}

void EditorTweaksConfDlg::SaveSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("EditorTweaks"));
    int oldSavedAlignerEntries = cfg->ReadInt(_T("/aligner/max_saved_entries"),4);
    int newSavedAlignerEntries = SpinCtrl1->GetValue();

    if(oldSavedAlignerEntries != newSavedAlignerEntries )
        cfg->Write(_T("aligner/max_saved_entries"),newSavedAlignerEntries);
}

