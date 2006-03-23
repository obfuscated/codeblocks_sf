#include "wxsproperty.h"
#include "wxspropertygridmanager.h"
#include "wxspropertycontainer.h"

#include <globals.h>

wxsProperty::wxsProperty(const wxString& _PGName, const wxString& _DataName):
    PGName(_PGName), DataName(_DataName)
{
}

long wxsProperty::PGRegister(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId ID,long Index)
{
    wxsPropertyGridManager* wxsGrid = wxDynamicCast(Grid,wxsPropertyGridManager);
    if ( !wxsGrid ) return -1;
    return wxsGrid->Register(Object,this,ID,Index);
}

long wxsProperty::GetPropertiesFlags(wxsPropertyContainer* Object)
{
    return Object->GetPropertiesFlags();
}

bool wxsProperty::XmlGetString(TiXmlElement* Element,wxString& Value,const wxString& SubChild)
{
    Value.Clear();
    
    if ( !Element ) 
    {
        return false;
    }
    
    if ( !SubChild.empty() )
    {
        Element = Element->FirstChildElement(cbU2C(SubChild));
        if ( !Element ) return false;
    }
    
    TiXmlText * Text = Element->FirstChild()->ToText();
    if ( !Text ) 
    {
        // Element does exist but doesn't contain text - in this
        // case we return true, because it's case of empty string
        return true;
    }
    Value = cbC2U(Text->Value());
    return true;
}

void wxsProperty::XmlSetString(TiXmlElement* Element,const wxString& Value,const wxString& SubChild)
{
    if ( !Element ) 
    {
        return;
    }
    
    if ( !SubChild.empty() )
    {
        Element = Element->InsertEndChild(TiXmlElement(cbU2C(SubChild)))->ToElement();
        if ( !Element ) return;
    }
    
    Element->InsertEndChild(TiXmlText(cbU2C(Value)));
}
