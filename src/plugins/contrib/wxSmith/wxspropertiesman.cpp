#include "wxspropertiesman.h"

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

    if ( CurrentWidget )
    {
        CurrentWidget->KillProperties();
    }

    PropertiesPanel->SetSizer(NULL);
    PropertiesPanel->DestroyChildren();
    
    CurrentWidget = Widget;
    
    if ( CurrentWidget )
    {
        wxFlexGridSizer* NewSizer = new wxFlexGridSizer(1);
        NewSizer->AddGrowableCol(0);
        wxWindow* Wnd = CurrentWidget->GetProperties(PropertiesPanel);
        CurrentWidget->UpdateProperties();
        NewSizer->Add(Wnd,0,wxGROW);
        PropertiesPanel->SetSizer(NewSizer);
        PropertiesPanel->Layout();
     }
}

/** Singleton definition */

wxsPropertiesMan wxsPropertiesMan::Singleton;


