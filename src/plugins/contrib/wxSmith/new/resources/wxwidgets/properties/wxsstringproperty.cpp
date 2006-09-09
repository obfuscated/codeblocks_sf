#include "wxsstringproperty.h"

#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxString)

// TODO (SpOoN##): Fix \n handling


wxsStringProperty::wxsStringProperty(const wxString& PGName, const wxString& DataName,long _Offset,bool _IsLongString,bool _XmlStoreEmpty,const wxString& _Default):
    wxsProperty(PGName,DataName),
    Offset(_Offset),
    IsLongString(_IsLongString),
    XmlStoreEmpty(_XmlStoreEmpty),
    Default(_Default)
{}


void wxsStringProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxString Fixed = VALUE;
    Fixed.Replace(_T("\n"),_T("\\n"));
    wxPGId Id;
    if ( IsLongString )
    {
        Id = Grid->AppendIn(Parent,wxLongStringProperty(GetPGName(),wxPG_LABEL,Fixed));
    }
    else
    {
        Id = Grid->AppendIn(Parent,wxStringProperty(GetPGName(),wxPG_LABEL,Fixed));
    }
    PGRegister(Object,Grid,Id);
}

bool wxsStringProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetString();
    VALUE.Replace(_T("\\n"),_T("\n"));
    return true;
}

bool wxsStringProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    wxString Fixed = VALUE;
    Fixed.Replace(_T("\n"),_T("\\n"));
    Grid->SetPropertyValue(Id,Fixed);
    return true;
}

bool wxsStringProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE.Clear();
        return false;
    }
    TiXmlText* Text = Element->FirstChild()->ToText();
    if ( !Text ) 
    {
        // There was element for this property but it was empty
        VALUE = wxEmptyString;
        return true;
    }
    VALUE = cbC2U(Text->Value());
    return true;
}

bool wxsStringProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( XmlStoreEmpty || (VALUE != Default) )
    {
        Element->InsertEndChild(TiXmlText(cbU2C(VALUE)));
        return true;
    }
    return false;
}

bool wxsStringProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetString(GetDataName(),VALUE,Default);
}

bool wxsStringProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutString(GetDataName(),VALUE,Default);
}
