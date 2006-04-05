#include "wxsboxsizer.h"

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

wxsItemInfo wxsBoxSizer::Info =
{
    _T("wxBoxSizer"),
    wxsTSizer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Layout"),
    50,
    _T("BoxSizer"),
    2, 6,
    NULL,
    NULL,
    0
};

wxSizer* wxsBoxSizer::BuildSizerPreview(wxWindow* Parent)
{
    return new wxBoxSizer(Orient);
}

void wxsBoxSizer::BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << GetVarName() << _T(" = new wxBoxSizer(");
            if ( Orient != wxHORIZONTAL ) Code << _T("wxVERTICAL"); else Code << _T("wxHORIZONTAL");
            Code << _T(");\n");
            return;
    }
    wxsLANGMSG(wxsBoxSizer::BuildSizerCreatingCode,Language);
}

void wxsBoxSizer::EnumItemProperties(long Flags)
{
    static OrientProp Prop(wxsOFFSET(wxsBoxSizer,Orient));
    Property(Prop,0);
}

void wxsBoxSizer::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
    }
    wxsLANGMSG(wxsBoxSizer::EnumDeclFiles,Language);
}
