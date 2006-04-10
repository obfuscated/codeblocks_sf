#include "wxsflagsproperty.h"

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,long)

wxsFlagsProperty::wxsFlagsProperty(const wxString& PGName, const wxString& DataName,long _Offset,const long* _Values,const wxChar** _Names,bool _UpdateEnteries,long _Default):
    wxsProperty(PGName,DataName),
    Offset(_Offset),
    Default(_Default),
    UpdateEnteries(_UpdateEnteries),
    Values(_Values),
    Names(_Names)
{}


void wxsFlagsProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGConstants PGC(Names,Values);
    PGRegister(Object,Grid,Grid->AppendIn(Parent,wxFlagsProperty(GetPGName(),wxPG_LABEL,PGC,VALUE)));
}

bool wxsFlagsProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsFlagsProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    if ( UpdateEnteries )
    {
        Grid->GetPropertyChoices(Id).Set(Names,Values);
    }
    Grid->SetPropertyValue(Id,VALUE);
    return true;
}

bool wxsFlagsProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
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
    VALUE = atol(Text->Value());
    return true;
}

bool wxsFlagsProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != Default )
    {
        char Buffer[0x40];  // Using char instead of wxChar because TiXml uses it
        Element->InsertEndChild(TiXmlText(ltoa(VALUE,Buffer,10)));
        return true;
    }
    return false;
}

bool wxsFlagsProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),VALUE,Default);
}

bool wxsFlagsProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),VALUE,Default);
}
