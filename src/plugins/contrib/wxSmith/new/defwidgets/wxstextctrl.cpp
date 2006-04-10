#include "wxstextctrl.h"

//#include <wx/textctrl.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsTextCtrlStyles)
    WXS_ST_CATEGORY("wxTextCtrl")
    WXS_ST(wxTE_NO_VSCROLL)
    WXS_ST(wxTE_AUTO_SCROLL)
    WXS_ST(wxTE_PROCESS_ENTER)
    WXS_ST(wxTE_PROCESS_TAB)
    WXS_ST(wxTE_MULTILINE)
    WXS_ST(wxTE_PASSWORD)
    WXS_ST(wxTE_READONLY)
//Help file : No effect under GTK1
    WXS_ST(wxHSCROLL)
    WXS_ST(wxTE_RICH)
    WXS_ST(wxTE_RICH2)
    WXS_ST(wxTE_NOHIDESEL)
    WXS_ST(wxTE_LEFT)
    WXS_ST_MASK(wxTE_AUTO_URL,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CENTRE,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_RIGHT,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CENTRE,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CHARWRAP,wxsSFUNIV|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_WORDWRAP,wxsSFUNIV|wxsSFGTK20,0,true)
    WXS_ST(wxTE_BESTWRAP)

// Help file :On PocketPC and Smartphone, causes the first letter to be capitalized
    WXS_ST_MASK(wxTE_CAPITALIZE,wxsSFWinCE,0,true)
// Help file : same as wxTE_HSCROLL, so ignore
//    WXS_ST(wxTE_DONTWRAP)

// Not in Help file but in XRC file, assume applying to all platform
    WXS_ST(wxTE_LINEWRAP)
WXS_ST_END()



WXS_EV_BEGIN(wxsTextCtrlEvents)
    WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,Text)
    WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EVI(EVT_TEXT_URL,wxEVT_COMMAND_TEXT_URL,wxTextUrlEvent,TextUrl)
    WXS_EVI(EVT_TEXT_MAXLEN,wxEVT_COMMAND_TEXT_MAXLEN,wxCommandEvent,TextMaxLen)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsTextCtrl::Info =
{
    _T("wxTextCtrl"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    75,
    _T("TextCtrl"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsTextCtrl::wxsTextCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsTextCtrlEvents,
        wxsTextCtrlStyles,
        _T("")),
        Text(_("Text")),
        MaxLength(0)
{}



void wxsTextCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxTextCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Text) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( MaxLength > 0 )
            {
                Code << GetVarName() << _T("->SetMaxLength(") << wxString::Format(_T("%d"),MaxLength) << _T(");\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsTextCtrl::BuildCreatingCode,Language);
}


wxObject* wxsTextCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxTextCtrl* Preview = new wxTextCtrl(Parent,GetId(),Text,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


void wxsTextCtrl::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsTextCtrl,Text,0,_("Text"),_T("text"),_T(""),true,false)
    WXS_LONG(wxsTextCtrl,MaxLength,0x8,_("Max Length"),_T("max length"),0)
}

void wxsTextCtrl::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/textctrl.h>")); return;
    }

    wxsLANGMSG(wxsTextCtrl::EnumDeclFiles,Language);
}
