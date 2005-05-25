#include "wxs2intproperty.h"

#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/stattext.h>

#include "../widget.h"

/*
static const int Text1Id = wxNewId();
static const int Text2Id = wxNewId();
*/

class wxs2IntPropertyWindow: public wxPanel
{
    public:
    
        wxs2IntPropertyWindow(wxWindow* Parent,wxs2IntProperty* Property);
        virtual ~wxs2IntPropertyWindow();
        
        void SetValues(int Val1,int Val2);
        
    private:
    
        void OnTextChange(wxCommandEvent& event);
        void OnTextEnter(wxCommandEvent& event);
        void OnKillFocus(wxFocusEvent& event);
        wxs2IntProperty* Prop;
        wxTextCtrl* Text1;
        wxTextCtrl* Text2;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxs2IntPropertyWindow,wxPanel)
    EVT_TEXT(-1,wxs2IntPropertyWindow::OnTextChange)
    EVT_TEXT_ENTER(-1,wxs2IntPropertyWindow::OnTextEnter)
    EVT_KILL_FOCUS(wxs2IntPropertyWindow::OnKillFocus)
END_EVENT_TABLE()

wxs2IntPropertyWindow::wxs2IntPropertyWindow(wxWindow* Parent,wxs2IntProperty* Property):
    wxPanel(Parent,-1),
    Prop(Property)
{
    Text1 = new wxTextCtrl(this,-1,wxT("")/*wxString::Format("%d",Property->Value1)*/,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    Text2 = new wxTextCtrl(this,-1,wxT("")/*wxString::Format("%d",Property->Value2)*/,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    
    wxSize Size = Text1->GetSize();
    Size.SetWidth(Size.GetHeight()*2);  // TODO: This seem to be useless
    Text1->SetSize(Size);
    Text2->SetSize(Size);

    wxFlexGridSizer* Sizer = new wxFlexGridSizer(3,3);
    Sizer->AddGrowableCol(0);
    Sizer->AddGrowableCol(2);
    Sizer->Add(Text1,0,wxGROW);
    Sizer->Add(new wxStaticText(this,-1,wxT("x")),0,wxLEFT|wxRIGHT|wxALIGN_CENTRE_VERTICAL,5);
    Sizer->Add(Text2,0,wxGROW);
    SetSizer(Sizer);
    Sizer->SetSizeHints(this);
}

wxs2IntPropertyWindow::~wxs2IntPropertyWindow()
{
}

void wxs2IntPropertyWindow::SetValues(int V1,int V2)
{
    Text1->SetValue(wxString::Format("%d",V1));
    Text2->SetValue(wxString::Format("%d",V2));
}

void wxs2IntPropertyWindow::OnTextChange(wxCommandEvent& event)
{
    if ( Prop->AlwUpd )
    {
        Prop->Value1 = atoi(Text1->GetValue().c_str());
        Prop->Value2 = atoi(Text2->GetValue().c_str());
        Prop->CorrectValues(Prop->Value1,Prop->Value2);
        Prop->ValueChanged();
    }
}

void wxs2IntPropertyWindow::OnTextEnter(wxCommandEvent& event)
{
    Prop->Value1 = atoi(Text1->GetValue().c_str());
    Prop->Value2 = atoi(Text2->GetValue().c_str());
    Prop->CorrectValues(Prop->Value1,Prop->Value2);
    Text1->SetValue(wxString::Format("%d",Prop->Value1));
    Text2->SetValue(wxString::Format("%d",Prop->Value2));
    Prop->ValueChanged();
}

void wxs2IntPropertyWindow::OnKillFocus(wxFocusEvent& event)
{
    Prop->Value1 = atoi(Text1->GetValue().c_str());
    Prop->Value2 = atoi(Text2->GetValue().c_str());
    Prop->CorrectValues(Prop->Value1,Prop->Value2);
    Text1->SetValue(wxString::Format("%d",Prop->Value1));
    Text2->SetValue(wxString::Format("%d",Prop->Value2));
    Prop->ValueChanged();
}







wxs2IntProperty::wxs2IntProperty(wxsProperties* Properties,int& Int1,int& Int2, bool AlwaysUpdate):
    wxsProperty(Properties), Value1(Int1), Value2(Int2), AlwUpd(AlwaysUpdate), Window(NULL)
{
	//ctor
}

wxs2IntProperty::~wxs2IntProperty()
{
	//dtor
}

const wxString& wxs2IntProperty::GetTypeName()
{
    static wxString Name(wxT("2 x int"));
    return Name;
}

wxWindow* wxs2IntProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxs2IntPropertyWindow(Parent,this);
}

void wxs2IntProperty::UpdateEditWindow()
{
    if ( Window ) Window->SetValues(Value1,Value2);
}
