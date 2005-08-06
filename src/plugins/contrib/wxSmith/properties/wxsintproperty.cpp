#include "wxsintproperty.h"

#include <wx/textctrl.h>
#include <wx/msgdlg.h>

class wxsIntPropertyWindow: public wxTextCtrl
{
    public:
    
        wxsIntPropertyWindow(wxWindow* Parent,wxsIntProperty* Property);
        virtual ~wxsIntPropertyWindow();
        
    private:
    
        void OnTextChange(wxCommandEvent& event);
        void OnTextEnter(wxCommandEvent& event);
        void OnKillFocus(wxFocusEvent& event);
        wxsIntProperty* Prop;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsIntPropertyWindow,wxTextCtrl)
    EVT_TEXT(-1,wxsIntPropertyWindow::OnTextChange)
    EVT_TEXT_ENTER(-1,wxsIntPropertyWindow::OnTextEnter)
    EVT_KILL_FOCUS(wxsIntPropertyWindow::OnKillFocus)
END_EVENT_TABLE()

wxsIntPropertyWindow::wxsIntPropertyWindow(wxWindow* Parent,wxsIntProperty* Property):
    wxTextCtrl(Parent,-1,wxString::Format(_T("%d"),Property->Value),wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER),
    Prop(Property)
{
}

wxsIntPropertyWindow::~wxsIntPropertyWindow()
{
}

void wxsIntPropertyWindow::OnTextChange(wxCommandEvent& event)
{
    if ( Prop->AlwUpd )
    {
    	long Val = 0;
    	GetValue().ToLong(&Val);
        Prop->Value = Prop->CorrectValue(Val);
        Prop->ValueChanged();
    }
}

void wxsIntPropertyWindow::OnTextEnter(wxCommandEvent& event)
{
    long Val = 0;
    GetValue().ToLong(&Val);
    Prop->Value = Prop->CorrectValue(Val);
    SetValue(wxString::Format(_T("%d"),Prop->Value));
    Prop->ValueChanged();
}

void wxsIntPropertyWindow::OnKillFocus(wxFocusEvent& event)
{
    long Val = 0;
    GetValue().ToLong(&Val);
    Prop->Value = Prop->CorrectValue(Val);
    SetValue(wxString::Format(_T("%d"),Prop->Value));
    Prop->ValueChanged();
}

wxsIntProperty::wxsIntProperty(wxsProperties* Properties,int& Int, bool AlwaysUpdate):
    wxsProperty(Properties), Value(Int), AlwUpd(AlwaysUpdate), Window(NULL)
{
	//ctor
}

wxsIntProperty::~wxsIntProperty()
{
	//dtor
}

const wxString& wxsIntProperty::GetTypeName()
{
    static wxString Name(_T("int"));
    return Name;
}

wxWindow* wxsIntProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxsIntPropertyWindow(Parent,this);
}

void wxsIntProperty::UpdateEditWindow()
{
    if ( Window ) Window->SetValue(wxString::Format(_T("%d"),Value));
}
        
