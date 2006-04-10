#include "wxsdialog.h"
#include <messagemanager.h>
#include <wx/app.h>

WXS_ST_BEGIN(wxsDialogStyles)
    WXS_ST_CATEGORY("wxDialog")
    WXS_ST(wxSTAY_ON_TOP)
    WXS_ST(wxCAPTION)
    WXS_ST(wxDEFAULT_DIALOG_STYLE)
    WXS_ST(wxTHICK_FRAME)
    WXS_ST(wxSYSTEM_MENU)
    WXS_ST(wxRESIZE_BORDER)
    WXS_ST(wxRESIZE_BOX)
    WXS_ST(wxCLOSE_BOX)
    WXS_ST(wxDIALOG_MODAL)
    WXS_ST(wxDIALOG_MODELESS)
    WXS_ST(wxDIALOG_NO_PARENT)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_ST(wxDIALOG_EX_METAL)
    WXS_ST(wxMAXIMIZE_BOX)
    WXS_ST(wxMINIMIZE_BOX)
    WXS_ST(wxFRAME_SHAPED)
// NOTE (cyberkoa#1#): wxNO_3D & wxDIALOG_EX_CONTEXTHELP is only available
// on WXMSW wxDIALOG_EX_METAL only for WXMAC
// NOTE (cyberkoa#1#):  wxDIALOG_EX_CONTEXTHELP & wxDIALOG_EX_METAL are
// extended style which need 2 steps construction
    WXS_ST_MASK(wxNO_3D,wxsSFWin,0,true)
    WXS_EXST_MASK(wxDIALOG_EX_CONTEXTHELP,wxsSFWin,0,true)
    WXS_EXST_MASK(wxDIALOG_EX_METAL,wxsSFOSX,0,true)
WXS_ST_END()

WXS_EV_BEGIN(wxsDialogEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsDialog::Info =
{
    _T("wxDialog"),
    wxsTContainer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T(""),
    0,
    _T("Dialog"),
    2, 6,
    NULL,
    NULL,
    0
};

wxsDialog::wxsDialog(wxsWindowRes* Resource):
    wxsContainer(Resource,
        wxsBaseProperties::flContainer,
        &Info,
        wxsDialogEvents,
        wxsDialogStyles,
        _T("wxDEFAULT_DIALOG_STYLE")),
    Title(_("Dialog")),
    Centered(false)
{}

void wxsDialog::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< _T("Create(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Title) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            AddChildrenCode(Code,wxsCPP);
            if ( Centered )
            {
                Code<<_T("Centre();\n");
            }

            return;
        }
    }

    wxsLANGMSG(wxsDialog::BuildCreatingCode,Language);
}

wxObject* wxsDialog::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxWindow* NewItem = NULL;
    wxDialog* Dlg = NULL;

    // In case of frame and dialog when in "Exact" mode, we do not create
    // new object, but use Parent and call Create for it.
    if ( Exact )
    {
        Dlg = wxDynamicCast(Parent,wxDialog);
        if ( Dlg )
        {
            Dlg->Create(NULL,GetId(),Title,Pos(wxTheApp->GetTopWindow()),Size(wxTheApp->GetTopWindow()),Style());
        }
        NewItem = Dlg;
    }
    else
    {
        // In preview we simulate dialog using panel
        NewItem = new wxPanel(Parent,GetId(),wxDefaultPosition,wxDefaultSize,wxRAISED_BORDER);
    }

    SetupWindow(NewItem,Exact);
    AddChildrenPreview(NewItem,Exact);

    if ( Dlg && Centered )
    {
        Dlg->Centre();
    }

    return NewItem;
}

void wxsDialog::EnumContainerProperties(long Flags)
{
    WXS_STRING(wxsDialog,Title,0,_("Title"),_T("title"),_T(""),false,false)
    WXS_BOOL  (wxsDialog,Centered,0,_("Centered"),_T("centered"),false);
}

long wxsDialog::GetPropertiesFlags()
{
    // No identifier nor variable for root items
    return wxsItem::GetPropertiesFlags() & ~(wxsFLVariable|wxsFLId);
}
