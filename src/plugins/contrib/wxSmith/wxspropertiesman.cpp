#include "wxspropertiesman.h"

#include "wxspalette.h"
#include "wxsmith.h"

wxsPropertiesMan::wxsPropertiesMan():
    CurrentWidget(NULL),
    PropertiesPanel(NULL)
{
	//ctor
}

wxsPropertiesMan::~wxsPropertiesMan()
{
}

void wxsPropertiesMan::SetActiveWidget(wxsWidget* Widget)
{
    if ( !PropertiesPanel ) return;
    
    if ( CurrentWidget == Widget ) return;

    PropertiesPanel->Freeze();
    
    if ( CurrentWidget )
    {
        CurrentWidget->KillProperties();
    }

    PropertiesPanel->SetSizer(NULL);
    PropertiesPanel->DestroyChildren();
    
    CurrentWidget = Widget;
    
    wxSize Size = PropertiesPanel->GetSize();
    if ( CurrentWidget )
    {
        wxFlexGridSizer* NewSizer = new wxFlexGridSizer(1);
        PropertiesPanel->SetVirtualSizeHints(1,1);
        PropertiesPanel->SetSize(1,1);
        NewSizer->AddGrowableCol(0);
        wxWindow* Wnd = CurrentWidget->GetProperties(PropertiesPanel);
        CurrentWidget->UpdateProperties();
        NewSizer->Add(Wnd,0,wxGROW);
        PropertiesPanel->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(PropertiesPanel);
        
        wxSmith::Get()->GetResourceTree()->SelectItem(Widget->GetTreeId());
    }
    
    PropertiesPanel->Refresh();
    PropertiesPanel->Thaw();
    PropertiesPanel->SetSize(Size);
}

void wxsPropertiesMan::OnSelectWidget(wxsEvent& event)
{
    SetActiveWidget(event.GetWidget());
}

void wxsPropertiesMan::OnUnselectWidget(wxsEvent& event)
{
    if ( event.GetWidget() == CurrentWidget )
    {
        SetActiveWidget(NULL);
    }
}

BEGIN_EVENT_TABLE(wxsPropertiesMan,wxEvtHandler)
    EVT_SELECT_WIDGET(wxsPropertiesMan::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsPropertiesMan::OnUnselectWidget)
END_EVENT_TABLE()


/** Singleton definition */
wxsPropertiesMan wxsPropertiesMan::Singleton;
