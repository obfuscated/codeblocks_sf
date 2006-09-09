#include "wxspanelr.h"
#include <messagemanager.h>
#include <wx/app.h>

WXS_ST_BEGIN(wxsPanelrStyles)
    WXS_ST_CATEGORY("wxPanel")
    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
WXS_ST_END()

WXS_EV_BEGIN(wxsPanelrEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsPanelr::Info =
{
    _T("wxPanel resource"),
    wxsTContainer,
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T(""),                     // Empty group - won't add into palette
    0,                          // Not important
    _T("Dialog"),               // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};

wxsPanelr::wxsPanelr(wxsWindowRes* Resource):
    wxsContainer(Resource,
        wxsBaseProperties::flContainer,
        &Info,
        wxsPanelrEvents,
        wxsPanelrStyles,
        _T("wxTAB_TRAVERSAL"))
{}

void wxsPanelr::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< _T("Create(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");
            SetupWindowCode(Code,Language);
            AddChildrenCode(Code,wxsCPP);
            return;
        }
    }

    wxsLANGMSG(wxsPanelr::BuildCreatingCode,Language);
}

wxObject* wxsPanelr::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxWindow* NewItem = new wxPanel(
        Parent,GetId(),wxDefaultPosition,wxDefaultSize,
        Exact ? Style() : wxRAISED_BORDER);
    SetupWindow(NewItem,Exact);
    AddChildrenPreview(NewItem,Exact);
    return NewItem;
}

void wxsPanelr::EnumContainerProperties(long Flags)
{
}

long wxsPanelr::GetPropertiesFlags()
{
    // No identifier nor variable for root items
    return wxsItem::GetPropertiesFlags() & ~(wxsFLVariable|wxsFLId);
}
