#include "wxsarraystringproperty.h"
#include "wxsarraystringeditordlg.h"

#include <globals.h>

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,wxArrayString)

wxsArrayStringProperty::wxsArrayStringProperty(const wxString& PGName,const wxString& DataName,const wxString& _DataSubName,long _Offset):
    wxsCustomEditorProperty(PGName,DataName),
    Offset(_Offset),
    DataSubName(_DataSubName)
{}

bool wxsArrayStringProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsArrayStringEditorDlg Dlg(NULL,VALUE);
    return Dlg.ShowModal() == wxID_OK;
}

bool wxsArrayStringProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    VALUE.Clear();
    
    if ( !Element )
    {
        return false;
    }
    
    for ( TiXmlElement* Item = Element->FirstChildElement(cbU2C(DataSubName));
          Item;
          Item = Item->NextSiblingElement(cbU2C(DataSubName)) )
    {
        TiXmlText* Text = Item->FirstChild()->ToText();
        if ( Text )
        {
            VALUE.Add(cbC2U(Text->Value()));
        }
        else
        {
            // There was no child text node, that's the case of empty string
            VALUE.Add(wxEmptyString);
        }
    }
    return true;
}

bool wxsArrayStringProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    size_t Count = VALUE.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        XmlSetString(Element,VALUE[i],DataSubName);
    }
    return Count != 0;
}

bool wxsArrayStringProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    VALUE.Clear();
    Stream->SubCategory(GetDataName());
    for(;;)
    {
        wxString Item;
        if ( !Stream->GetString(DataSubName,Item,wxEmptyString) ) break;
        VALUE.Add(Item);
    }
    Stream->PopCategory();
    return true;
}

bool wxsArrayStringProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    Stream->SubCategory(GetDataName());
    size_t Count = VALUE.GetCount();
    for ( size_t i=0; i<Count; i++ )
    {
        Stream->PutString(DataSubName,VALUE[i],wxEmptyString);
    }
    Stream->PopCategory();
    return true;
}

wxString wxsArrayStringProperty::GetStr(wxsPropertyContainer* Object)
{
    wxString Result;
    size_t Count = VALUE.Count();
    
    if ( Count == 0 )
    {
        return _("Click to add items");        
    }
    
    for ( size_t i=0; i<Count; i++ )
    {
        wxString Item = VALUE[i];
        Item.Replace(_T("\""),_T("\\\""));
        if ( i > 0 ) 
        {
            Result.Append(_T(' '));
        }
        Result.Append(_T('"'));
        Result.Append(Item);
        Result.Append(_T('"'));
    }
    return Result;
}
