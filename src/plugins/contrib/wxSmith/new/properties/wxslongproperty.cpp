#include "wxslongproperty.h"

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,long)

wxsLongProperty::wxsLongProperty(const wxString& PGName, const wxString& DataName,long _Offset,long _Default):
    wxsProperty(PGName,DataName),
    Offset(_Offset),
    Default(_Default)
{}


void wxsLongProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    PGRegister(Object,Grid,Grid->AppendIn(Parent,wxIntProperty(GetPGName(),wxPG_LABEL,VALUE)));
}

bool wxsLongProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsLongProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    Grid->SetPropertyValue(Id,VALUE);
    return true;
}

bool wxsLongProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE = Default;
        return false;
    }
    TiXmlText* Text = Element->FirstChild()->ToText();
    if ( !Text )
    {
        VALUE = Default;
        return false;
    }
    VALUE = atoi(Text->Value());
    return true;
}

bool wxsLongProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != Default )
    {
        char Buffer[0x40];  // Using char instead of wxChar because TiXml uses it
        Element->InsertEndChild(TiXmlText(ltoa(VALUE,Buffer,10)));
        return true;
    }
    return false;
}

bool wxsLongProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),VALUE,Default);
}

bool wxsLongProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),VALUE,Default);
}
