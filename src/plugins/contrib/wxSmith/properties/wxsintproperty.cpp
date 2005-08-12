#include "wxsintproperty.h"

#include <wx/textctrl.h>
#include <wx/msgdlg.h>

#ifdef __NO_PROPGRGID
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
            Prop->ValueChanged(false);
        }
    }
    
    void wxsIntPropertyWindow::OnTextEnter(wxCommandEvent& event)
    {
        long Val = 0;
        GetValue().ToLong(&Val);
        Prop->Value = Prop->CorrectValue(Val);
        SetValue(wxString::Format(_T("%d"),Prop->Value));
        Prop->ValueChanged(true);
    }
    
    void wxsIntPropertyWindow::OnKillFocus(wxFocusEvent& event)
    {
        long Val = 0;
        GetValue().ToLong(&Val);
        Prop->Value = Prop->CorrectValue(Val);
        SetValue(wxString::Format(_T("%d"),Prop->Value));
        Prop->ValueChanged(true);
    }

#endif
    
wxsIntProperty::wxsIntProperty(wxsProperties* Properties,int& Int, bool AlwaysUpdate):
    wxsProperty(Properties), Value(Int), AlwUpd(AlwaysUpdate), 
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0)
    #endif
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

#ifdef __NO_PROPGRGID

    wxWindow* wxsIntProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxsIntPropertyWindow(Parent,this);
    }
    
    void wxsIntProperty::UpdateEditWindow()
    {
        if ( Window ) Window->SetValue(wxString::Format(_T("%d"),Value));
    }
            
#else

    void wxsIntProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	PGId = Grid->Append(Name,wxPG_LABEL,Value);
    }
    
    /** Function notifying about property change */
    bool wxsIntProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
    	if ( Id == PGId )
    	{
    		Value = Grid->GetPropertyValue(Id).GetLong();
    		int Cor = Value;
    		Value = CorrectValue(Value);
    		if ( Value != Cor )
    		{
    			Grid->SetPropertyValue(Id,Value);
    		}
    		return ValueChanged(true);
    	}
    	return true;
    }
    
    /** Function updating value of this property insided property grid */
    void wxsIntProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
    	Grid->SetPropertyValue(PGId,Value);
    }
    
#endif
