#include "wxsstaticbitmap.h"

#include <wx/statbmp.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsStaticBitmapStyles)
    WXS_ST_CATEGORY("wxStaticBitmap")
WXS_ST_END()


WXS_EV_BEGIN(wxsStaticBitmapEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsStaticBitmap::Info =
{
    _T("wxStaticBitmap"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    70,
    _T("StaticBitmap"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsStaticBitmap::wxsStaticBitmap(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsStaticBitmapEvents,
        wxsStaticBitmapStyles,
        _T(""))
{}



void wxsStaticBitmap::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString SizeName = GetVarName() + _T("_Size");
            Code << _T("wxSize ") << SizeName << _T(" = ") << SizeCode(WindowParent,wxsCPP) << _T(";\n");
            wxString BmpCode = Bitmap.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
            Code<< GetVarName() << _T(" = new wxStaticBitmap(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << (BmpCode.empty() ? _T("wxNullBitmap") : BmpCode) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeName << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsStaticBitmap::BuildCreatingCode,Language);
}


wxObject* wxsStaticBitmap::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxStaticBitmap* Preview = new wxStaticBitmap(Parent,GetId(),Bitmap.GetPreview(Size(Parent)),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Exact);
}



void wxsStaticBitmap::EnumWidgetProperties(long Flags)
{
   WXS_BITMAP(wxsStaticBitmap,Bitmap,0,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
}

void wxsStaticBitmap::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Decl.Add(_T("<wx/bitmap.h>"));
            Decl.Add(_T("<wx/image.h>"));
            Def.Add(_T("<wx/artprov.h>"));
            return;
        }
    }

    wxsLANGMSG(wxsStaticBitmap::EnumDeclFiles,Language);
}
