#include "wxsstyleproperty.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <vector>

#ifdef __NO_PROPGRGID

    #define WXS_MAX_STYLE_LEN   15
    #define WXSSPW_FIRST_ID     0x1010
    
    class wxsStylePropertyWindow: public wxPanel
    {
        public:
            wxsStylePropertyWindow(wxWindow* Parent,wxsStyleProperty* Property,wxsStyle* Styles):
                wxPanel(Parent,-1),
                Prop(Property),
                StopUpdate(true)
            {
                wxSizer* Sizer = new wxFlexGridSizer(1,2,2);
                
                wxWindowID CurrentId = WXSSPW_FIRST_ID; 
                
                if ( Styles )
                {
                    for ( ; Styles->Name.Length(); Styles++ )
                    {
                        if ( Styles->Value == ((unsigned int)-1) )
                        {
                            // Adding dividing text
                            wxStaticText* ST = new wxStaticText(this,-1,Styles->Name);
                            wxFont Font = ST->GetFont();
                            Font.SetWeight(wxBOLD);
                            Font.SetUnderlined(true);
                            ST->SetFont(Font);
                            Sizer->Add(ST);
                        }
                        else if ( Styles->Value )
                        {
                            IdToStyleMaps.push_back(Styles->Value);
                            wxString Name = Styles->Name;
                            bool IsToolTip = false;
                            if ( Name.Length() > WXS_MAX_STYLE_LEN )
                            {
                                Name = Name.Mid(0,WXS_MAX_STYLE_LEN-3) + _T("...");
                                IsToolTip = true;
                            }
                            wxCheckBox* CB = new wxCheckBox(this,CurrentId++,Name);
                            if ( IsToolTip )
                            {
                                CB->SetToolTip(Styles->Name);
                            }
                            Sizer->Add(CB);
                        }
                    }
                }
                
                SetSizer(Sizer);
                Sizer->SetSizeHints(this);
                
                UpdateValues();
                
                StopUpdate = false;
            }
            
            void UpdateValues()
            {
                for ( int i=IdToStyleMaps.size(); --i >= 0; )
                {
                    wxCheckBox* CP = (wxCheckBox*)FindWindow(WXSSPW_FIRST_ID+i);
                    if ( !CP ) continue;
                    int StyleVal = IdToStyleMaps[i];
                    CP->SetValue( (Prop->Style&StyleVal) == StyleVal );
                }
            }
            
        private:
        
            void OnChange(wxCommandEvent& event)
            {
                if ( StopUpdate ) return;
                StopUpdate = true;
                int NewStyle = 0;
                for ( int i=IdToStyleMaps.size(); --i >= 0; )
                {
                    wxCheckBox* CP = (wxCheckBox*)FindWindow(WXSSPW_FIRST_ID+i);
                    if ( !CP ) continue;
                    if ( CP->GetValue() )
                    {
                        NewStyle |= IdToStyleMaps[i];
                    }
                }
                Prop->Style = NewStyle;
                Prop->ValueChanged(true);
                StopUpdate = false;
            }
        
            wxsStyleProperty* Prop;
            std::vector<int> IdToStyleMaps;
            bool StopUpdate;
            
            DECLARE_EVENT_TABLE()
    };
    
    BEGIN_EVENT_TABLE(wxsStylePropertyWindow,wxPanel)
        EVT_CHECKBOX(wxID_ANY,wxsStylePropertyWindow::OnChange)
    END_EVENT_TABLE()
    
#endif

wxsStyleProperty::wxsStyleProperty(wxsProperties* Properties,int &_Style,wxsStyle *_Styles):
    wxsProperty(Properties),
    Style(_Style),
    Styles(_Styles),
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0)
    #endif
{
}

wxsStyleProperty::~wxsStyleProperty()
{
}

const wxString& wxsStyleProperty::GetTypeName()
{
    static wxString Name(_T("widget style"));
    return Name;
}

#ifdef __NO_PROPGRGID

    wxWindow* wxsStyleProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxsStylePropertyWindow(Parent,this,Styles);
    }
    
    void wxsStyleProperty::UpdateEditWindow()
    {
        if ( Window ) Window->UpdateValues();
    }
            
#else

    void wxsStyleProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	wxPGConstants Consts;
    	
    	for ( wxsStyle* St = Styles; St->Name.Length(); ++St )
    	{
    		if ( St->Value != ((unsigned int)-1) )
    		{
    			Consts.Add(St->Name,St->Value);
    		}
    	}
    	
    	PGId = Grid->Append( wxFlagsProperty(Name,wxPG_LABEL,Consts,Style) );
        Grid->SetPropertyAttribute(PGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    }
    
    bool wxsStyleProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
    	if ( Id == PGId )
    	{
    		Style = Grid->GetPropertyValue(Id).GetLong();
    		return ValueChanged(true);
    	}
    	return true;
    }
    
    void wxsStyleProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
    	Grid->SetPropertyValue(PGId,Style);
    }

#endif
