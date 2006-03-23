#include "wxsquickpropspanel.h"
#include "wxspropertycontainer.h"

wxsQuickPropsPanel::wxsQuickPropsPanel(wxsPropertyContainer* PC):
    wxPanel(),
    Container(PC)
{
}

wxsQuickPropsPanel::wxsQuickPropsPanel(
    wxWindow* parent,
    wxsPropertyContainer* PC,
    wxWindowID id, 
    const wxPoint& pos, 
    const wxSize& size, 
    long style, 
    const wxString& name):
        wxPanel(parent,id,pos,size,style,name),
        Container(PC)
{
}

wxsQuickPropsPanel::~wxsQuickPropsPanel()
{
    // Notifying container that quick properties object does no longer exist
    if ( Container )
    {
        Container->CurrentQP = NULL;
        Container = NULL;
    }
}

void wxsQuickPropsPanel::NotifyChange()
{
    if ( Container )
    {
        Container->NotifyPropertyChangeFromQuickProps();
    }
}
