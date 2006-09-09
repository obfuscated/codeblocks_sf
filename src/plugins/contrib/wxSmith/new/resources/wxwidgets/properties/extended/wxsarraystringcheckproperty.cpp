#include "wxsarraystringcheckproperty.h"
#include "wxsarraystringcheckeditordlg.h"

#include <globals.h>

// Helper macros for fetching variables
#define VALUE   wxsVARIABLE(Object,Offset,wxArrayString)
#define CHECK   wxsVARIABLE(Object,CheckOffset,wxArrayBool)

wxsArrayStringCheckProperty::wxsArrayStringCheckProperty(const wxString& PGName,const wxString& DataName,const wxString& _DataSubName,long _Offset,long _CheckOffset):
    wxsCustomEditorProperty(PGName,DataName),
    Offset(_Offset),
    CheckOffset(_CheckOffset),
    DataSubName(_DataSubName)
{}

bool wxsArrayStringCheckProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsArrayStringCheckEditorDlg Dlg(NULL,VALUE,CHECK);
    return Dlg.ShowModal() == wxID_OK;
}

bool wxsArrayStringCheckProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    VALUE.Clear();
    CHECK.Clear();
    
    if ( !Element ) 
    {
        return false;
    }
    
    for ( TiXmlElement* Item = Element->FirstChildElement(cbU2C(DataSubName));
          Item;
          Item = Item->NextSiblingElement(cbU2C(DataSubName)) )
    {
        // Fetching value of "checked" flag
        const char* CheckedAttr = Item->Attribute("checked");
        CHECK.Add( CheckedAttr && CheckedAttr[0]=='1' && CheckedAttr[1]=='\0' );
        
        // Fetching item text
        TiXmlText* Text = Item->FirstChild()->ToText();
        if ( Text )
        {
            VALUE.Add(cbC2U(Text->Value()));
        }
        else
        {
            VALUE.Add(wxEmptyString);
        }
    }
    return true;
}

bool wxsArrayStringCheckProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    size_t Count = VALUE.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        TiXmlNode* End = Element->InsertEndChild(TiXmlElement(cbU2C(DataSubName)));
        if ( End )
        {
            if ( CHECK[i] )
            {
                End->ToElement()->SetAttribute("checked","1");
            }
            End->InsertEndChild(TiXmlText(cbU2C(VALUE[i])));
        }
    }
    return Count != 0;
}

bool wxsArrayStringCheckProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    VALUE.Clear();
    Stream->SubCategory(GetDataName());
    for(;;)
    {
        wxString Item;
        if ( !Stream->GetString(DataSubName,Item,wxEmptyString) ) break;
        VALUE.Add(Item);
        
        bool Checked;
        Stream->GetBool(DataSubName + _T("_checked"),Checked,false);
        CHECK.Add(Checked);
    }
    Stream->PopCategory();
    return true;
}

bool wxsArrayStringCheckProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    Stream->SubCategory(GetDataName());
    size_t Count = VALUE.GetCount();
    for ( size_t i=0; i<Count; i++ )
    {
        Stream->PutString(DataSubName,VALUE[i],wxEmptyString);
        Stream->PutBool(DataSubName + _T("_checked"),CHECK[i],false);
    }
    Stream->PopCategory();
    return true;
}

wxString wxsArrayStringCheckProperty::GetStr(wxsPropertyContainer* Object)
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
        if ( CHECK[i] )
        {
            Result.Append(_T('['));
        }
        Result.Append(_T('"'));
        Result.Append(Item);
        Result.Append(_T('"'));
        if ( CHECK[i] )
        {
            Result.Append(_T(']'));
        }
    }
    return Result;
}
