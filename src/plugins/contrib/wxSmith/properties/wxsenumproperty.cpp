#include "wxsenumproperty.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <vector>

#ifdef __NO_PROPGRGID

    class wxsEnumPropertyWindow: public wxChoice
    {
        public:
        
            wxsEnumPropertyWindow(wxWindow* Parent,wxsEnumProperty* Property):
                wxChoice(Parent,-1),
                Prop(Property),
                Count(0)
            {
            	const wxChar** PtrN = Prop->Names;
            	const long* PtrV = Prop->Values;
            	const wxChar* Selection = _T("");
            	while ( *PtrN )
            	{
            		if ( Prop->Value == *PtrV )
            		{
            			Selection = *PtrN;
            		}
            		
            		Append(*PtrN);
            		PtrN++;
            		PtrV++;
            		Count++;
            	}
            	SetStringSelection(Selection);
            }
            
            void UpdateValue()
            {
            	Count = 0;
            	const wxChar** PtrN = Prop->Names;
            	const long* PtrV = Prop->Values;
            	const wxChar* Selection = _T("");
            	while ( *PtrN )
            	{
            		if ( Prop->Value == *PtrV )
            		{
            			Selection = *PtrN;
            		}
            		
            		PtrN++;
            		PtrV++;
            		Count++;
            	}
            	SetStringSelection(Selection);
            }
            
        private:
        
            void OnChoice(wxCommandEvent& event)
            {
            	int Index = GetSelection();
            	if ( Index < 0 || Index > Count )
            	{
            		Prop->Value = 0;
            	}
            	else
            	{
            		Prop->Value = Prop->Values[Index];
            	}
            	Prop->ValueChanged(true);
            }
        
            wxsEnumProperty* Prop;
            int Count;
            
            DECLARE_EVENT_TABLE()
    };
    
    BEGIN_EVENT_TABLE(wxsEnumPropertyWindow,wxChoice)
        EVT_CHOICE(wxID_ANY,wxsEnumPropertyWindow::OnChoice)
    END_EVENT_TABLE()
    
#endif

wxsEnumProperty::wxsEnumProperty(wxsProperties* Properties,int &_Value,const wxChar** _Names,const long* _Values):
    wxsProperty(Properties),
    Value(_Value),
    Names(_Names),
    Values(_Values),
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0)
    #endif
{
}

wxsEnumProperty::~wxsEnumProperty()
{
}

const wxString& wxsEnumProperty::GetTypeName()
{
    static wxString Name(_T("enum property"));
    return Name;
}

#ifdef __NO_PROPGRGID

    wxWindow* wxsEnumProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxsEnumPropertyWindow(Parent,this);
    }
    
    void wxsEnumProperty::UpdateEditWindow()
    {
        if ( Window ) Window->UpdateValue();
    }
            
#else

    void wxsEnumProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	PGId = Grid->Append( wxEnumProperty(Name,wxPG_LABEL,Names,Values,0,Value) );
    }
    
    bool wxsEnumProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
    	if ( Id == PGId )
    	{
    		Value = Grid->GetPropertyValue(Id).GetLong();
    		return ValueChanged(true);
    	}
    	return true;
    }
    
    void wxsEnumProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
    	Grid->SetPropertyValue(PGId,Value);
    }

#endif
