#include "../wxsheaders.h"
#include "wxsboolproperty.h"

#include <wx/checkbox.h>

#ifdef __NO_PROPGRGID

    class WXSCLASS wxsBoolPropertyWindow: public wxCheckBox
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
        wxCheckBox(Parent,-1,_T("")),
        Prop(Property)
    {
    }

    wxsBoolPropertyWindow::~wxsBoolPropertyWindow()
    {
    }

    void wxsBoolPropertyWindow::OnChange(wxCommandEvent& event)
    {
        Prop->Value = GetValue();
        Prop->ValueChanged(true);
    }
#endif

wxsBoolProperty::wxsBoolProperty(wxsProperties* Properties,bool& Bool):
    wxsProperty(Properties), Value(Bool),
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0)
    #endif
{
	//ctor
}

wxsBoolProperty::~wxsBoolProperty()
{
	//dtor
}

const wxString& wxsBoolProperty::GetTypeName()
{
    static wxString Name(_T("bool"));
    return Name;
}

#ifdef __NO_PROPGRGID

    wxWindow* wxsBoolProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxsBoolPropertyWindow(Parent,this);
    }

    void wxsBoolProperty::UpdateEditWindow()
    {
        if ( Window ) Window->SetValue(Value);
    }

#else

    void wxsBoolProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
        PGId = Grid->Append(Name,wxPG_LABEL,Value);
        Grid->SetPropertyAttribute(PGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    }

    bool wxsBoolProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
        if ( Id == PGId )
        {
        	Value = Grid->GetPropertyValue(Id).GetBool();
        	return ValueChanged(true);
        }
        return true;
    }

    void wxsBoolProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
        Grid->SetPropertyValue(PGId,Value);
    }

#endif
