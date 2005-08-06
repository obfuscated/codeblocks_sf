#include "wxsborderproperty.h"

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include "../widget.h"

class wxsBorderPropertyWindow: public wxPanel
{
    public:
        wxsBorderPropertyWindow(wxWindow* Parent,wxsBorderProperty* Object);
        virtual ~wxsBorderPropertyWindow();
        
        void UpdateData();
        
    private:

        void OnButtonChanged(wxCommandEvent& event);
        
        //wxToggleButton 
        wxCheckBox
            *Left, *Right, *Top, *Bottom;
    
        wxsBorderProperty* Object;
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsBorderPropertyWindow,wxPanel)
    EVT_TOGGLEBUTTON(wxID_ANY,wxsBorderPropertyWindow::OnButtonChanged)
    EVT_CHECKBOX(wxID_ANY,wxsBorderPropertyWindow::OnButtonChanged)
END_EVENT_TABLE()

wxsBorderPropertyWindow::wxsBorderPropertyWindow(wxWindow* Parent,wxsBorderProperty* _Object):
    wxPanel(Parent,-1),
    Object(_Object)
{
    Left = new wxCheckBox(this,-1,_T(""));
    Right = new wxCheckBox(this,-1,_T(""));
    Top = new wxCheckBox(this,-1,_T(""));
    Bottom = new wxCheckBox(this,-1,_T(""));
    
    wxFlexGridSizer* Sizer = new wxFlexGridSizer(3,1,1);
    
    Sizer->Add(1,1);
    Sizer->Add(Top);
    Sizer->Add(1,1);
    Sizer->Add(Left);
    Sizer->Add(1,1);
    Sizer->Add(Right);
    Sizer->Add(1,1);
    Sizer->Add(Bottom);
    Sizer->Add(1,1);
    
    SetSizer(Sizer);
    Sizer->SetSizeHints(this);
}

wxsBorderPropertyWindow::~wxsBorderPropertyWindow()
{}

void wxsBorderPropertyWindow::OnButtonChanged(wxCommandEvent& event)
{
    int NewFlags = 
        ( Left->GetValue()   ? wxsWidgetBaseParams::Left   :  wxsWidgetBaseParams::None ) |
        ( Right->GetValue()  ? wxsWidgetBaseParams::Right  :  wxsWidgetBaseParams::None ) |
        ( Top->GetValue()    ? wxsWidgetBaseParams::Top    :  wxsWidgetBaseParams::None ) |
        ( Bottom->GetValue() ? wxsWidgetBaseParams::Bottom :  wxsWidgetBaseParams::None );
        
    assert(Object != NULL);
    
    Object->BorderFlags = NewFlags;
    Object->ValueChanged();
}

void wxsBorderPropertyWindow::UpdateData()
{
    assert ( Object != NULL );
    int Flags = Object->BorderFlags;
   
    Left->SetValue( (Flags&wxsWidgetBaseParams::Left) != 0 );
    Right->SetValue( (Flags&wxsWidgetBaseParams::Right) != 0 );
    Top->SetValue( (Flags&wxsWidgetBaseParams::Top) != 0 );
    Bottom->SetValue( (Flags&wxsWidgetBaseParams::Bottom) != 0 );

}

wxsBorderProperty::wxsBorderProperty(wxsProperties* Properties,int& Flag):
    wxsProperty(Properties),
    BorderFlags(Flag)
{
	//ctor
}

wxsBorderProperty::~wxsBorderProperty()
{
	//dtor
}

const wxString& wxsBorderProperty::GetTypeName()
{
    static wxString Type(_T("Widget Border"));
    return Type;
}

wxWindow* wxsBorderProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxsBorderPropertyWindow(Parent,this);
}

void wxsBorderProperty::UpdateEditWindow()
{
    if ( Window )
    {
        Window->UpdateData();
    }
}
