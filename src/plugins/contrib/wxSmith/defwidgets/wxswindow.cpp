#include "wxsdialog.h"

#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"

class wxsWindowPreview: public wxPanel
{
    public:
    
        // Need to check Window->GetParent() before setting style -
        // - if this is wxPanel which has parent, it is not a resource
        wxsWindowPreview(wxWindow* Parent,wxsWindow* _Window,const wxPoint& Position,const wxSize& Size):
            wxPanel(Parent,-1,Position,Size,
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
                    if ( ((Window->GetBaseParams().SizeX == -1) &&
                          (Window->GetBaseParams().SizeY == -1)) ||
                           Window->GetBaseParams().DefaultSize )
                    {
                    	Sizer->SetSizeHints(this);
                    }
                }
            }
        }
        
    private:
        
        wxsWindow* Window;
};

wxsWindow::wxsWindow(wxsWidgetManager* Man,wxsWindowRes* Res,wxsWidget::BasePropertiesType pType):
    wxsContainer(Man,Res,true,0,pType)
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
void wxsWindow::MyFinalUpdatePreview(wxWindow* Preview)
{
    dynamic_cast<wxsWindowPreview*>
        (Preview) -> UpdatePreview();
}

/** This function should create preview window for widget */
wxWindow* wxsWindow::MyCreatePreview(wxWindow* Parent)
{
    return new wxsWindowPreview(Parent,this, GetParent() ? GetPosition() : wxDefaultPosition, GetSize());
}

int wxsWindow::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Spacer )
	{
		// ITem must be a child of sizer - cannot add it here
		wxMessageBox(_("This item can be added into sizer only"));
		return -1;
	}
	
	if ( NewWidget->GetInfo().Sizer )
	{
		// We're adding sizer to this container - it must be empty
		// we will check some more properties to give better error
		// explanation
		
		if ( GetChildCount() )
		{
            if ( GetChild(0)->GetInfo().Sizer )
            {
                wxMessageBox(_("This item has sizer already. Can not add other one"));
            }
            else
            {
                wxMessageBox(_("There are widgets on this item. Sizer can be added to empty item only"));
            }
            return -1;
		}
	}
	else
	{
		// We're adding widget into this item - if there's any sizer inside we can not add
		int Cnt = GetChildCount();
		for ( int i=0; i<Cnt; i++ )
		{
			if ( GetChild(i)->GetInfo().Sizer )
			{
                wxMessageBox(_("This item contains sizer. Please add new items into this sizer."));
                return -1;
			}
		}
	}
	
	return wxsContainer::AddChild(NewWidget,InsertBeforeThis);
}
