#include "ReopenEditorConfDLg.h"
#include "ReopenEditor.h"
#include "configmanager.h"

//(*InternalHeaders(ReopenEditorConfDLg)
#include <wx/xrc/xmlres.h>
#include <wx/radiobox.h>
//*)

//(*IdInit(ReopenEditorConfDLg)
//*)

BEGIN_EVENT_TABLE(ReopenEditorConfDLg,wxPanel)
	//(*EventTable(ReopenEditorConfDLg)
	//*)
END_EVENT_TABLE()

ReopenEditorConfDLg::ReopenEditorConfDLg(wxWindow* parent)
{
	BuildContent(parent);
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    bool isManaged = cfg->ReadBool(_T("/reopen_editor/managed"),true);
    XRCCTRL(*this, "rbReopenDockOrNot", wxRadioBox)->SetSelection(isManaged?1:0);

}

void ReopenEditorConfDLg::BuildContent(wxWindow* parent)
{
	//(*Initialize(ReopenEditorConfDLg)
	wxXmlResource::Get()->LoadObject(this,parent,_T("ReopenEditorConfDLg"),_T("wxPanel"));
	RadioBox1 = (wxRadioBox*)FindWindow(XRCID("rbReopenDockOrNot"));
	//*)
}

ReopenEditorConfDLg::~ReopenEditorConfDLg()
{
	//(*Destroy(ReopenEditorConfDLg)
	//*)
}

void ReopenEditorConfDLg::SaveSettings()
{
    ReopenEditor* plugin =  (ReopenEditor*)Manager::Get()->GetPluginManager()->FindPluginByName(_T("ReopenEditor"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    bool wasManaged = cfg->ReadBool(_T("/reopen_editor/managed"),true);
    bool isManaged = XRCCTRL(*this, "rbReopenDockOrNot", wxRadioBox)->GetSelection() == 1;

    if(wasManaged != isManaged)
    {
        cfg->Write(_T("/reopen_editor/managed"),isManaged);
        plugin->SetManaged(isManaged);
        plugin->ShowList();
    }

}
