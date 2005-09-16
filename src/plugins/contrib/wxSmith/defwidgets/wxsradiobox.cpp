#include "wxsradiobox.h"

#include "../wxsglobals.h"

WXS_ST_BEGIN(wxsRadioBoxStyles)
    WXS_ST_CATEGORY("wxRadioBox")
    WXS_ST(wxRA_SPECIFY_ROWS)
    WXS_ST(wxRA_SPECIFY_COLS)    
#ifdef __PALMOS__    
    WXS_ST(wxRA_USE_CHECKBOX)
#endif    
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsRadioBoxStyles)

WXS_EV_BEGIN(wxsRadioBoxEvents)
    WXS_EVI(EVT_RADIOBOX,wxCommandEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsRadioBoxEvents)

// Constructor
//wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")

wxsDWDefineBegin(wxsRadioBoxBase,wxRadioBox,
    /* This code only avoids generating some warnings - it won't be used anywhere */
    ThisWidget = new wxRadioBox(parent,id,_T(""),pos,size,0,NULL,1,style);
    )

    wxsDWDefStr(label,"Label:","");
    #ifdef __NO_PROPGRID
        wxsDWDefIntX(defaultChoice,"selection","Default:",-1)
    #else
        wxsDWDefIntX(defaultChoice,"selection","",-1)
    #endif
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices:",defaultChoice,-1)
    wxsDWDefInt(dimension,"Dimension:",1)
wxsDWDefineEnd()

wxWindow* wxsRadioBox::MyCreatePreview(wxWindow* Parent)
{
	if ( dimension < 1 ) dimension = 1;
    wxRadioBox* Preview =  new wxRadioBox(Parent,-1,label,
        GetPosition(),GetSize(),arrayChoices,dimension,GetStyle());
	if ( defaultChoice >= 0 ) Preview->SetSelection(defaultChoice);
	PreviewApplyDefaults(Preview);
	return Preview;
}

wxString wxsRadioBox::GetProducingCode(wxsCodeParams& Params)
{
	if ( dimension < 1 ) dimension = 1;
    const CodeDefines& CDefs = GetCodeDefines();
	if ( arrayChoices.Count() == 0 )
	{
		return wxString::Format(_T("%s = new wxRadioBox(%s,%s,%s,%s,%s,0,NULL,%d,%s);"),
            GetBaseParams().VarName.c_str(),
            Params.ParentName.c_str(),
            GetBaseParams().IdName.c_str(),
            GetWxString(label).c_str(),
            CDefs.Pos.c_str(),
            CDefs.Size.c_str(),
            dimension,
            CDefs.Style.c_str());
	}
	
	wxString Code;
	Code.Printf(_T("wxString wxRadioBoxChoices%d[%d];\n"),Params.UniqueNumber,arrayChoices.Count());
	for ( size_t i = 0; i < arrayChoices.Count(); ++i )
	{
		Code.Append( wxString::Format(_T("wxRadioBoxChoices%d[%d] = %s;\n"),Params.UniqueNumber,i,GetWxString(arrayChoices[i]).c_str()) );
	}
	
    Code.Append ( wxString::Format(_T("%s = new wxRadioBox(%s,%s,%s,%s,%s,%d,wxRadioBoxChoices%d,%d,%s);"),
        GetBaseParams().VarName.c_str(),
        Params.ParentName.c_str(),
        GetBaseParams().IdName.c_str(),
        GetWxString(label).c_str(),
        CDefs.Pos.c_str(),
        CDefs.Size.c_str(),
        arrayChoices.Count(),
        Params.UniqueNumber,
        dimension,
        CDefs.Style.c_str()) );
        
    if ( defaultChoice >= 0 )
    {
    	Code.Append( wxString::Format(_T("%s->SetSelection(%d);"),GetBaseParams().VarName.c_str(),defaultChoice) );
    }
    
    Code << CDefs.InitCode;
    
    return Code;
}
