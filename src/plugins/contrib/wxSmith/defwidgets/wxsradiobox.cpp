#include "wxsradiobox.h"

WXS_ST_BEGIN(wxsRadioBoxStyles)
    WXS_ST_CATEGORY("wxRadioBox")
    WXS_ST(wxRA_SPECIFY_ROWS)
    WXS_ST(wxRA_SPECIFY_COLS)    
#ifdef __PALMOS__    
    WXS_ST(wxRA_USE_CHECKBOX)
#endif    
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsRadioBoxStyles)

// Constructor
//wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")

wxsDWDefineBegin(wxsRadioBoxBase,wxRadioBox,
    )

    #ifdef __NO_PROPGRID
        wxsDWDefIntX(defaultChoice,"selection","Default",-1)
    #else
        wxsDWDefIntX(defaultChoice,"selection","",-1)
    #endif
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice,0)
    wxsDWDefInt(dimension,"Dimension",0)
wxsDWDefineEnd()

wxWindow* wxsRadioBox::MyCreatePreview(wxWindow* Parent)
{
    wxArrayString Arr = arrayChoices;
    Arr.Add(_T(""));
    wxRadioBox* Preview =  new wxRadioBox(Parent,-1,label,
        GetPosition(),GetSize(),Arr,dimension,GetStyle());
	if ( defaultChoice ) Preview->SetSelection(defaultChoice);
	return Preview;
}

wxString wxsRadioBox::GetProducingCode(wxsCodeParams& Params)
{
    const CodeDefines& CDefs = GetCodeDefines();
	if ( arrayChoices.Count() == 0 )
	{
		return wxString::Format(_T("%s = new wxRadioBox(%s,%s,%s,%s,%s,0,NULL,%d,%s)"),
            Params.ParentName.c_str(),
            GetBaseParams().VarName.c_str(),
            GetBaseParams().IdName.c_str(),
            GetWxString(label).c_str(),
            CDefs.Pos.c_str(),
            CDefs.Size.c_str(),
            dimension,
            CDefs.Style.c_str());
	}
	
	wxString Code;
	Code.Printf(_T("wxStringArray wxRadioBoxChoices%d;\n"),Params.UniqueNumber);
	for ( size_t i = 0; i < arrayChoices.Count(); ++i )
	{
		Code.Append( wxString::Format(_T("wxRadioBoxChoices%d.Append(%s);\n"),Params.UniqueNumber,i,GetWxString(arrayChoices[i]).c_str()) );
	}
	
    Code.Append ( wxString::Format(_T("%s = new wxRadioBox(%s,%s,%s,%s,%s,wxRadioBoxChoices%d,%d,%s)"),
        Params.ParentName.c_str(),
        GetBaseParams().VarName.c_str(),
        GetBaseParams().IdName.c_str(),
        GetWxString(label).c_str(),
        CDefs.Pos.c_str(),
        CDefs.Size.c_str(),
        Params.UniqueNumber,
        dimension,
        CDefs.Style.c_str()) );
    
    return Code;
}
