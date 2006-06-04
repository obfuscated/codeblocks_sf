#include "../wxsheaders.h"
#include <wx/filename.h>
#include "wxsstaticbitmap.h"

WXS_ST_BEGIN(wxsStaticBitmapStyles)
    //WXS_ST_CATEGORY("wxStaticBitmap")
WXS_ST_END(wxsStaticBitmapStyles)

WXS_EV_BEGIN(wxsStaticBitmapEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsStaticBitmapEvents)


//   WXS_THIS = new wxStaticBitmap(WXS_PARENT,WXS_ID,wxBitmap(wxImage(ImageFile).Rescale(BaseProperties.SizeX,BaseProperties.SizeY)),WXS_POS,WXS_SIZE,WXS_STYLE);
wxsDWDefineBegin(wxsStaticBitmapBase,wxStaticBitmap,
        if(BaseProperties.SizeX==-1&&BaseProperties.SizeY==-1){
         if(ImageFile.IsEmpty())
           WXS_THIS = new wxStaticBitmap(WXS_PARENT,WXS_ID,wxBitmap(wxNullBitmap),WXS_POS,WXS_SIZE,WXS_STYLE);
         else
           WXS_THIS = new wxStaticBitmap(WXS_PARENT,WXS_ID,wxBitmap(wxImage(ImageFile)),WXS_POS,WXS_SIZE,WXS_STYLE);
        }
        else
           WXS_THIS = new wxStaticBitmap(WXS_PARENT,WXS_ID,wxBitmap(wxImage(ImageFile).Rescale(BaseProperties.SizeX,BaseProperties.SizeY)),WXS_POS,WXS_SIZE,WXS_STYLE);
        )
    wxsDWDefAdvImageX(ImageFile, "bitmap","Image", "");
wxsDWDefineEnd()

wxsStaticBitmap::wxsStaticBitmap(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsStaticBitmapBase(Man,Res)
{
    BaseProperties.DefaultSize = true;
}


wxString wxsStaticBitmap::GetProducingCode(const wxsCodeParams& Params)
{
    wxString SizeCode;

    if(BaseProperties.SizeX==-1 && BaseProperties.SizeY==-1)
      SizeCode = _T("");
    else
      SizeCode = _T(".Rescale") + Params.Size.Mid(6); // Get the "(x,y) from "wxSize(x,y)"

    wxString Code;

    if(ImageFile.IsEmpty())
        Code.Printf(_T("%s = new wxStaticBitmap(%s,%s,wxNullBitmap,%s,%s,%s);\n"),
            Params.VarName.c_str(),
            Params.ParentName.c_str(),
            Params.IdName.c_str(),
            Params.Pos.c_str(),
            Params.Size.c_str(),
            Params.Style.c_str());

    else
    Code.Printf(_T("%s = new wxStaticBitmap(%s,%s,wxBitmap(wxImage(%s)%s),%s,%s,%s);\n"),
            Params.VarName.c_str(),
            Params.ParentName.c_str(),
            Params.IdName.c_str(),
            wxsGetWxString(ImageFile).c_str(),
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
        StaticBitmap = new wxStaticBitmap(Parent,-1,wxBitmap(wxNullBitmap),GetPosition(),GetSize(),GetStyle());
    else if(BaseProperties.SizeX==-1 && BaseProperties.SizeY==-1)
        StaticBitmap = new wxStaticBitmap(Parent,-1,wxBitmap(wxImage(ImageFile)),GetPosition(),GetSize(),GetStyle());
    else
        StaticBitmap = new wxStaticBitmap(Parent,-1,wxBitmap(wxImage(ImageFile).Rescale(BaseProperties.SizeX,BaseProperties.SizeY)),GetPosition(),GetSize(),GetStyle());

    PreviewApplyDefaults(StaticBitmap);
    return StaticBitmap;
}
