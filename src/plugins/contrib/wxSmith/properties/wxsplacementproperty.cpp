#include "wxsplacementproperty.h"

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include "../wxsdefsizer.h"

#ifdef __NO_PROPGRGID

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
        Pos[0] = new wxRadioButton(this,-1,_T(""),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
        for (int i=1; i<9; i++ )
        {
            Pos[i] = new wxRadioButton(this,-1,_T(""));
        }
    
        Exp = new wxCheckBox(this,-1,_("Expand"));
        Shap = new wxCheckBox(this,-1,_("Shaped"));
    
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
                NewPlacement = wxsSizerExtraParams::LeftTop + i;
            }
        }
        
        assert(Object != NULL);
        
        Object->PlacementType = NewPlacement;
        Object->Expand = Exp->GetValue();
        Object->Shaped = Shap->GetValue();
        Object->ValueChanged(true);
    }
    
    void wxsPlacementPropertyWindow::UpdateData()
    {
        assert ( Object != NULL );
        
        int Placement = Object->PlacementType;
    
        for ( int i=0; i<9; i++ )
            Pos[i]->SetValue(Placement == wxsSizerExtraParams::LeftTop + i );
            
        Exp->SetValue(Object->Expand);
        Shap->SetValue(Object->Shaped);
    }
    
#endif

wxsPlacementProperty::wxsPlacementProperty(wxsProperties* Properties,int& Placement,bool& _Expand,bool& _Shaped):
    wxsProperty(Properties),
    PlacementType(Placement),
    Expand(_Expand),
    Shaped(_Shaped),
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0),
        ExpandId(0),
        ShapedId(0)
    #endif
{
	//ctor
}

wxsPlacementProperty::~wxsPlacementProperty()
{
	//dtor
}

const wxString& wxsPlacementProperty::GetTypeName()
{
    static wxString Type(_T("Widget Placement"));
    return Type;
}

#ifdef __NO_PROPGRGID

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

#else

    void wxsPlacementProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	static const wxChar* Placements[] =
    	{
    		_("Left - Top"),
    		_("Top"),
    		_("Right - Top"),
    		_("Right"),
    		_("Right - Bottom"),
    		_("Bottom"),
    		_("Left - Bottom"),
    		_("Left"),
    		_("Center"),
    		NULL
    	};
    	
    	static long Values[] =
    	{
    		wxsSizerExtraParams::LeftTop,
    		wxsSizerExtraParams::Top,
            wxsSizerExtraParams::RightTop,
            wxsSizerExtraParams::Right,
            wxsSizerExtraParams::RightBottom,
            wxsSizerExtraParams::Bottom,
            wxsSizerExtraParams::LeftBottom,
            wxsSizerExtraParams::Left,
            wxsSizerExtraParams::Center
    	};
    	
    	PGId = Grid->Append( wxEnumProperty(Name,wxPG_LABEL,Placements,Values,0,PlacementType) );
    	ExpandId = Grid->Append( wxBoolProperty( _("Expand"), wxPG_LABEL, Expand ) );
    	ShapedId = Grid->Append( wxBoolProperty( _("Shaped"), wxPG_LABEL, Shaped ) );
        Grid->SetPropertyAttribute(ExpandId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
        Grid->SetPropertyAttribute(ShapedId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    }
            
    bool wxsPlacementProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
    	if ( Id == PGId || Id == ExpandId || Id == ShapedId )
    	{
    		PlacementType = Grid->GetPropertyValue(PGId).GetLong();
    		Expand = Grid->GetPropertyValue(ExpandId).GetBool();
    		Shaped = Grid->GetPropertyValue(ShapedId).GetBool();
    		return ValueChanged(true);
    	}
    	return true;
    }
            
    void wxsPlacementProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
    	Grid->SetPropertyValue(PGId,PlacementType);
    	Grid->SetPropertyValue(ExpandId,Expand);
    	Grid->SetPropertyValue(ShapedId,Shaped);
    }

#endif
