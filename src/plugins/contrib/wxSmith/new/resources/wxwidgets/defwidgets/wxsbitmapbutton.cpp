#include "wxsbitmapbutton.h"

#include <wx/bmpbuttn.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsBitmapButtonStyles)
    WXS_ST_CATEGORY("wxBitmapButton")
    WXS_ST_MASK(wxBU_LEFT,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_TOP,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_RIGHT,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_BOTTOM,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_AUTODRAW,wxsSFWin,0,true)

    // cyberkoa: "The help mentions that wxBU_EXACTFIX is not used but the XRC code yes
    //  WXS_ST(wxBU_EXACTFIX)
WXS_ST_END()


WXS_EV_BEGIN(wxsBitmapButtonEvents)
    WXS_EVI(EVT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEvent,Click)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsBitmapButton::Info =
{
    _T("wxBitmapButton"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("BitmapButton"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsBitmapButton::wxsBitmapButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsBitmapButtonEvents,
        wxsBitmapButtonStyles,
        _T(""))
{}



void wxsBitmapButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString SizeName = GetVarName() + _T("_Size");
            Code << _T("wxSize ") << SizeName << _T(" = ") << _T("wxSize(")
                 << wxString::Format(_T("%d"),(BaseProps.Size.X > 2)? (BaseProps.Size.X - 2):BaseProps.Size.X) << _T(",")
                 << wxString::Format(_T("%d"),(BaseProps.Size.Y > 2)? (BaseProps.Size.Y - 2):BaseProps.Size.Y) << _T(");\n");

            wxString BmpCode = BitmapLabel.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_BUTTON);
            Code<< GetVarName() << _T(" = new wxBitmapButton(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << (BmpCode.empty() ? _T("wxNullBitmap") : BmpCode) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << wxsGetWxString(_T("wxDefaultSize")) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( !(BitmapDisabled.Id.empty()||BitmapDisabled.Client.empty())|| !(BitmapDisabled.FileName.empty()) )
            {
                BmpCode = BitmapDisabled.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
                Code << GetVarName() << _T("->SetBitmapDisabled(") << BmpCode << _T(");\n");
            }
            if ( !(BitmapSelected.Id.empty()||BitmapSelected.Client.empty())|| !(BitmapSelected.FileName.empty()) )
            {
                BmpCode = BitmapSelected.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
                Code << GetVarName() << _T("->SetBitmapSelected(") << BmpCode << _T(");\n");
            }
            if ( !(BitmapFocus.Id.empty()||BitmapFocus.Client.empty())|| !(BitmapFocus.FileName.empty()) )
            {
                BmpCode = BitmapFocus.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
                Code << GetVarName() << _T("->SetBitmapFocus(") << BmpCode << _T(");\n");
            }

            if ( IsDefault ) Code << GetVarName() << _T("->SetDefault();\n");
            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating bitmap button (id: %d)"),Language);
}


wxObject* wxsBitmapButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{

    // Reduce the size of the bitmap by 2 pixel for width and height of the bitmap button
    wxSize BitmapSize = wxSize((BaseProps.Size.X > 2)? (BaseProps.Size.X - 2):BaseProps.Size.X,(BaseProps.Size.Y > 2)? (BaseProps.Size.Y - 2):BaseProps.Size.Y);

    wxBitmapButton* Preview = new wxBitmapButton(Parent,GetId(),BitmapLabel.GetPreview(BitmapSize),Pos(Parent),BitmapSize,Style());

    if ( !(BitmapDisabled.Id.empty()||BitmapDisabled.Client.empty())|| !(BitmapDisabled.FileName.empty()) )
        Preview->SetBitmapDisabled(BitmapDisabled.GetPreview(BitmapSize));

    if ( !(BitmapSelected.Id.empty()||BitmapSelected.Client.empty())|| !(BitmapSelected.FileName.empty()) )
        Preview->SetBitmapSelected(BitmapSelected.GetPreview(BitmapSize));

    if ( !(BitmapFocus.Id.empty()||BitmapFocus.Client.empty())|| !(BitmapFocus.FileName.empty()) )
        Preview->SetBitmapFocus(BitmapFocus.GetPreview(BitmapSize));

    if ( IsDefault ) Preview->SetDefault();
    return SetupWindow(Preview,Exact);
}


void wxsBitmapButton::EnumWidgetProperties(long Flags)
{
    WXS_BITMAP(wxsBitmapButton,BitmapLabel,0,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
    WXS_BITMAP(wxsBitmapButton,BitmapDisabled,0,_("Bitmap"),_T("disabled"),_T("wxART_OTHER"))
    WXS_BITMAP(wxsBitmapButton,BitmapSelected,0,_("Bitmap"),_T("selected"),_T("wxART_OTHER"))
    WXS_BITMAP(wxsBitmapButton,BitmapFocus,0,_("Bitmap"),_T("focus"),_T("wxART_OTHER"))
    WXS_BOOL  (wxsBitmapButton,IsDefault,0,_("Is default"),_("default"),false)
}

void wxsBitmapButton::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Decl.Add(_T("<wx/bitmap.h>"));
            Decl.Add(_T("<wx/wximage.h>"));
            Decl.Add(_T("<wx/bmpbuttn.h>"));
            Def.Add(_T("<wx/artprov.h>"));
            return;
        }
    }

    wxsLANGMSG(wxsBitmapButtonn::EnumDeclFiles,Language);
}
