#include "wxsflexgridsizer.h"

wxsItemInfo wxsFlexGridSizer::Info =
{
    _T("wxFlexGridSizer"),
    wxsTSizer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Layout"),
    80,
    _T("FlexGridSizer"),
    2, 6,
    NULL,
    NULL,
    0
};

wxSizer* wxsFlexGridSizer::BuildSizerPreview(wxWindow* Parent)
{
	wxFlexGridSizer* Sizer = new wxFlexGridSizer(Rows,Cols,
        wxsDimensionProperty::GetPixels(VGap,VGapDU,Parent),
        wxsDimensionProperty::GetPixels(HGap,HGapDU,Parent));

    wxArrayInt Cols = GetArray(GrowableCols);
    for ( size_t i=0; i<Cols.Count(); i++ )
    {
    	Sizer->AddGrowableCol(Cols[i]);
    }

    wxArrayInt Rows = GetArray(GrowableRows);
    for ( size_t i=0; i<Rows.Count(); i++ )
    {
    	Sizer->AddGrowableRow(Rows[i]);
    }
    return Sizer;
}

void wxsFlexGridSizer::BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << GetVarName() << _T(" = new wxFlexGridSizer(")
                 << wxString::Format(_T("%d,%d"),Rows,Cols) << _T(",")
                 << wxsDimensionProperty::GetPixelsCode(VGap,VGapDU,WindowParent,wxsCPP) << _T(",")
                 << wxsDimensionProperty::GetPixelsCode(HGap,HGapDU,WindowParent,wxsCPP) << _T(");\n");

            wxArrayInt Cols = GetArray(GrowableCols);
            for ( size_t i=0; i<Cols.Count(); i++ )
            {
                Code << GetVarName() << wxString::Format(_T("->AddGrowableCol(%d);\n"),Cols[i]);
            }

            wxArrayInt Rows = GetArray(GrowableRows);
            for ( size_t i=0; i<Rows.Count(); i++ )
            {
                Code << GetVarName() << wxString::Format(_T("->AddGrowableRow(%d);\n"),Rows[i]);
            }

            return;
    }

    wxsLANGMSG(wxsFlexGridSizer::BuildSizerCreatingCode,Language);
}

void wxsFlexGridSizer::EnumItemProperties(long Flags)
{
    WXS_LONG(wxsFlexGridSizer,Cols,0,_("Cols"),_T("cols"),0);
    WXS_LONG(wxsFlexGridSizer,Rows,0,_("Rows"),_T("rows"),0);
    WXS_DIMENSION(wxsFlexGridSizer,VGap,VGapDU,0,_("V-Gap"),_("V-Gap in dialog units"),_T("vgap"),0,false);
    WXS_DIMENSION(wxsFlexGridSizer,HGap,HGapDU,0,_("H-Gap"),_("H,y-Gap in dialog units"),_T("hgap"),0,false);
    WXS_STRING(wxsFlexGridSizer,GrowableCols,0,_("Growable cols"),_T("growablecols"),wxEmptyString,false,false);
    WXS_STRING(wxsFlexGridSizer,GrowableRows,0,_("Growable rows"),_T("growablerows"),wxEmptyString,false,false);
    FixupList(GrowableCols);
    FixupList(GrowableRows);
}

wxArrayInt wxsFlexGridSizer::GetArray(const wxString& String,bool* Valid)
{
	wxStringTokenizer Tokens(String,_T(","));
	wxArrayInt Array;
	if ( Valid )
	{
		*Valid = true;
	}

	while ( Tokens.HasMoreTokens() )
	{
		long Value;
		wxString Token = Tokens.GetNextToken();
		Token.Trim(true);
		Token.Trim(false);
		if ( !Token.ToLong(&Value) && Valid )
		{
			*Valid = false;
		}
		Array.Add((int)Value);
	}

	return Array;
}

bool wxsFlexGridSizer::FixupList(wxString& List)
{
	bool Ret;
	wxArrayInt Array = GetArray(List,&Ret);
	List.Clear();
	for ( size_t i=0; i<Array.Count(); i++ )
	{
		List.Append(wxString::Format(_T("%d"),Array[i]));
		if ( i < Array.Count() - 1 )
		{
			List.Append(_T(','));
		}
	}
	return Ret;
}

void wxsFlexGridSizer::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
    }
    wxsLANGMSG(wxsFlexGridSizer::EnumDeclFiles,Language);
}
