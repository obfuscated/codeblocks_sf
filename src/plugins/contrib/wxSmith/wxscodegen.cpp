#include "wxsheaders.h"
#include "wxscodegen.h"

wxsCodeGen::wxsCodeGen(wxsWidget* Widget,bool DontCreateRoot)
{
    PreviousUniqueNumber = 1;
    wxsCodeParams Params;
    Params.UniqueNumber = 1;
    Params.IsDirectParent = true;
	if ( DontCreateRoot )
	{
        Params.ParentName = _T("this");
		int Cnt = Widget->GetChildCount();
		for ( int i=0; i<Cnt; i++ )
		{
		    wxsWidget* Child = Widget->GetChild(i);
            Child->BuildCodeParams(Params);
			AppendCodeReq(Child,Params);
		}
	}
	else
	{
	    Widget->BuildCodeParams(Params);
		AppendCodeReq(Widget,Params);
	}

    BeautyCode(Code);
}

wxsCodeGen::~wxsCodeGen()
{
}

void wxsCodeGen::AppendCodeReq(wxsWidget* Widget,wxsCodeParams& ThisParams)
{
    if ( !Widget )
        return;

    Code.Append( Widget->GetProducingCode(ThisParams) );
    Code.Append(_T('\n'));

    int Cnt = Widget->GetChildCount();

    wxsCodeParams ChildParams;
    ChildParams.UniqueNumber = ++PreviousUniqueNumber;

    if ( Widget->GetInfo().Sizer )
    {
        ChildParams.ParentName = ThisParams.ParentName;
        ChildParams.IsDirectParent = false;
    }
    else
    {
        if ( Widget->GetParent() )
        {
            ChildParams.ParentName = Widget->BaseProperties.VarName;
        }
        else
        {
            ChildParams.ParentName = _T("this");
        }
        ChildParams.IsDirectParent = true;
    }

    for ( int i=0; i<Cnt; i++ )
    {
        wxsWidget* Child = Widget->GetChild(i);
        Child->BuildCodeParams(ChildParams);
        AppendCodeReq(Child,ChildParams);
    }

    Code.Append( Widget->GetFinalizingCode(ThisParams) );
    Code.Append(_T('\n'));
}

void wxsCodeGen::BeautyCode(wxString& Code)
{
    wxString NewCode;
    int Tabs = 0;

    const wxChar* Ptr = Code.c_str();

    for (;;)
    {
        // Cutting off initial part
        while ( *Ptr == _T(' ') || *Ptr==_T('\t') || *Ptr==_T('\n') || *Ptr==_T('\r') ) Ptr++;

        if ( !*Ptr ) break;

        // Adding spaces at the beginning of line
        NewCode.Append(_T('\t'),Tabs);

        // Adding characters till the end of line or till some other circumstances

        int BracketsCnt = 0;
        while ( *Ptr && *Ptr!=_T('{') && *Ptr!=_T('}') && *Ptr!=_T('\n') && *Ptr!=_T('\r') )
        {
            // Additional brackets counting will avoid line splitting inside for statement
            if ( *Ptr == _T('(') ) BracketsCnt++;
            else if ( *Ptr == _T(')') ) BracketsCnt--;
            else if ( *Ptr == _T(';') && !BracketsCnt ) break;
            NewCode.Append(*Ptr++);
        }

        if ( !*Ptr )
        {
            NewCode.Append(_T('\n'));
            break;
        }

        switch ( *Ptr++ )
        {
			case _T(';'):
				NewCode.Append(_T(';'));
				NewCode.Append(_T('\n'));
				break;

            case _T('\n'):
            case _T('\r'):
                NewCode.Append(_T('\n'));
                break;

            case _T('{'):
                NewCode.Append(_T('\n'));
                NewCode.Append(_T('\t'),Tabs);
                NewCode.Append(_T("{\n"));
                Tabs++;
                break;

            case _T('}'):
                NewCode.Append(_T('\n'));
                Tabs--;
                if ( Tabs < 0 ) Tabs = 0;
                NewCode.Append(_T('\t'),Tabs);
                NewCode.Append(_T("}\n"));
                break;
        }
    }

    Code = NewCode;
}
