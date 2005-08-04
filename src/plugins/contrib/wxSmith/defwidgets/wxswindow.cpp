#include "wxsdialog.h"

#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"

class wxsWindowPreview: public wxPanel
{
    public:
    
        // Need to check Window->GetParent() before setting style -
        // - if this is wxPanel which has parent, it is not a resource
        wxsWindowPreview(wxWindow* Parent,wxsWindow* _Window):
            wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize,
                _Window->GetParent() ? _Window->GetBaseParams().Style : wxRAISED_BORDER ),
            Window(_Window)
        {
        	if ( !Window->GetParent() )
        	{
                SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));        	
        	}
        }
        
        void UpdatePreview()
        {
        	SetSizer(NULL);
            int Cnt = Window->GetChildCount();
            
            // Searching for sizer - it's preview is standard widget and must be
            // additionally binded to this window through sizer
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Widget = Window->GetChild(i);
                if ( Widget->GetInfo().Sizer && Widget->GetPreview() )
                {
                    wxSizer* Sizer = new wxGridSizer(1);
                    Sizer->Add(Widget->GetPreview(),1,wxGROW);
                    SetSizer(Sizer);
                    Sizer->SetSizeHints(this);
                }
            }
        }
        
    private:
        
        wxsWindow* Window;
};

wxsWindow::wxsWindow(wxsWidgetManager* Man,wxsWidget::BasePropertiesType pType):
    wxsContainer(Man,true,0,pType)
{
}

wxsWindow::~wxsWindow()
{
}

/** Function shich should update content of current widget
 *
 * This function should update current content of widget if it's created
 * to keep it's content up to date
 */
void wxsWindow::MyUpdatePreview()
{
    if ( GetPreview() )
    {
        dynamic_cast<wxsWindowPreview*>
            (GetPreview()) -> UpdatePreview();
    }
}

/** This function should create preview window for widget */
wxWindow* wxsWindow::MyCreatePreview(wxWindow* Parent)
{
   return new wxsWindowPreview(Parent,this);
}
