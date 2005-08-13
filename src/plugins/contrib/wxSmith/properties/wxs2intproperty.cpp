#include "wxs2intproperty.h"

#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/stattext.h>

#include "../widget.h"

#ifdef __NO_PROPGRGID
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
        Text1 = new wxTextCtrl(this,-1,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
        Text2 = new wxTextCtrl(this,-1,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
        
        wxSize Size = Text1->GetSize();
        Size.SetWidth(Size.GetHeight()*2);  // TODO: This seem to be useless
        Text1->SetSize(Size);
        Text2->SetSize(Size);
    
        wxFlexGridSizer* Sizer = new wxFlexGridSizer(3,3);
        Sizer->AddGrowableCol(0);
        Sizer->AddGrowableCol(2);
        Sizer->Add(Text1,0,wxGROW);
        Sizer->Add(new wxStaticText(this,-1,_T("x")),0,wxLEFT|wxRIGHT|wxALIGN_CENTRE_VERTICAL,5);
        Sizer->Add(Text2,0,wxGROW);
        SetSizer(Sizer);
        Sizer->SetSizeHints(this);
    }
    
    wxs2IntPropertyWindow::~wxs2IntPropertyWindow()
    {
    }
    
    void wxs2IntPropertyWindow::SetValues(int V1,int V2)
    {
        Text1->SetValue(wxString::Format(_T("%d"),V1));
        Text2->SetValue(wxString::Format(_T("%d"),V2));
    }
    
    void wxs2IntPropertyWindow::OnTextChange(wxCommandEvent& event)
    {
        if ( Prop->AlwUpd )
        {
            long Val = 0;
            Text1->GetValue().ToLong(&Val); Prop->Value1 = (int)Val;
            Val = 0;
            Text2->GetValue().ToLong(&Val); Prop->Value2 = (int)Val;
            Prop->CorrectValues(Prop->Value1,Prop->Value2);
            Prop->ValueChanged(false);
        }
    }
    
    void wxs2IntPropertyWindow::OnTextEnter(wxCommandEvent& event)
    {
        long Val = 0;
        Text1->GetValue().ToLong(&Val); Prop->Value1 = (int)Val;
        Val = 0;
        Text2->GetValue().ToLong(&Val); Prop->Value2 = (int)Val;
        Prop->CorrectValues(Prop->Value1,Prop->Value2);
        Text1->SetValue(wxString::Format(_T("%d"),Prop->Value1));
        Text2->SetValue(wxString::Format(_T("%d"),Prop->Value2));
        Prop->ValueChanged(true);
    }
    
    void wxs2IntPropertyWindow::OnKillFocus(wxFocusEvent& event)
    {
        long Val = 0;
        Text1->GetValue().ToLong(&Val); Prop->Value1 = (int)Val;
        Val = 0;
        Text2->GetValue().ToLong(&Val); Prop->Value2 = (int)Val;
        Prop->CorrectValues(Prop->Value1,Prop->Value2);
        Text1->SetValue(wxString::Format(_T("%d"),Prop->Value1));
        Text2->SetValue(wxString::Format(_T("%d"),Prop->Value2));
        Prop->ValueChanged(true);
    }
#endif

wxs2IntProperty::wxs2IntProperty(wxsProperties* Properties,int& Int1,int& Int2, bool AlwaysUpdate):
    wxsProperty(Properties), Value1(Int1), Value2(Int2), AlwUpd(AlwaysUpdate), 
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0),
        Val1Id(0),
        Val2Id(0)
    #endif
{
	//ctor
}

wxs2IntProperty::~wxs2IntProperty()
{
	//dtor
}

const wxString& wxs2IntProperty::GetTypeName()
{
    static wxString Name(_T("2 x int"));
    return Name;
}

#ifdef __NO_PROPGRGID

    wxWindow* wxs2IntProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxs2IntPropertyWindow(Parent,this);
    }
    
    void wxs2IntProperty::UpdateEditWindow()
    {
        if ( Window ) Window->SetValues(Value1,Value2);
    }

#else

    void wxs2IntProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
        PGId   = Grid->Append( wxParentProperty(Name,wxPG_LABEL) );
        Val1Id = Grid->AppendIn(PGId, wxIntProperty(_("X"), wxPG_LABEL, Value1) );
        Val2Id = Grid->AppendIn(PGId, wxIntProperty(_("Y"), wxPG_LABEL, Value2) );
    }
    
    bool wxs2IntProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
        if ( (Id == Val1Id) || (Id == Val2Id) || (Id == PGId) )
        {
        	Value1 = Grid->GetPropertyValue(Val1Id).GetLong();
        	Value2 = Grid->GetPropertyValue(Val2Id).GetLong();
        	int Cor1 = Value1;
        	int Cor2 = Value2;
        	CorrectValues(Value1,Value2);
        	if ( Value1 != Cor1 )
        	{
        		Grid->SetPropertyValue(Val1Id,Value1);
        	}
        	if ( Value2 != Cor2 )
        	{
        		Grid->SetPropertyValue(Val2Id,Value2);
        	}
        	return ValueChanged(true);
        }
        return true;
    }
    
    void wxs2IntProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
        Grid->SetPropertyValue(Val1Id,Value1);
        Grid->SetPropertyValue(Val2Id,Value2);
    }
    
#endif
