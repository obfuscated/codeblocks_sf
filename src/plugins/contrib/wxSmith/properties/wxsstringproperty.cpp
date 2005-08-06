#include "wxsstringproperty.h"

#include <wx/textctrl.h>

class wxsStringPropertyWindow: public wxTextCtrl
{
    public:
    
        wxsStringPropertyWindow(wxWindow* Parent,wxsStringProperty* Property);
        virtual ~wxsStringPropertyWindow();
        
    private:
    
        void OnTextChange(wxCommandEvent& event);
        void OnTextEnter(wxCommandEvent& event);
        void OnKillFocus(wxFocusEvent& event);
        wxsStringProperty* Prop;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsStringPropertyWindow,wxTextCtrl)
    EVT_TEXT(-1,wxsStringPropertyWindow::OnTextChange)
    EVT_TEXT_ENTER(-1,wxsStringPropertyWindow::OnTextEnter)
    EVT_KILL_FOCUS(wxsStringPropertyWindow::OnKillFocus)
END_EVENT_TABLE()

wxsStringPropertyWindow::wxsStringPropertyWindow(wxWindow* Parent,wxsStringProperty* Property):
    wxTextCtrl(Parent,-1,Property->Value,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER),
    Prop(Property)
{
}

wxsStringPropertyWindow::~wxsStringPropertyWindow()
{
}

void wxsStringPropertyWindow::OnTextChange(wxCommandEvent& event)
{
    if ( Prop->AlwUpd )
    {
        Prop->Value = GetValue();
        Prop->ValueChanged();
    }
}

void wxsStringPropertyWindow::OnTextEnter(wxCommandEvent& event)
{
    if ( !Prop->AlwUpd )
    {
        Prop->Value = GetValue();
        Prop->ValueChanged();
    }
}

void wxsStringPropertyWindow::OnKillFocus(wxFocusEvent& event)
{
    if ( !Prop->AlwUpd )
    {
        Prop->Value = GetValue();
        Prop->ValueChanged();
    }
}

wxsStringProperty::wxsStringProperty(wxsProperties* Properties,wxString& String, bool AlwaysUpdate):
    wxsProperty(Properties), Value(String), AlwUpd(AlwaysUpdate), Window(NULL)
{
	//ctor
}

wxsStringProperty::~wxsStringProperty()
{
	//dtor
}

const wxString& wxsStringProperty::GetTypeName()
{
    static wxString Name(_T("wxString"));
    return Name;
}

wxWindow* wxsStringProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxsStringPropertyWindow(Parent,this);
}

void wxsStringProperty::UpdateEditWindow()
{
    if ( Window ) Window->SetValue(Value);

}
