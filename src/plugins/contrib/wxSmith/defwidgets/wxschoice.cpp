#include "../wxsheaders.h"
#include "wxschoice.h"
#include <wx/choice.h>

WXS_ST_BEGIN(wxsChoiceStyles)
    WXS_ST_CATEGORY("wxChoice")
    WXS_ST(wxCB_SORT)
WXS_ST_END(wxsChoiceStyles)

WXS_EV_BEGIN(wxsChoiceEvents)
    WXS_EVI(EVT_CHOICE,wxCommandEvent,Select)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsChoiceEvents)

wxsDWDefineBegin(wxsChoiceBase,wxChoice,
        WXS_THIS = new wxChoice(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,0,0,WXS_STYLE,wxDefaultValidator,WXS_NAME);
	)

    wxsDWDefIntX(defaultChoice,"selection","",-1)
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice,wxCB_SORT)

wxsDWDefineEnd()

wxWindow* wxsChoice::MyCreatePreview(wxWindow* Parent)
{
    wxChoice* Combo = new wxChoice(Parent,GetId(),GetPosition(),GetSize(),arrayChoices,GetStyle(),wxDefaultValidator,GetName());
    if ( defaultChoice >= 0 && defaultChoice < (int)arrayChoices.Count() )
    {
        Combo->SetSelection(defaultChoice);
    }
    PreviewApplyDefaults(Combo);
    return Combo;
}

wxString wxsChoice::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Code;
    Code.Printf(_T("%s = new wxChoice(%s,%s,%s,%s,0,NULL,%s,wxDefaultValidator,%s);\n"),
            Params.VarName.c_str(),
            Params.ParentName.c_str(),
            Params.IdName.c_str(),
            Params.Pos.c_str(),
            Params.Size.c_str(),
            Params.Style.c_str(),
            Params.Name.c_str());
    for ( size_t i = 0; i <  arrayChoices.Count(); ++i )
    {
        Code << wxString::Format(_T("%s->Append(%s);\n"),
            Params.VarName.c_str(),
            wxsGetWxString(arrayChoices[i]).c_str());
    }

    if ( defaultChoice >= 0 && defaultChoice < (int)arrayChoices.Count() )
    {
        Code << wxString::Format(_T("%s->SetSelection(%d);\n"),
            Params.VarName.c_str(),
            defaultChoice);
    }

    Code << Params.InitCode;

    return Code;
}
