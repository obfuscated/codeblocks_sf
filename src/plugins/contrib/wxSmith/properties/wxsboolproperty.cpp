#include "wxsboolproperty.h"

#include <wx/checkbox.h>

class wxsBoolPropertyWindow: public wxCheckBox
{
    public:
    
        wxsBoolPropertyWindow(wxWindow* Parent,wxsBoolProperty* Property);
        virtual ~wxsBoolPropertyWindow();
        
    private:
    
        void OnChange(wxCommandEvent& event);
        wxsBoolProperty* Prop;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsBoolPropertyWindow,wxCheckBox)
    EVT_CHECKBOX(-1,wxsBoolPropertyWindow::OnChange)
END_EVENT_TABLE()

wxsBoolPropertyWindow::wxsBoolPropertyWindow(wxWindow* Parent,wxsBoolProperty* Property):
    wxCheckBox(Parent,-1,wxT("")),
    Prop(Property)
{
}   

wxsBoolPropertyWindow::~wxsBoolPropertyWindow()
{
}

void wxsBoolPropertyWindow::OnChange(wxCommandEvent& event)
{
    Prop->Value = GetValue();
    Prop->ValueChanged();
}

wxsBoolProperty::wxsBoolProperty(wxsProperties* Properties,bool& Bool):
    wxsProperty(Properties), Value(Bool), Window(NULL)
{
	//ctor
}

wxsBoolProperty::~wxsBoolProperty()
{
	//dtor
}

const wxString& wxsBoolProperty::GetTypeName()
{
    static wxString Name(wxT("bool"));
    return Name;
}

wxWindow* wxsBoolProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxsBoolPropertyWindow(Parent,this);
}

void wxsBoolProperty::UpdateEditWindow()
{
    if ( Window ) Window->SetValue(Value);
}
