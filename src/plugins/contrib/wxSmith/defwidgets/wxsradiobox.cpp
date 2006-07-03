#include "../wxsheaders.h"
#include "wxsradiobox.h"

#include "../wxsglobals.h"

WXS_ST_BEGIN(wxsRadioBoxStyles)
    WXS_ST_CATEGORY("wxRadioBox")
    WXS_ST(wxRA_SPECIFY_ROWS)
    WXS_ST(wxRA_SPECIFY_COLS)
// NOTE (cyberkoa##): wxRA_HORIZONTAL & wxRA_VERTICAL is not in HELP file but in wxMSW's XRC
    WXS_ST_MASK(wxRA_HORIZONTAL,wxsSFAll,0,false)
    WXS_ST_MASK(wxRA_VERTICAL,wxsSFAll,0,false)
    WXS_ST_MASK(wxRA_USE_CHECKBOX,wxsSFPALMOS,0,true)

WXS_ST_END(wxsRadioBoxStyles)

WXS_EV_BEGIN(wxsRadioBoxEvents)
    WXS_EVI(EVT_RADIOBOX,wxCommandEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsRadioBoxEvents)

// Constructor
//wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")

wxsDWDefineBegin(wxsRadioBoxBase,wxRadioBox,
    WXS_THIS = new wxRadioBox(WXS_PARENT,WXS_ID,_T(""),WXS_POS,WXS_SIZE,0,NULL,1,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    )

    wxsDWDefStr(label,"Label:","");
    wxsDWDefIntX(defaultChoice,"selection","",-1)
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices:",defaultChoice,-1)
    wxsDWDefInt(dimension,"Dimension:",1)
wxsDWDefineEnd()

wxWindow* wxsRadioBox::MyCreatePreview(wxWindow* Parent)
{
	if ( dimension < 1 ) dimension = 1;
    wxRadioBox* Preview =  new wxRadioBox(Parent,GetId(),label,
        GetPosition(),GetSize(),arrayChoices,dimension,GetStyle());
	if ( defaultChoice >= 0 ) Preview->SetSelection(defaultChoice);
	PreviewApplyDefaults(Preview);
	return Preview;
}

wxString wxsRadioBox::GetProducingCode(const wxsCodeParams& Params)
{
	if ( dimension < 1 ) dimension = 1;
	if ( arrayChoices.Count() == 0 )
	{
		return wxString::Format(_T("%s = new wxRadioBox(%s,%s,%s,%s,%s,0,NULL,%d,%s,%s);"),
            Params.VarName.c_str(),
            Params.ParentName.c_str(),
            Params.IdName.c_str(),
            wxsGetWxString(label).c_str(),
            Params.Pos.c_str(),
            Params.Size.c_str(),
            dimension,
            Params.Style.c_str(),
            Params.Name.c_str());
	}

	wxString Code;
	Code.Printf(_T("wxString wxRadioBoxChoices_%s[%d];\n"),Params.VarName.c_str(),arrayChoices.Count());
	for ( size_t i = 0; i < arrayChoices.Count(); ++i )
	{
		Code.Append( wxString::Format(_T("wxRadioBoxChoices_%s[%d] = %s;\n"),Params.VarName.c_str(),i,wxsGetWxString(arrayChoices[i]).c_str()) );
	}

    Code.Append ( wxString::Format(_T("%s = new wxRadioBox(%s,%s,%s,%s,%s,%d,wxRadioBoxChoices_%s,%d,%s);"),
        Params.VarName.c_str(),
        Params.ParentName.c_str(),
        Params.IdName.c_str(),
        wxsGetWxString(label).c_str(),
        Params.Pos.c_str(),
        Params.Size.c_str(),
        arrayChoices.Count(),
        Params.VarName.c_str(),
        dimension,
        Params.Style.c_str()) );

    if ( defaultChoice >= 0 )
    {
    	Code.Append( wxString::Format(_T("%s->SetSelection(%d);"),Params.VarName.c_str(),defaultChoice) );
    }

    Code << Params.InitCode;

    return Code;
}
