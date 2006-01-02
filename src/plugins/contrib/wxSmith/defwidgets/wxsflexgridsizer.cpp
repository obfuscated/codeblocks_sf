#include "../wxsheaders.h"
#include "wxsflexgridsizer.h"

#include "../properties/wxsstringproperty.h"
#include <wx/tokenzr.h>

namespace
{
    class wxsFlexGridSizerColsRowsProperty: public wxsStringProperty
    {
    	public:
            wxsFlexGridSizerColsRowsProperty(wxString& String, bool AlwaysUpdate):
                wxsStringProperty(String,AlwaysUpdate)
            {}

        protected:

            virtual wxString CorrectValue(const wxString& Value)
            {
            	wxString Return = Value;
            	wxsFlexGridSizer::FixupList(Return);
            	return Return;
            }
    };
};

wxString wxsFlexGridSizer::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Code;
    Code.Printf(_T("%s = new wxFlexGridSizer(%d,%d,%d,%d);"),
        Params.VarName.c_str(),
        Rows, Cols, VGap, HGap );

    wxArrayInt Cols = GetArray(GrowableCols);
    for ( size_t i=0; i<Cols.Count(); i++ )
    {
    	Code.Append(wxString::Format(_T("\n%s->AddGrowableCol(%d);"),
            Params.VarName.c_str(),
            Cols[i]));
    }

    wxArrayInt Rows = GetArray(GrowableRows);
    for ( size_t i=0; i<Rows.Count(); i++ )
    {
    	Code.Append(wxString::Format(_T("\n%s->AddGrowableRow(%d);"),
            Params.VarName.c_str(),
            Rows[i]));
    }

    return Code;
}

bool wxsFlexGridSizer::MyXmlLoad()
{
    Rows = XmlGetInteger(_T("rows"));
    Cols = XmlGetInteger(_T("cols"));
    VGap = XmlGetInteger(_T("vgap"));
    HGap = XmlGetInteger(_T("hgap"));
    GrowableRows = XmlGetVariable(_T("growablerows"));
    GrowableCols = XmlGetVariable(_T("growablecols"));
    bool Ret = true;
    if ( !FixupList(GrowableCols) ) Ret = false;
    if ( !FixupList(GrowableRows) ) Ret = false;
    return Ret;
}

bool wxsFlexGridSizer::MyXmlSave()
{
    XmlSetInteger(_T("rows"),Rows);
    XmlSetInteger(_T("cols"),Cols);
    XmlSetInteger(_T("vgap"),VGap);
    XmlSetInteger(_T("hgap"),HGap);
    FixupList(GrowableCols);
    FixupList(GrowableRows);
    if ( GrowableCols.Length() )
    {
        XmlSetVariable(_T("growablecols"),GrowableCols);
    }
    if ( GrowableRows.Length() )
    {
        XmlSetVariable(_T("growablerows"),GrowableRows);
    }
    return true;
}


void wxsFlexGridSizer::MyCreateProperties()
{
    Properties.Add2IProperty(_("Cols x rows:"),Cols,Rows);
    Properties.Add2IProperty(_("VGap x HGap:"),VGap,HGap);
    Properties.AddProperty(_("Growable cols:"),
        new wxsFlexGridSizerColsRowsProperty(GrowableCols,true));
    Properties.AddProperty(_("Growable rows:"),
        new wxsFlexGridSizerColsRowsProperty(GrowableRows,true));
    wxsWidget::MyCreateProperties();
}

void wxsFlexGridSizer::Init()
{
	Rows = 0;
	Cols = 0;
	VGap = 0;
	HGap = 0;
	GrowableCols = _T("");
	GrowableRows = _T("");
}

wxSizer* wxsFlexGridSizer::ProduceSizer(wxWindow* Panel)
{
	wxFlexGridSizer* Sizer = new wxFlexGridSizer(Rows,Cols,VGap,HGap);
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
	List = _T("");
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
