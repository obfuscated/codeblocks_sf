/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsbitmapiconproperty.h"
#include "wxsbitmapiconeditordlg.h"

#include <messagemanager.h>
#include <wx/artprov.h>
#include <wx/image.h>

wxBitmap wxsBitmapIconData::GetPreview(const wxSize& Size,const wxString& DefaultClient)
{
    if ( Id.empty() )
    {
        if ( FileName.empty() )
        {
            return wxNullBitmap;
        }

        wxImage Img(FileName);
        if ( !Img.Ok() ) return wxNullBitmap;
        if ( Size != wxDefaultSize )
        {
            Img.Rescale(Size.GetWidth(),Size.GetHeight());
        }
        return wxBitmap(Img);
    }

    wxString TempClient = Client.empty() ? DefaultClient : Client;
    return wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(Id),TempClient,Size);
}

wxString wxsBitmapIconData::BuildCode(bool NoResize,const wxString& SizeCode,wxsCodingLang Language,const wxString& DefaultClient)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString Code;
            if ( Id.empty() )
            {
                if ( FileName.empty() ) return wxEmptyString;

                if ( NoResize )
                {
                    Code << _T("wxBitmap(wxImage(") << wxsCodeMarks::WxString(wxsCPP,FileName,false) << _T("))");
                }
                else
                {
                    Code << _T("wxBitmap(wxImage(") << wxsCodeMarks::WxString(wxsCPP,FileName,false) << _T(").Rescale(")
                         << SizeCode << _T(".GetWidth(),") << SizeCode << _T(".GetHeight()))");
                }
            }
            else
            {
                Code << _T("wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(") << wxsCodeMarks::WxString(wxsCPP,Id) << _T("),");
                if ( Client.empty() )
                {
                    Code << wxsCodeMarks::WxString(wxsCPP,DefaultClient,false);
                }
                else
                {
                    Code << wxsCodeMarks::WxString(wxsCPP,Client,false);
                }

                if ( !NoResize )
                {
                    Code << _T(",") << SizeCode;
                }

                Code << _T(")");
            }

            return Code;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsBitmapIconData::BuildCode"),Language);
        }
    }

    return wxEmptyString;
}

bool wxsBitmapIconData::IsEmpty()
{
    return Id.IsEmpty() && FileName.IsEmpty();
}


// Helper macros for fetching variables
#define VALUE   wxsVARIABLE(Object,Offset,wxsBitmapIconData)

wxsBitmapIconProperty::wxsBitmapIconProperty(const wxString& PGName,const wxString& DataName,long _Offset,const wxString& _DefaultClient):
    wxsCustomEditorProperty(PGName,DataName),
    Offset(_Offset),
    DefaultClient(_DefaultClient)
{}

bool wxsBitmapIconProperty::ShowEditor(wxsPropertyContainer* Object)
{
    wxsBitmapIconEditorDlg Dlg(NULL,VALUE,DefaultClient);
    return Dlg.ShowModal() == wxID_OK;
}

bool wxsBitmapIconProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        return false;
    }

    // Trying to read from element's attributes
    VALUE.Id = cbC2U(Element->Attribute("stock_id"));
    VALUE.Client = cbC2U(Element->Attribute("stock_client"));

    // If failed, trying to read from child elements
    // (previous buggy style)
    if ( VALUE.Id.IsEmpty() )
    {
        XmlGetString(Element,VALUE.Id,_T("stock_id"));
    }
    if ( VALUE.Client.IsEmpty() )
    {
        XmlGetString(Element,VALUE.Client,_T("stock_client"));
    }

    if ( VALUE.Id.IsEmpty() )
    {
        // No wxART_PROVIDER Id, do it must be filename
        VALUE.Id.Clear();
        VALUE.Client.Clear();
        return XmlGetString(Element,VALUE.FileName);
    }

    VALUE.FileName.Clear();
    return true;
}

bool wxsBitmapIconProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !VALUE.Id.empty() )
    {
        Element->SetAttribute("stock_id",cbU2C(VALUE.Id));
        if ( !VALUE.Client.empty() )
        {
            Element->SetAttribute("stock_client",cbU2C(VALUE.Client));
        }
        return true;
    }

    if ( !VALUE.FileName.empty() )
    {
        XmlSetString(Element,VALUE.FileName);
        return true;
    }

    return false;
}

bool wxsBitmapIconProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( Stream->GetString(_T("id"),VALUE.Id,wxEmptyString) )
    {
        Stream->GetString(_T("client"),VALUE.Client,wxEmptyString);
    }
    else
    {
        if ( !Stream->GetString(_T("file_name"),VALUE.FileName,wxEmptyString) ) Ret = false;
    }
    Stream->PopCategory();
    return Ret;
}

bool wxsBitmapIconProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    Stream->SubCategory(GetDataName());
    if ( VALUE.Id.empty() )
    {
        if ( !Stream->PutString(_T("file_name"),VALUE.FileName,wxEmptyString) ) Ret = false;
    }
    else
    {

        if ( !Stream->PutString(_T("id"),VALUE.Id,wxEmptyString) ) Ret = false;
        if ( !Stream->PutString(_T("client"),VALUE.Client,wxEmptyString) ) Ret = false;
    }
    Stream->PopCategory();
    return Ret;
}
