#include "wxsgridsizer.h"

wxsItemInfo wxsGridSizer::Info =
{
    _T("wxGridSizer"),
    wxsTSizer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Layout"),
    70,
    _T("GridSizer"),
    2, 6,
    NULL,
    NULL,
    0
};

wxSizer* wxsGridSizer::BuildSizerPreview(wxWindow* Parent)
{
	return new wxGridSizer(Rows,Cols,
        wxsDimensionProperty::GetPixels(VGap,VGapDU,Parent),
        wxsDimensionProperty::GetPixels(HGap,HGapDU,Parent));
}

void wxsGridSizer::BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << GetVarName() << _T(" = new wxGridSizer(")
                 << wxString::Format(_T("%d,%d"),Rows,Cols) << _T(",")
                 << wxsDimensionProperty::GetPixelsCode(VGap,VGapDU,WindowParent,wxsCPP) << _T(",")
                 << wxsDimensionProperty::GetPixelsCode(HGap,HGapDU,WindowParent,wxsCPP) << _T(");\n");
            return;
    }

    wxsLANGMSG(wxsGridSizer::BuildSizerCreatingCode,Language);
}

void wxsGridSizer::EnumItemProperties(long Flags)
{
    WXS_LONG(wxsGridSizer,Cols,0,_("Cols"),_T("cols"),0);
    WXS_LONG(wxsGridSizer,Rows,0,_("Rows"),_T("rows"),0);
    WXS_DIMENSION(wxsGridSizer,VGap,VGapDU,0,_("V-Gap"),_("V-Gap in dialog units"),_T("vgap"),0,false);
    WXS_DIMENSION(wxsGridSizer,HGap,HGapDU,0,_("H-Gap"),_("H,y-Gap in dialog units"),_T("hgap"),0,false);
}

void wxsGridSizer::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
    }
    wxsLANGMSG(wxsGridSizer::EnumDeclFiles,Language);
}
