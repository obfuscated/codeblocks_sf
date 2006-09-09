#include "wxsboolproperty.h"

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,bool)

wxsBoolProperty::wxsBoolProperty(const wxString& PGName,const wxString& DataName,long _Offset,bool _Default):
    wxsProperty(PGName,DataName),
    Offset(_Offset),
    Default(_Default)
{}


void wxsBoolProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId ID = Grid->AppendIn(Parent,wxBoolProperty(GetPGName(),wxPG_LABEL,VALUE));
    Grid->SetPropertyAttribute(ID,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
    PGRegister(Object,Grid,ID);
}

bool wxsBoolProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetBool();
    return true;
}

bool wxsBoolProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    Grid->SetPropertyValue(Id,VALUE);
    return true;
}

bool wxsBoolProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
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
    VALUE = atoi(Text->Value()) != 0;
    return true;
}

bool wxsBoolProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != Default )
    {
        Element->InsertEndChild(TiXmlText(VALUE?"1":"0"));
        return true;
    }
    return false;
}

bool wxsBoolProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetBool(GetDataName(),VALUE,Default);
}

bool wxsBoolProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutBool(GetDataName(),VALUE,Default);
}
