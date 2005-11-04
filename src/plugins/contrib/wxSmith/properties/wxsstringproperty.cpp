#include "../wxsheaders.h"
#include "wxsstringproperty.h"

#include <wx/textctrl.h>

#ifdef __NO_PROPGRGID

    class WXSCLASS wxsStringPropertyWindow: public wxTextCtrl
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
            Prop->ValueChanged(false);
        }
    }

    void wxsStringPropertyWindow::OnTextEnter(wxCommandEvent& event)
    {
        wxStrting Cor = GetValue();
        Prop->Value = CorrectString(Cor);
        if ( Cor != Prop->Value )
        {
            SetValue(Prop->Value);
        }
        Prop->ValueChanged(true);
    }

    void wxsStringPropertyWindow::OnKillFocus(wxFocusEvent& event)
    {
        wxStrting Cor = GetValue();
        Prop->Value = CorrectString(Cor);
        if ( Cor != Prop->Value )
        {
            SetValue(Prop->Value);
        }
        Prop->ValueChanged(true);
    }

#endif

wxsStringProperty::wxsStringProperty(wxsProperties* Properties,wxString& String, bool AlwaysUpdate,bool Long):
    wxsProperty(Properties), Value(String), AlwUpd(AlwaysUpdate), IsLong(Long),
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0)
    #endif
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

#ifdef __NO_PROPGRGID

    wxWindow* wxsStringProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxsStringPropertyWindow(Parent,this);
    }

    void wxsStringProperty::UpdateEditWindow()
    {
        if ( Window ) Window->SetValue(Value);

    }

#else

    void wxsStringProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	if ( IsLong )
    	{
    		wxString Str = Value;
    		Str.Replace(_T("\n"),_T("\\n"));
    		PGId = Grid->Append( wxLongStringProperty(Name,wxPG_LABEL,Str) );
    	}
    	else
    	{
            PGId = Grid->Append(Name,wxPG_LABEL,Value);
    	}
    }

    bool wxsStringProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
    	if ( Id == PGId )
    	{
    		wxString Cor = Grid->GetPropertyValue(Id).GetString();
    		if ( IsLong )
    		{
// FIXME (SpOoN#1#): This won't work properly when using '\n' in entered text but this is in fact bug in wxPropertyGrid
    			Cor.Replace(_T("\\n"),_T("\n"));
    		}
    		Value = CorrectValue(Cor);
    		if ( Value != Cor )
    		{
    			Grid->SetPropertyValue(Id,Value);
    		}
    		return ValueChanged(true);
    	}
    	return true;
    }

    void wxsStringProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
        wxString Str = Value;
        if ( IsLong )
        {
             Str.Replace(_T("\n"),_T("\\n"));
        }
        Grid->SetPropertyValue(PGId,Str);
    }

#endif
