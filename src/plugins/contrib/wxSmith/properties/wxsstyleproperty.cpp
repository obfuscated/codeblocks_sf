#include "wxsstyleproperty.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <vector>


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
                for ( ; Styles->Name; Styles++ )
                {
                    if ( !Styles->Value )
                    {
                        // Adding dividing text
                        wxStaticText* ST = new wxStaticText(this,-1,Styles->Name);
                        wxFont Font = ST->GetFont();
                        Font.SetWeight(wxBOLD);
                        Font.SetUnderlined(true);
                        ST->SetFont(Font);
                        Sizer->Add(ST);
                    }
                    else
                    {
                        IdToStyleMaps.push_back(Styles->Value);
                        wxCheckBox* CB = new wxCheckBox(this,CurrentId++,Styles->Name);
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
            Prop->ValueChanged();
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

wxsStyleProperty::wxsStyleProperty(wxsProperties* Properties,int &_Style,wxsStyle *_Styles):
    wxsProperty(Properties),
    Style(_Style),
    Styles(_Styles),
    Window(NULL)
{
}

wxsStyleProperty::~wxsStyleProperty()
{
}

const wxString& wxsStyleProperty::GetTypeName()
{
    static wxString Name(wxT("widget style"));
    return Name;
}

wxWindow* wxsStyleProperty::BuildEditWindow(wxWindow* Parent)
{
    return Window = new wxsStylePropertyWindow(Parent,this,Styles);
}

void wxsStyleProperty::UpdateEditWindow()
{
    if ( Window ) Window->UpdateValues();
}
        
