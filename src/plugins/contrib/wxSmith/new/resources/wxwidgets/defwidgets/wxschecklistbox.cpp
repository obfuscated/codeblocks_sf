#include "wxschecklistbox.h"

#include <wx/checklst.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsCheckListBoxStyles)
    WXS_ST_CATEGORY("wxCheckListBox")
    WXS_ST_MASK(wxLB_HSCROLL,wxsSFWin,0,true) // Windows ONLY
    WXS_ST(wxLB_SINGLE)
    WXS_ST(wxLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_ALWAYS_SB)
    WXS_ST(wxLB_NEEDED_SB)
    WXS_ST(wxLB_SORT)
WXS_ST_END()


WXS_EV_BEGIN(wxsCheckListBoxEvents)
    WXS_EVI(EVT_CHECKLISTBOX,wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,wxCommandEvent,Toggled)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsCheckListBox::Info =
{
    _T("wxCheckListBox"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("CheckListBox"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsCheckListBox::wxsCheckListBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsCheckListBoxEvents,
        wxsCheckListBoxStyles,
        _T(""))
{}


void wxsCheckListBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxCheckListBox(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( ArrayChecks[i] )
                {
                    Code << GetVarName() << _T("->Check( ");
                }
                Code << GetVarName() << _T("->Append(") << wxsGetWxString(ArrayChoices[i]) << _T(")");
                if ( ArrayChecks[i] )
                {
                    Code << GetVarName() << _T(" )");
                }
                Code << _T(";\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsCheckListBox::BuildCreatingCode,Language);
}


wxObject* wxsCheckListBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxCheckListBox* Preview = new wxCheckListBox(Parent,GetId(),Pos(Parent),Size(Parent),0,NULL,Style());

    for ( size_t i = 0; i < ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( ArrayChecks[i] )
        {
            Preview->Check(Val);
        }
    }
    return SetupWindow(Preview,Exact);
}


void wxsCheckListBox::EnumWidgetProperties(long Flags)
{
    WXS_ARRAYSTRINGCHECK(wxsCheckListBox,ArrayChoices,ArrayChecks,0,_("Choices"),_T("content"),_T("item"));
}

void wxsCheckListBox::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/checklst.h>")); return;
    }

    wxsLANGMSG(wxsCheckListBox::EnumDeclFiles,Language);
}
