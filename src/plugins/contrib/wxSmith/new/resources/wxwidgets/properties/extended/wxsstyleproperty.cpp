#include "wxsstyleproperty.h"

#include <wx/tokenzr.h>
#include <globals.h>
#include <messagemanager.h>
#include "../../wxsglobals.h"

// Helper macro for fetching variables
#define STYLEBITS   wxsVARIABLE(Object,Offset,long)

wxsStyleProperty::wxsStyleProperty(
    const wxString&  StyleName,
    const wxString&  DataName,
    const wxsStyle* _StyleSet,
    long _Offset,
    const wxString& _Default,
    bool _IsExtra):
        wxsProperty(StyleName,DataName),
        StyleSet(_StyleSet),
        Offset(_Offset),
        IsExtra(_IsExtra)
{
    Default = ParseStringToBits(_Default);
    int StylesCount = 0;
    for ( const wxsStyle* St = StyleSet; !St->Name.empty(); St++ )
    {
        if ( !St->IsCategory() )
        {
            if ( St->IsExtra() == IsExtra )
            {
                StyleNames.Add(St->Name);
                StyleBits.Add(1L<<StylesCount);
                StyleFlags.Add(St->Flags);
                StylesCount++;
            }
        }
    }
    StyleNames.Shrink();
    StyleBits.Shrink();
}

void wxsStyleProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    if ( !StyleNames.empty() )
    {
        bool IsXrc = ( GetPropertiesFlags(Object) & (wxsFLFile|wxsFLMixed) ) != 0;
        wxPGConstants StyleConsts;
        if ( IsXrc )
        {
            size_t Count = StyleNames.Count();
            for ( size_t i = 0; i < Count; i++ )
            {
                if ( StyleFlags[i] & wxsSFXRC )
                {
                    StyleConsts.Add(StyleNames[i],StyleBits[i]);
                }
            }
        }
        else
        {
            StyleConsts.Add(StyleNames,StyleBits);
        }

        if ( StyleConsts.GetCount() )
        {
            wxPGId ID = Grid->AppendIn(Parent,wxFlagsProperty(GetPGName(),wxPG_LABEL,StyleConsts,STYLEBITS));
            Grid->SetPropertyAttribute(ID,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
            PGRegister(Object,Grid,ID);
        }
    }
}

bool wxsStyleProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    STYLEBITS = Grid->GetPropertyValue(Id).GetLong();
    return true;
}

bool wxsStyleProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    Grid->SetPropertyValue(Id,STYLEBITS);
    return true;
}

bool wxsStyleProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        STYLEBITS = Default;
        return false;
    }

    TiXmlText* Text = Element->FirstChild()->ToText();
    wxString Str;
    if ( Text )
    {
        Str = cbC2U(Text->Value());
    }
    if ( Str.empty() )
    {
        STYLEBITS = Default;
        return false;
    }
    STYLEBITS = ParseStringToBits(Str);
    return true;
}

bool wxsStyleProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( STYLEBITS != Default )
    {
        Element->InsertEndChild(TiXmlText(cbU2C(BitsToString(STYLEBITS))));
        return true;
    }
    return false;
}

bool wxsStyleProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->GetLong(GetDataName(),STYLEBITS,Default);
}

bool wxsStyleProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),STYLEBITS,Default);
}

long wxsStyleProperty::ParseStringToBits(const wxString& String)
{
    long Bits = 0;
    wxStringTokenizer Tkn(String, wxT("| \t\n"), wxTOKEN_STRTOK);
    while ( Tkn.HasMoreTokens() )
    {
        int Index = StyleNames.Index(Tkn.GetNextToken());
        if ( Index != wxNOT_FOUND )
        {
            Bits |= StyleBits[Index];
        }
    }
    return Bits;
}

wxString wxsStyleProperty::BitsToString(long Bits)
{
    wxString Result;
    size_t Count = StyleNames.Count();
    for ( size_t i = 0; i<Count; i++ )
    {
        if ( Bits & StyleBits[i] )
        {
            Result.Append(StyleNames[i]);
            Result.Append(_T('|'));
        }
    }

    if ( Result.empty() )
    {
        return _T("0");
    }

    Result.RemoveLast();
    return Result;
}

void wxsStyleProperty::SetStyle(long& StyleBits,long Style,const wxsStyle* S,bool IsExtra)
{
    long Bit = 1L;

    for ( ; !S->Name.empty(); S++ )
    {
        if ( !S->IsCategory() && (S->IsExtra() == IsExtra) )
        {
            if ( S->Value == Style )
            {
                StyleBits |= Bit;
                return;
            }
            Bit <<= 1;
        }
    }
}

void wxsStyleProperty::ResetStyle(long& StyleBits,long Style,const wxsStyle* S,bool IsExtra)
{
    long Bit = ~1L;
    for ( ; !S->Name.empty(); S++ )
    {
        if ( !S->IsCategory() && (S->IsExtra() == IsExtra) )
        {
            if ( S->Value == Style )
            {
                StyleBits &= Bit;
                return;
            }
            Bit <<= 1;
        }
    }
}

wxString wxsStyleProperty::GetString(long StyleBits,const wxsStyle* S,bool IsExtra,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString Result;
            long Bit = 1L;
            if ( S )
            {
                for ( ; !S->Name.empty(); S++ )
                {
                    if ( !S->IsCategory() && (S->IsExtra() == IsExtra) )
                    {
                        if ( StyleBits & Bit )
                        {
                            Result.Append(S->Name);
                            Result.Append(_T('|'));
                        }
                        Bit <<= 1;
                    }
                }
            }

            if ( Result.empty() )
            {
                return _T("0");
            }

            Result.RemoveLast();
            return Result;
        }
    }

    wxsLANGMSG(wxsStyleProperty::GetString,Language);
    return wxEmptyString;
}

long wxsStyleProperty::GetWxStyle(long StyleBits,const wxsStyle* S,bool IsExtra)
{
    long Bit = 1L;
    long Result = 0L;
    if ( S )
    {
        for ( ; !S->Name.empty(); S++ )
        {
            if ( !S->IsCategory() && (S->IsExtra() == IsExtra) )
            {
                if ( StyleBits & Bit )
                {
                    Result |= S->Value;
                }
                Bit <<= 1;
            }
        }
    }

    return Result;
}

void wxsStyleProperty::SetFromString(long& StyleBits,const wxString& String,const wxsStyle* StyleSet,bool IsExtra)
{
    StyleBits = wxsStyleProperty(_T(""),_T(""),StyleSet,0,String,IsExtra).Default;
}
