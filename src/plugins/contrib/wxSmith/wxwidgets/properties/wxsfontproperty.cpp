/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsfontproperty.h"
#include "wxssimplefonteditordlg.h"

#include <wx/fontenum.h>
#include <wx/tokenzr.h>
#include <wx/fontmap.h>
#include <wx/settings.h>

#include "../wxsflags.h"

using namespace wxsFlags;

namespace
{
    wxString wxsDoubleToString( double value )
    {
        // Because GCC uses locale settings to determine
        // whether to use dot or comma as floating point separator
        // we can not use standard printf-like methods because
        // the code becomes non cross-platform

        wxString res;
        res.Printf( _T("%f"), value );
        res.Replace( _T(","), _T(".") );
        return res;
    }
}

wxFont wxsFontData::BuildFont()
{
    if ( IsDefault )
    {
        return wxFont();
    }

    wxString Face;
    wxFontEnumerator Enumer;
    Enumer.EnumerateFacenames();
    #if wxCHECK_VERSION(2, 8, 0)
        wxArrayString faceNames = Enumer.GetFacenames();
    #else
        wxArrayString& faceNames = *Enumer.GetFacenames();
    #endif
    size_t Count = Faces.Count();
    for ( size_t i = 0; i<Count; i++ )
    {
        if ( faceNames.Index(Faces[i]) != wxNOT_FOUND )
        {
            Face = Faces[i];
            break;
        }
    }

    wxFontEncoding Enc = wxFontMapper::Get()->CharsetToEncoding(Encoding,false);
    if ( Enc == wxFONTENCODING_SYSTEM )
        Enc = wxFONTENCODING_DEFAULT;

    if ( !SysFont.empty() && HasSysFont )
    {
        wxFont Base;
        if      ( SysFont == _T("wxSYS_OEM_FIXED_FONT") )      Base = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);
        else if ( SysFont == _T("wxSYS_ANSI_FIXED_FONT") )     Base = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
        else if ( SysFont == _T("wxSYS_ANSI_VAR_FONT") )       Base = wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT);
        else if ( SysFont == _T("wxSYS_SYSTEM_FONT") )         Base = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
        else if ( SysFont == _T("wxSYS_DEVICE_DEFAULT_FONT") ) Base = wxSystemSettings::GetFont(wxSYS_DEVICE_DEFAULT_FONT);
        else if ( SysFont == _T("wxSYS_DEFAULT_GUI_FONT") )    Base = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

        if ( !Base.Ok() )                                      Base = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

        if ( HasSize ) Base.SetPointSize(Size);
        else if ( HasRelativeSize ) Base.SetPointSize((int)(Base.GetPointSize() * RelativeSize));
        if ( HasStyle ) Base.SetStyle(Style);
        if ( HasWeight ) Base.SetWeight(Weight);
        if ( HasUnderlined ) Base.SetUnderlined(Underlined);
        if ( HasFamily ) Base.SetFamily(Family);
        if ( !Faces.empty() ) Base.SetFaceName(Face);
        if ( HasEncoding ) Base.SetEncoding(Enc);
        return Base;
    }

    return wxFont(
        HasSize ? Size : wxDEFAULT,
        HasFamily ? Family : wxFONTFAMILY_DEFAULT,
        HasStyle ? Style : wxFONTSTYLE_NORMAL,
        HasWeight ? Weight : wxFONTWEIGHT_NORMAL,
        HasUnderlined ? Underlined : false,
        Face,
        HasEncoding ? Enc : wxFONTENCODING_DEFAULT);
}

wxString wxsFontData::BuildFontCode(const wxString& FontName,wxsCoderContext* Context)
{
    if ( IsDefault )
    {
        return wxEmptyString;
    }

    switch ( Context->m_Language )
    {
        case wxsCPP:
        {
            Context->AddHeader(_T("<wx/font.h>"),_T(""),hfLocal);
            wxString Code;

            // Fetching Face name string
            wxString FaceStr = _T("wxEmptyString");

            if ( Faces.Count() == 1 )
            {
                FaceStr = wxsCodeMarks::WxString(wxsCPP,Faces[0],false);
            }
            else if ( Faces.Count() > 1 )
            {
                Context->AddHeader(_T("<wx/fontenum.h>"),_T(""),hfLocal);

                wxString& Enumerator    = Context->m_Extra[_T("FontEnumerator")];
                wxString& FacesArrayStr = Context->m_Extra[_T("FontFaces")];
                wxString  FaceName      = _T("__") + FontName + _T("Face");

                if ( Enumerator.IsEmpty() )
                {
                    // Creating font enumerator if there's none present
                    Enumerator = _T("__FontEnumerator");
                    FacesArrayStr = _T("__FontFaces");
                    Code << _T("wxFontEnumerator ") << Enumerator << _T(";\n");
                    Code << Enumerator << _T(".EnumerateFacenames();\n");

                    // TODO: Add flag to avoid the wxCHECK_VERSION macro
                    // Fetching array of face names
                    Code << _T("#if wxCHECK_VERSION(2, 8, 0)\n");
                    Code << _T("\tconst wxArrayString& ") << FacesArrayStr << _T(" = ") << Enumerator << _T(".GetFacenames();\n");
                    Code << _T("#else\n");
                    Code << _T("\tconst wxArrayString& ") << FacesArrayStr << _T(" = *") << Enumerator << _T(".GetFacenames();\n");
                    Code << _T("#endif\n");
                }

                // Generating local variable which will hold the name of face
                Code << _T("wxString ") << FaceName << _T(";\n");
                FaceStr = FaceName;

                for ( size_t i = 0; i<Faces.Count(); i++ )
                {
                    wxString ThisFace = wxsCodeMarks::WxString(wxsCPP,Faces[i],false);
                    Code << _T("if ( ") << FacesArrayStr << _T(".Index(") << ThisFace << _T(") != wxNOT_FOUND )\n");
                    Code << _T("\t") << FaceName << _T(" = ") << ThisFace << _T(";\n");
                    if ( i != Faces.Count() -1 )
                    {
                        Code << _T("else ");
                    }
                }
            }

            // Fetching encoding

            wxString EncodingStr = _T("wxFONTENCODING_DEFAULT");
            if ( HasEncoding )
            {
                Context->AddHeader(_T("<wx/fontmap.h>"),_T(""),hfLocal);

                wxString EncodingVar = FontName + _T("Encoding");
                Code << _T("wxFontEncoding ") << EncodingVar
                     << _T(" = wxFontMapper::Get()->CharsetToEncoding(")
                     << wxsCodeMarks::WxString(wxsCPP,Encoding) << _T(",false);\n");
                Code << _T("if ( ") << EncodingVar << _T(" == wxFONTENCODING_SYSTEM ) ");
                Code << EncodingVar << _T(" = wxFONTENCODING_DEFAULT;\n");
                EncodingStr = EncodingVar;
            }

            wxString FamilyStr = _T("wxFONTFAMILY_DEFAULT");
            if ( HasFamily )
            {
                if ( Family == wxFONTFAMILY_DECORATIVE ) FamilyStr = _T("wxFONTFAMILY_DECORATIVE"); else
                if ( Family == wxFONTFAMILY_ROMAN      ) FamilyStr = _T("wxFONTFAMILY_ROMAN");      else
                if ( Family == wxFONTFAMILY_SCRIPT     ) FamilyStr = _T("wxFONTFAMILY_SCRIPT");     else
                if ( Family == wxFONTFAMILY_SWISS      ) FamilyStr = _T("wxFONTFAMILY_SWISS");      else
                if ( Family == wxFONTFAMILY_MODERN     ) FamilyStr = _T("wxFONTFAMILY_MODERN");     else
                if ( Family == wxFONTFAMILY_TELETYPE   ) FamilyStr = _T("wxFONTFAMILY_TELETYPE");
            }

            wxString StyleStr = _T("wxFONTSTYLE_NORMAL");
            if ( HasStyle )
            {
                if ( Style == wxFONTSTYLE_ITALIC ) StyleStr = _T("wxFONTSTYLE_ITALIC"); else
                if ( Style == wxFONTSTYLE_SLANT  ) StyleStr = _T("wxFONTSTYLE_SLANT" );
            }

            wxString WeightStr = _T("wxFONTWEIGHT_NORMAL");
            if ( HasWeight )
            {
                if ( Weight == wxFONTWEIGHT_BOLD  ) WeightStr = _T("wxFONTWEIGHT_BOLD");  else
                if ( Weight == wxFONTWEIGHT_LIGHT ) WeightStr = _T("wxFONTWEIGHT_LIGHT");
            }

            if ( !SysFont.empty() && HasSysFont )
            {
                Context->AddHeader(_T("<wx/settings.h>"),_T(""),hfLocal);

                Code << _T("wxFont ") << FontName << _T(" = wxSystemSettings::GetFont(") << SysFont << _T(");\n");
                Code << _T("if ( !") << FontName << _T(".Ok() ) ") << FontName << _T(" = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);\n");

                if ( HasSize )
                {
                    Code << FontName
                         << _T(".SetPointSize(")
                         << wxString::Format(_T("%ld"),Size)
                         << _T(");\n");
                }
                else if ( HasRelativeSize )
                {
                    Code << FontName
                         << _T(".SetPointSize((int)(")
                         << FontName
                         << _T(".GetPointSize() * ")
                         << wxsDoubleToString(RelativeSize)
                         << _T("));\n");
                }
                if ( HasStyle )
                {
                    Code << FontName << _T(".SetStyle(") << StyleStr << _T(");\n");
                }
                if ( HasWeight )
                {
                    Code << FontName << _T(".SetWeight(") << WeightStr << _T(");\n");
                }
                if ( HasUnderlined )
                {
                    Code << FontName << _T(".SetUnderlined(")
                         << (Underlined?_T("true"):_T("false")) << _T(");\n");
                }
                if ( HasFamily )
                {
                    Code << FontName << _T(".SetFamily(") << FamilyStr << _T(");\n");
                }
                if ( !Faces.empty() )
                {
                    Code << FontName << _T(".SetFaceName(") << FaceStr << _T(");\n");
                }
                if ( HasEncoding )
                {
                    Code << FontName << _T(".SetEncoding(") << EncodingStr << _T(");\n");
                }
                return Code;
            }

            Code << _T("wxFont ") << FontName << _T("(")
                 << (HasSize ? wxString::Format(_T("%ld,"),Size) : _T("wxDEFAULT,"))
                 << FamilyStr << _T(",")
                 << StyleStr << _T(",")
                 << WeightStr << _T(",")
                 << (HasUnderlined && Underlined ? _T("true,") : _T("false,"))
                 << FaceStr << _T(",")
                 << EncodingStr << _T(");\n");

            return Code;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFontData::BuildFontCode"),Context->m_Language);
        }
    }
    return wxEmptyString;
}

// Helper macros for fetching variables
#define VALUE   wxsVARIABLE(Object,Offset,wxsFontData)

wxsFontProperty::wxsFontProperty(const wxString& PGName,const wxString& DataName,long _Offset,int Priority):
    wxsCustomEditorProperty(PGName,DataName,Priority),
    Offset(_Offset)
{}

bool wxsFontProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsSimpleFontEditorDlg Dlg(0,VALUE);
    return Dlg.ShowModal() == wxID_OK;
}

wxString wxsFontProperty::GetStr(wxsPropertyContainer* Object)
{
    wxFont fnt = VALUE.BuildFont();
    wxString res = wxEmptyString;
    if (fnt.IsOk())
        res = fnt.GetNativeFontInfoUserDesc();
    return res.IsEmpty()?wxsCustomEditorProperty::GetStr(Object):res;
}


bool wxsFontProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE.IsDefault = true;
        VALUE.HasSize = false;
        VALUE.HasStyle = false;
        VALUE.HasWeight = false;
        VALUE.HasUnderlined = false;
        VALUE.HasFamily = false;
        VALUE.HasEncoding = false;
        VALUE.HasSysFont = false;
        VALUE.HasRelativeSize = false;
        VALUE.Faces.Clear();
        return false;
    }

    wxString Val;

    // Fetching size
    if ( (VALUE.HasSize = XmlGetString(Element,Val,_T("size")) ))
    {
        Val.ToLong(&VALUE.Size);
    }

    if ( (VALUE.HasStyle = XmlGetString(Element,Val,_T("style")) ))
    {
        if ( Val == _T("italic") ) VALUE.Style = wxFONTSTYLE_ITALIC; else
        if ( Val == _T("slant") )  VALUE.Style = wxFONTSTYLE_SLANT;  else
                                   VALUE.Style = wxFONTSTYLE_NORMAL;
    }

    if ( (VALUE.HasWeight = XmlGetString(Element,Val,_T("weight")) ))
    {
        if ( Val == _T("bold") )  VALUE.Weight = wxFONTWEIGHT_BOLD;   else
        if ( Val == _T("light") ) VALUE.Weight = wxFONTWEIGHT_LIGHT;  else
                                  VALUE.Weight = wxFONTWEIGHT_NORMAL;
    }

    if ( (VALUE.HasUnderlined = XmlGetString(Element,Val,_T("underlined")) ))
    {
        if ( Val == _T("1" ) ) VALUE.Underlined = true;
        else                   VALUE.Underlined = false;
    }

    if ( (VALUE.HasFamily = XmlGetString(Element,Val,_T("family")) ))
    {
        if ( Val == _T("decorative") ) VALUE.Family = wxFONTFAMILY_DECORATIVE; else
        if ( Val == _T("roman") )      VALUE.Family = wxFONTFAMILY_ROMAN;      else
        if ( Val == _T("script") )     VALUE.Family = wxFONTFAMILY_SCRIPT;     else
        if ( Val == _T("swiss") )      VALUE.Family = wxFONTFAMILY_SWISS;      else
        if ( Val == _T("modern") )     VALUE.Family = wxFONTFAMILY_MODERN;     else
        if ( Val == _T("teletype") )   VALUE.Family = wxFONTFAMILY_TELETYPE;   else
                                       VALUE.Family = wxFONTFAMILY_DEFAULT;
    }

    VALUE.Faces.Clear();
    if ( XmlGetString(Element,Val,_T("face")) )
    {
        wxStringTokenizer tk(Val, wxT(","));

        while ( tk.HasMoreTokens() )
        {
            VALUE.Faces.Add(tk.GetNextToken());
        }
    }

    VALUE.HasEncoding = XmlGetString(Element,VALUE.Encoding,_T("encoding"));
    VALUE.HasSysFont = XmlGetString(Element,VALUE.SysFont,_T("sysfont"));

    if ( (VALUE.HasRelativeSize = XmlGetString(Element,Val,_T("relativesize")) ))
    {
        Val.ToDouble(&VALUE.RelativeSize);
    }

    VALUE.IsDefault = false;

    return true;
}

bool wxsFontProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE.IsDefault )
    {
        return false;
    }

    if ( VALUE.HasSize )
    {
        XmlSetString(Element,wxString::Format(_T("%ld"),VALUE.Size),_T("size"));
    }

    if ( VALUE.HasStyle )
    {
        if ( VALUE.Style == wxFONTSTYLE_ITALIC ) XmlSetString(Element,_T("italic"),_T("style")); else
        if ( VALUE.Style == wxFONTSTYLE_SLANT  ) XmlSetString(Element,_T("slant"), _T("style")); else
                                                 XmlSetString(Element,_T("normal"),_T("style"));
    }

    if ( VALUE.HasWeight )
    {
        if ( VALUE.Weight == wxFONTWEIGHT_BOLD  ) XmlSetString(Element,_T("bold"),  _T("weight")); else
        if ( VALUE.Weight == wxFONTWEIGHT_LIGHT ) XmlSetString(Element,_T("light"), _T("weight")); else
                                                  XmlSetString(Element,_T("normal"),_T("weight"));
    }

    if ( VALUE.HasUnderlined )
    {
        if ( VALUE.Underlined ) XmlSetString(Element,_T("1"),_T("underlined"));
        else                    XmlSetString(Element,_T("0"),_T("underlined"));

    }

    if ( VALUE.HasFamily )
    {
        if ( VALUE.Family == wxFONTFAMILY_DECORATIVE ) XmlSetString(Element,_T("decorative"),_T("family")); else
        if ( VALUE.Family == wxFONTFAMILY_ROMAN      ) XmlSetString(Element,_T("roman"),     _T("family")); else
        if ( VALUE.Family == wxFONTFAMILY_SCRIPT     ) XmlSetString(Element,_T("script"),    _T("family")); else
        if ( VALUE.Family == wxFONTFAMILY_SWISS      ) XmlSetString(Element,_T("swiss"),     _T("family")); else
        if ( VALUE.Family == wxFONTFAMILY_MODERN     ) XmlSetString(Element,_T("modern"),    _T("family")); else
        if ( VALUE.Family == wxFONTFAMILY_TELETYPE   ) XmlSetString(Element,_T("teletype"),  _T("family")); else
        if ( VALUE.Family == wxFONTFAMILY_DEFAULT    ) XmlSetString(Element,_T("default"),   _T("family"));
    }

    wxString Faces;
    size_t Count = VALUE.Faces.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        Faces.Append(VALUE.Faces[i]);
        Faces.Append(_T(','));
    }

    if ( !Faces.empty() )
    {
        Faces.RemoveLast();
        XmlSetString(Element,Faces,_T("face"));
    }

    if ( VALUE.HasEncoding )
    {
        XmlSetString(Element,VALUE.Encoding,_T("encoding"));
    }

    if ( VALUE.HasSysFont )
    {
        XmlSetString(Element,VALUE.SysFont,_T("sysfont"));
    }

    if ( VALUE.HasRelativeSize )
    {
        XmlSetString(Element, wxsDoubleToString(VALUE.RelativeSize), _T("relativesize"));
    }

    return true;
}

bool wxsFontProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    bool Tmp;
    long TmpL;
    Stream->SubCategory(GetDataName());

    Stream->GetBool(_T("has_size"),Tmp,false); VALUE.HasSize = Tmp;
    Stream->GetLong(_T("size"),VALUE.Size,0);
    Stream->GetBool(_T("has_style"),Tmp,false); VALUE.HasStyle = Tmp;
    TmpL = static_cast<long>(VALUE.Style);
    Stream->GetLong(_T("style"),TmpL,wxFONTSTYLE_NORMAL);
    VALUE.Style = static_cast<wxFontStyle>(TmpL);
    Stream->GetBool(_T("has_weight"),Tmp,false); VALUE.HasWeight = Tmp;
    TmpL = static_cast<long>(VALUE.Weight);
    Stream->GetLong(_T("weight"),TmpL,wxFONTWEIGHT_NORMAL);
    VALUE.Weight = static_cast<wxFontWeight>(TmpL);
    Stream->GetBool(_T("has_underlined"),Tmp,false); VALUE.HasUnderlined = Tmp;
    Stream->GetBool(_T("underlined"),VALUE.Underlined,false);
    Stream->GetBool(_T("has_family"),Tmp,false); VALUE.HasFamily = Tmp;
    TmpL = static_cast<long>(VALUE.Family);
    Stream->GetLong(_T("family"),TmpL,wxFONTFAMILY_DEFAULT);
    VALUE.Family = static_cast<wxFontFamily>(TmpL);
    VALUE.Faces.Clear();
    Stream->SubCategory(_T("faces"));
    wxString Str;
    while ( Stream->GetString(_T("face"),Str,_T("")) )
    {
        VALUE.Faces.Add(Str);
    }
    Stream->PopCategory();
    Stream->GetBool(_T("has_encoding"),Tmp,false); VALUE.HasEncoding = Tmp;
    Stream->GetString(_T("encoding"),VALUE.Encoding,_T(""));
    Stream->GetBool(_T("has_relativesize"),Tmp,false); VALUE.HasRelativeSize = Tmp;
    Stream->GetDouble(_T("relativesize"),VALUE.RelativeSize,0.0);

    Stream->PopCategory();
    return Ret;
}

bool wxsFontProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    bool Tmp;
    long TmpL;
    Stream->SubCategory(GetDataName());

    Tmp = VALUE.HasSize;
    Stream->PutBool(_T("has_size"),Tmp,false); Tmp = VALUE.HasSize;
    Stream->PutLong(_T("size"),VALUE.Size,0);
    Tmp = VALUE.HasStyle;
    Stream->PutBool(_T("has_style"),Tmp,false); VALUE.HasStyle = Tmp;
    TmpL = static_cast<long>(VALUE.Style);
    Stream->PutLong(_T("style"),TmpL,wxFONTSTYLE_NORMAL);
    VALUE.Style = static_cast<wxFontStyle>(TmpL);
    Tmp = VALUE.HasWeight;
    Stream->PutBool(_T("has_weight"),Tmp,false); VALUE.HasWeight = Tmp;
    TmpL = static_cast<long>(VALUE.Weight);
    Stream->PutLong(_T("weight"),TmpL,wxFONTWEIGHT_NORMAL);
    VALUE.Weight = static_cast<wxFontWeight>(TmpL);
    Tmp = VALUE.HasUnderlined;
    Stream->PutBool(_T("has_underlined"),Tmp,false); VALUE.HasUnderlined = Tmp;
    Stream->PutBool(_T("underlined"),VALUE.Underlined,false);
    Tmp = VALUE.HasFamily;
    Stream->PutBool(_T("has_family"),Tmp,false); VALUE.HasFamily = Tmp;
    TmpL = static_cast<long>(VALUE.Family);
    Stream->PutLong(_T("family"),TmpL,wxFONTFAMILY_DEFAULT);
    VALUE.Family = static_cast<wxFontFamily>(TmpL);
    VALUE.Faces.Clear();
    Stream->SubCategory(_T("faces"));
    size_t Count = VALUE.Faces.Count();
    for ( size_t i = 0; i < Count; i++ )
    {
        Stream->PutString(_T("face"),VALUE.Faces[i],_T(""));
    }
    Stream->PopCategory();
    Tmp = VALUE.HasEncoding;
    Stream->PutBool(_T("has_encoding"),Tmp,false); VALUE.HasEncoding = Tmp;
    Stream->PutString(_T("encoding"),VALUE.Encoding,_T(""));
    Tmp = VALUE.HasRelativeSize;
    Stream->PutBool(_T("has_relativesize"),Tmp,false); VALUE.HasRelativeSize = Tmp;
    Stream->PutDouble(_T("relativesize"),VALUE.RelativeSize,0.0);

    Stream->PopCategory();
    return Ret;
}


