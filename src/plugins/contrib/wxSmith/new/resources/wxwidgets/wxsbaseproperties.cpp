#include "wxsbaseproperties.h"

#include <messagemanager.h>

void wxsBaseProperties::EnumProperties(long Flags)
{
    WXS_POSITION(wxsBaseProperties,Position,flPosition,_("Default pos"),_("X"),_("Y"),_("Pos in dialog units"),_T("pos"));
    WXS_SIZE    (wxsBaseProperties,Size,flSize,_("Default size"),_("Width"),_("Height"),_("Size in dialog units"),_T("size"));
    WXS_BOOL    (wxsBaseProperties,Enabled,flEnabled,_("Enabled"),_T("enabled"),true);
    WXS_BOOL    (wxsBaseProperties,Focused,flFocused,_("Focused"),_T("focused"),false);
    WXS_BOOL    (wxsBaseProperties,Hidden,flHidden,_("Hidden"),_T("hidden"),false);
    WXS_COLOUR  (wxsBaseProperties,Fg,flColours,_("Foreground"),_T("fg"));
    WXS_COLOUR  (wxsBaseProperties,Bg,flColours,_("Background"),_T("bg"));
    WXS_FONT    (wxsBaseProperties,Font,flFont,_("Font"),_("font"));
    WXS_STRING  (wxsBaseProperties,ToolTip,flToolTip,_("Tooltip"),_T("tooltip"),wxEmptyString,true,false);
    WXS_STRING  (wxsBaseProperties,HelpText,flHelpText,_("Help text"),_T("helptext"),wxEmptyString,true,false);
}

void wxsBaseProperties::SetupWindow(wxWindow* Window,bool IsExact)
{
    if ( !Enabled ) Window->Disable();
    if ( Focused  ) Window->SetFocus();
    if ( Hidden && IsExact ) Window->Hide();
    wxColour FGCol = wxsColourProperty::GetColour(Fg);
    if ( FGCol.Ok() ) Window->SetForegroundColour(FGCol);
    wxColour BGCol = wxsColourProperty::GetColour(Bg);
    if ( BGCol.Ok() ) Window->SetBackgroundColour(BGCol);
    wxFont FontVal = Font.BuildFont();
    if ( FontVal.Ok() ) Window->SetFont(FontVal);
    if ( !ToolTip.empty() ) Window->SetToolTip(ToolTip);
    if ( !HelpText.empty() ) Window->SetHelpText(HelpText);
}

void wxsBaseProperties::BuildSetupWindowCode(wxString& Code,const wxString& WindowName,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString VarAccess = WindowName.empty() ? _T("") : WindowName + _T("->");
            if ( !Enabled ) Code << VarAccess << _T("Disable();\n");
            if ( Focused  ) Code << VarAccess << _T("SetFocus();\n");
            if ( Hidden )   Code << VarAccess << _T("Hide();\n");

            wxString FGCol = wxsColourProperty::GetColourCode(Fg,wxsCPP);
            if ( !FGCol.empty() ) Code << VarAccess << _T("SetForegroundColour(") << FGCol << _T(");\n");

            wxString BGCol = wxsColourProperty::GetColourCode(Bg,wxsCPP);
            if ( !BGCol.empty() ) Code << VarAccess << _T("SetBackgroundColour(") << BGCol << _T(");\n");

            wxString FontVal = Font.BuildFontCode(WindowName + _T("Font"), wxsCPP);
            if ( !FontVal.empty() )
            {
                Code << FontVal;
                Code << VarAccess << _T("SetFont(") << WindowName << _T("Font);\n");
            }
            if ( !ToolTip.empty()  ) Code << VarAccess << _T("SetToolTip(") << wxsGetWxString(ToolTip) << _T(");\n");
            if ( !HelpText.empty() ) Code << VarAccess << _T("SetHelpText(") << wxsGetWxString(HelpText) << _T(");\n");
            return;
        }
    }

    wxsLANGMSG(wxsBaseProperties::BuildSetupWindowCode,Language);
}

void wxsBaseProperties::AddQPPChild(wxsAdvQPP* QPP,long Flags)
{
    // TODO: Add it
}
