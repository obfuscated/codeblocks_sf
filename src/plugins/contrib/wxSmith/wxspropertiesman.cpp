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
        
        int itMask = 0;
        
        if ( CurrentWidget->GetParent() )
        {
            itMask |= wxsPalette::itBefore | wxsPalette::itAfter;
        }
        
        if ( CurrentWidget->IsContainer() )
        {
            itMask |= wxsPalette::itInto;
        }
        
        wxsPalette::Get()->SetInsertionTypeMask(itMask);
        wxSmith::Get()->GetResourceTree()->SelectItem(Widget->GetTreeId());
    }
    else
    {
        wxsPalette::Get()->SetInsertionTypeMask(0);
    }
    
    PropertiesPanel->Refresh();
    PropertiesPanel->Thaw();
    PropertiesPanel->SetSize(Size);
}

/** Singleton definition */
wxsPropertiesMan wxsPropertiesMan::Singleton;
