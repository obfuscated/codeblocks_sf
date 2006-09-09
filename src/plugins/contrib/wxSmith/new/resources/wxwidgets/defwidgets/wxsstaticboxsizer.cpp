#include "wxsstaticboxsizer.h"

namespace
{
    static const long OrientValues[] = { wxHORIZONTAL, wxVERTICAL, 0 };
    static const wxChar* OrientNames[] = { _T("wxHORIZONTAL"), _T("wxVERTICAL"), NULL };
    class OrientProp: public wxsEnumProperty
    {
        public:
            OrientProp(int Offset): wxsEnumProperty(_("Orientation"),_T("orient"),Offset,OrientValues,OrientNames,false,wxHORIZONTAL,true) {}
            virtual const wxString GetTypeName() { return _T("Sizer Orientation"); }
    };
}

wxsItemInfo wxsStaticBoxSizer::Info =
{
    _T("wxStaticBoxSizer"),
    wxsTSizer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Layout"),
    60,
    _T("StaticBoxSizer"),
    2, 6,
    NULL,
    NULL,
    0
};

wxSizer* wxsStaticBoxSizer::BuildSizerPreview(wxWindow* Parent)
{
    return new wxStaticBoxSizer(Orient,Parent,Label);
}

void wxsStaticBoxSizer::BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << GetVarName() << _T(" = new wxStaticBoxSizer(");
            if ( Orient != wxHORIZONTAL ) Code << _T("wxVERTICAL"); else Code << _T("wxHORIZONTAL");
            Code << _T(",") << WindowParent << _T(",") << wxsGetWxString(Label) << _T(");\n");
            return;
    }

    wxsLANGMSG(wxsStaticBoxSizer::BuildSizerCreatingCode,Language);
}

void wxsStaticBoxSizer::EnumItemProperties(long Flags)
{
    WXS_STRING(wxsStaticBoxSizer,Label,0,_("Label"),_T("label"),wxEmptyString,false,false);
    static OrientProp Prop(wxsOFFSET(wxsStaticBoxSizer,Orient));
    Property(Prop,0);
}

void wxsStaticBoxSizer::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
    }
    wxsLANGMSG(wxsStaticBoxSizer::EnumDeclFiles,Language);
}
