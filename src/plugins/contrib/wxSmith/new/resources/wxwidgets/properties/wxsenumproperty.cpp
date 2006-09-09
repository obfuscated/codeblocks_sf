#include "wxsenumproperty.h"

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,long)

wxsEnumProperty::wxsEnumProperty(const wxString& PGName, const wxString& DataName,long _Offset,const long* _Values,const wxChar** _Names,bool _UpdateEnteries,long _Default,bool _UseNamesInXml):
    wxsProperty(PGName,DataName),
    Offset(_Offset),
    Default(_Default),
    UpdateEnteries(_UpdateEnteries),
    Values(_Values),
    Names(_Names),
    UseNamesInXml(_UseNamesInXml)
{}


void wxsEnumProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGConstants PGC(Names,Values);
    PGRegister(Object,Grid,Grid->AppendIn(Parent,wxEnumProperty(GetPGName(),wxPG_LABEL,PGC,VALUE)));
}

bool wxsEnumProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    VALUE = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsEnumProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    if ( UpdateEnteries )
    {
        Grid->GetPropertyChoices(Id).Set(Names,Values);
    }
    Grid->SetPropertyValue(Id,VALUE);
    return true;
}

bool wxsEnumProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
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
    if ( UseNamesInXml )
    {
        // Searching for node text in names
        wxString TextS = cbC2U(Text->Value());
        int i = 0;
        for ( const wxChar** Ptr = Names; *Ptr; Ptr++, i++ )
        {
            if ( TextS == *Ptr )
            {
                VALUE = Values[i];
                return true;
            }
        }
        VALUE = Default;
        return false;;
    }
    else
    {
        VALUE = atoi(Text->Value());
    }
    return true;
}

bool wxsEnumProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE != Default )
    {
        if ( UseNamesInXml )
        {
            // searching for name of this value
            int i = 0;
            for ( const wxChar** Ptr = Names; *Ptr; Ptr++, i++ )
            {
                if ( VALUE == Values[i] )
                {
                    Element->InsertEndChild(TiXmlElement(cbU2C(*Ptr)));
                    return true;
                }
            }

            // Did not found value, storing as integer
        }

        char Buffer[0x40];  // Using char instead of wxChar because TiXml uses it
        Element->InsertEndChild(TiXmlText(ltoa(VALUE,Buffer,10)));
        return true;
    }
    return false;
}

bool wxsEnumProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),VALUE,Default);
}

bool wxsEnumProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),VALUE,Default);
}
