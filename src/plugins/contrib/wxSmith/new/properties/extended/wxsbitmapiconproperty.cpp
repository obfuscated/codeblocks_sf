#include "wxsbitmapiconproperty.h"
#include "wxsbitmapiconeditordlg.h"

#include "../../wxsglobals.h"
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
                    Code << _T("wxBitmap(") << wxsGetWxString(FileName) << _T(");\n");
                }
                else
                {
                    Code << _T("wxBitmap(wxImage(") << wxsGetWxString(FileName) << _T(").Rescale(")
                         << SizeCode << _T(".GetWidth(),") << SizeCode << _T(".GetHeight()));\n");
                }
            }
            else
            {
                Code << _T("wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(") << wxsGetWxString(Id) << _T("),");
                if ( Client.empty() )
                {
                    Code << wxsGetWxString(DefaultClient);
                }
                else
                {
                    Code << wxsGetWxString(Client);
                }

                if ( !NoResize )
                {
                    Code << _T(",") << SizeCode;
                }

                Code << _T(")");
            }

            return Code;
        }
    }

    wxsLANGMSG(wxsBitmapIconData::BuildCode,Language);
    return wxEmptyString;
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

    if ( !XmlGetString(Element,VALUE.Id,_T("stock_id")) || VALUE.Id.empty() )
    {
        VALUE.Id.Clear();
        VALUE.Client.Clear();
        return XmlGetString(Element,VALUE.FileName);
    }
    XmlGetString(Element,VALUE.Client,_T("stock_client"));
    VALUE.FileName.Clear();
    return true;
}

bool wxsBitmapIconProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !VALUE.Id.empty() )
    {
        XmlSetString(Element,VALUE.Id,_T("stock_id"));
        if ( !VALUE.Client.empty() )
        {
            XmlSetString(Element,VALUE.Client,_T("stock_client"));
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
