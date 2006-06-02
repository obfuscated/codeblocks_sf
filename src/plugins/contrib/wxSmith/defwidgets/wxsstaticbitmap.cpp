#include "../wxsheaders.h"
#include <wx/filename.h>
#include "wxsstaticbitmap.h"

WXS_ST_BEGIN(wxsStaticBitmapStyles)
    //WXS_ST_CATEGORY("wxStaticBitmap")
WXS_ST_END(wxsStaticBitmapStyles)

WXS_EV_BEGIN(wxsStaticBitmapEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsStaticBitmapEvents)


wxsDWDefineBegin(wxsStaticBitmapBase,wxStaticBitmap,
        WXS_THIS = new wxStaticBitmap(WXS_PARENT,WXS_ID,wxBitmap(wxImage(ImageFile, ImageType).Rescale(BaseProperties.SizeX,BaseProperties.SizeY)),WXS_POS,WXS_SIZE,WXS_STYLE);
        )
    wxsDWDefAdvImageX(ImageFile, "bitmap","Image", "");
wxsDWDefineEnd()

wxsStaticBitmap::wxsStaticBitmap(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsStaticBitmapBase(Man,Res)
{
    BaseProperties.DefaultSize = false;
    BaseProperties.SizeX = 32;
    BaseProperties.SizeY = 32;
    ImageType = GetBitmapType();
}

bool wxsStaticBitmap::MyPropertiesUpdated(bool Validate,bool Correct)
{
 // Update new value for ImageType by extracting the extension
  ImageType = GetBitmapType();

  return true;
}

wxString wxsStaticBitmap::GetProducingCode(const wxsCodeParams& Params)
{
    wxString SizeCode;
    if(Params.Size==_T("wxDefaultSize"))
      SizeCode = _T("");
    else
      SizeCode = _T(".Rescale") + Params.Size.Mid(6); // Get the "(x,y) from "wxSize(x,y)"


    wxString Code;
    Code.Printf(_T("%s = new wxStaticBitmap(%s,%s,wxBitmap(wxImage(%s, %s)%s),%s,%s,%s);\n"),
            Params.VarName.c_str(),
            Params.ParentName.c_str(),
            Params.IdName.c_str(),
            wxsGetWxString(ImageFile).c_str(),
            GetBitmapTypeAsString().c_str(),
            SizeCode.c_str(),
            Params.Pos.c_str(),
            Params.Size.c_str(),
            Params.Style.c_str());

    Code << Params.InitCode;

    return Code;
}

wxWindow* wxsStaticBitmap::MyCreatePreview(wxWindow* Parent)
{
    wxStaticBitmap* StaticBitmap;

    if(ImageFile.IsEmpty())
        StaticBitmap = new wxStaticBitmap(Parent,-1,wxBitmap(BaseProperties.SizeX,BaseProperties.SizeY),GetPosition(),GetSize(),GetStyle());
    else
        StaticBitmap = new wxStaticBitmap(Parent,-1,wxBitmap(wxImage(ImageFile, ImageType).Rescale(BaseProperties.SizeX,BaseProperties.SizeY)),GetPosition(),GetSize(),GetStyle());

    PreviewApplyDefaults(StaticBitmap);
    return StaticBitmap;
}

long wxsStaticBitmap::GetBitmapType()
{
  extension = wxFileName(ImageFile).GetExt().Lower();
  if(extension==_T("jpg")||extension==_T("jpeg"))       return wxBITMAP_TYPE_JPEG;
  else if (extension==_T("png"))                        return wxBITMAP_TYPE_PNG;
  else if (extension==_T("bmp"))                        return wxBITMAP_TYPE_BMP;
  else if (extension==_T("gif"))                        return wxBITMAP_TYPE_GIF;
  else if (extension==_T("tif")||extension==_T("tiff")) return wxBITMAP_TYPE_TIF;
  else                                                  return wxBITMAP_TYPE_ANY; // Auto-detect
}

wxString wxsStaticBitmap::GetBitmapTypeAsString()
{
  extension = wxFileName(ImageFile).GetExt().Lower();
  if(extension==_T("jpg")||extension==_T("jpeg"))       return _T("wxBITMAP_TYPE_JPEG");
  else if (extension==_T("png"))                        return _T("wxBITMAP_TYPE_PNG");
  else if (extension==_T("bmp"))                        return _T("wxBITMAP_TYPE_BMP");
  else if (extension==_T("gif"))                        return _T("wxBITMAP_TYPE_GIF");
  else if (extension==_T("tif")||extension==_T("tiff")) return _T("wxBITMAP_TYPE_TIF");
  else                                                  return _T("wxBITMAP_TYPE_ANY"); // Auto-detect
}
