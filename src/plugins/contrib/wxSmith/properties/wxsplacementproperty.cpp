#include "wxsplacementproperty.h"

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include "../widget.h"

class wxsPlacementPropertyWindow: public wxPanel
{
    public:
        wxsPlacementPropertyWindow(wxWindow* Parent,wxsPlacementProperty* Object);
        virtual ~wxsPlacementPropertyWindow();
        
        void UpdateData();
        
    private:

        void OnButtonChanged(wxCommandEvent& event);
        
        wxRadioButton* Pos[9];
        wxCheckBox*    Exp;
        wxCheckBox*    Shap;
    
        wxsPlacementProperty* Object;
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsPlacementPropertyWindow,wxPanel)
    EVT_CHECKBOX(wxID_ANY,wxsPlacementPropertyWindow::OnButtonChanged)
    EVT_RADIOBUTTON(wxID_ANY,wxsPlacementPropertyWindow::OnButtonChanged)
END_EVENT_TABLE()

wxsPlacementPropertyWindow::wxsPlacementPropertyWindow(wxWindow* Parent,wxsPlacementProperty* _Object):
    wxPanel(Parent,-1),
    Object(_Object)
{
    Pos[0] = new wxRadioButton(this,-1,"",wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
    for (int i=1; i<9; i++ )
    {
        Pos[i] = new wxRadioButton(this,-1,"");
    }

    Exp = new wxCheckBox(this,-1,"Expand");
    Shap = new wxCheckBox(this,-1,"Shaped");

    wxFlexGridSizer* Sizer1 = new wxFlexGridSizer(3,1,1);
    for ( int i=0; i<9; i++ )
        Sizer1->Add(Pos[i]);

    wxFlexGridSizer* Sizer2 = new wxFlexGridSizer(1,1,1);
    Sizer2->Add(Exp);
    Sizer2->Add(Shap);
    
    wxFlexGridSizer* Sizer3 = new wxFlexGridSizer(3,1,1);
    Sizer3->Add(Sizer1,0,wxALIGN_CENTER_VERTICAL);
    Sizer3->Add(10,1);
    Sizer3->Add(Sizer2,0,wxALIGN_CENTER_VERTICAL);
    
    SetSizer(Sizer3);
    Sizer3->SetSizeHints(this);
}

wxsPlacementPropertyWindow::~wxsPlacementPropertyWindow()
{}

void wxsPlacementPropertyWindow::OnButtonChanged(wxCommandEvent& event)
{
    int NewPlacement = 0;
    for ( int i=0; i<9; i++ )
    {
        if ( Pos[i]->GetValue() )
        {
            NewPlacement = wxsWidgetBaseParams::LeftTop + i;
        }
    }
    
    assert(Object != NULL);
    
    Object->PlacementType = NewPlacement;
    Object->Expand = Exp->GetValue();
    Object->Shaped = Shap->GetValue();
    Object->ValueChanged();
}

void wxsPlacementPropertyWindow::UpdateData()
{
    assert ( Object != NULL );
    
    int Placement = Object->PlacementType;

    for ( int i=0; i<9; i++ )
        Pos[i]->SetValue(Placement == wxsWidgetBaseParams::LeftTop + i );
        
    Exp->SetValue(Object->Expand);
    Shap->SetValue(Object->Shaped);
}

wxsPlacementProperty::wxsPlacementProperty(wxsProperties* Properties,int& Placement,bool& _Expand,bool& _Shaped):
    wxsProperty(Properties),
    PlacementType(Placement),
    Expand(_Expand),
    Shaped(_Shaped)
{
	//ctor
}

wxsPlacementProperty::~wxsPlacementProperty()
{
	//dtor
}

const wxString& wxsPlacementProperty::GetTypeName()
{
    static wxString Type("Widget Placement");
    return Type;
}

wxWindow* wxsPlacementProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxsPlacementPropertyWindow(Parent,this);
}

void wxsPlacementProperty::UpdateEditWindow()
{
    if ( Window )
    {
        Window->UpdateData();
    }
}
