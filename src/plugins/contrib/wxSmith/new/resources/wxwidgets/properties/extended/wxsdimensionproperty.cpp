#include "wxsdimensionproperty.h"

#include "../../wxsglobals.h"
#include <messagemanager.h>

#define WXS_DIM_VALUE   1
#define WXS_DIM_UNITS   2

// Helper macros for fetching variables
#define VALUE   wxsVARIABLE(Object,ValueOffset,long)
#define UNITS   wxsVARIABLE(Object,DialogUnitsOffset,bool)

wxsDimensionProperty::wxsDimensionProperty(
    const wxString&  PGName,
    const wxString& _PGDUName,
    const wxString&  DataName,
    long _ValueOffset,
    long _DialogUnitsOffset,
    long _Default,
    bool _DefaultDialogUnits):
        wxsProperty(PGName,DataName),
        ValueOffset(_ValueOffset),
        Default(_Default),
        DialogUnitsOffset(_DialogUnitsOffset),
        DefaultDialogUnits(_DefaultDialogUnits),
        PGDUName(_PGDUName)
{}

wxString wxsDimensionProperty::GetPixelsCode(long Value,bool DialogUnits,wxString ParentName,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( !DialogUnits ) return wxString::Format(_T("%d"),Value);
            return wxString::Format(_T("wxDLG_UNIT(%s,wxSize(%d,0)).GetWidth()"),ParentName.c_str(),Value);
        }
    }

    wxsLANGMSG(wxsDimensionProperty::GetPixelsCode,Language);
    return wxEmptyString;
}


void wxsDimensionProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId DUId;
    PGRegister(Object,Grid,Grid->AppendIn(Parent,wxIntProperty(GetPGName(),wxPG_LABEL,VALUE)),WXS_DIM_VALUE);
    PGRegister(Object,Grid,DUId = Grid->AppendIn(Parent,wxBoolProperty(PGDUName,wxPG_LABEL,UNITS)),WXS_DIM_UNITS);
    Grid->SetPropertyAttribute(DUId,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
}

bool wxsDimensionProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case WXS_DIM_VALUE:
            VALUE = Grid->GetPropertyValue(Id).GetLong();
            return true;

        case WXS_DIM_UNITS:
            UNITS = Grid->GetPropertyValue(Id).GetBool();
            return true;
    }
    return false;
}

bool wxsDimensionProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case WXS_DIM_VALUE:
            Grid->SetPropertyValue(Id,VALUE);
            return true;

        case WXS_DIM_UNITS:
            Grid->SetPropertyValue(Id,UNITS);
            return true;
    }
    return false;
}

bool wxsDimensionProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE = Default;
        UNITS = DefaultDialogUnits;
        return false;
    }
    TiXmlText* Text = Element->FirstChild()->ToText();
    if ( !Text )
    {
        VALUE = Default;
        UNITS = DefaultDialogUnits;
        return false;
    }
    const char* Ptr = Text->Value();

    // 'd' character at the end of string means this value is in dialog units
    if ( Ptr[0] && Ptr[strlen(Ptr)-1]=='d' )
    {
        UNITS = true;
    }
    else
    {
        UNITS = false;
    }

    // atoi should cut off 'd' at the end
    VALUE = atol(Text->Value());
    return true;
}

bool wxsDimensionProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != Default || UNITS != DefaultDialogUnits )
    {
        char Buffer[0x40];  // Using char instead of wxChar because TiXml uses it

        ltoa(VALUE,Buffer,10);
        if ( UNITS )
        {
            strcat(Buffer,"d");
        }

        Element->InsertEndChild(TiXmlText(Buffer));
        return true;
    }
    return false;
}

bool wxsDimensionProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( !Stream->GetLong(_T("value"),VALUE,Default) ) Ret = false;
    if ( !Stream->GetBool(_T("dialogunits"),UNITS,DefaultDialogUnits) ) Ret = false;
    Stream->PopCategory();
    return Ret;
}

bool wxsDimensionProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( !Stream->PutLong(_T("value"),VALUE,Default) ) Ret = false;
    if ( !Stream->PutBool(_T("dialogunits"),UNITS,DefaultDialogUnits) ) Ret = false;
    Stream->PopCategory();
    return Ret;
}
