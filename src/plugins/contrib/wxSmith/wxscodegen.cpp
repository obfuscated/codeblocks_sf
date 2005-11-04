#include "wxsheaders.h"
#include "wxscodegen.h"

wxsCodeGen::wxsCodeGen(wxsWidget* Widget,int InitialSpaces,int TabSize,bool DontCreateRoot)
{
	if ( DontCreateRoot )
	{
		int Cnt = Widget->GetChildCount();

		wxsCodeParams Params;
		Params.UniqueNumber = 1;
        Params.ParentName = _T("this");
        Params.IsDirectParent = true;

		for ( int i=0; i<Cnt; i++ )
		{
			AppendCodeReq(Widget->GetChild(i),Params);
			Params.UniqueNumber++;
		}
	}
	else
	{
		wxsCodeParams Params;
		Params.ParentName = _T("Parent");
		Params.IsDirectParent = true;
		Params.UniqueNumber = 1;
		AppendCodeReq(Widget,Params);
	}

    BeautyCode(Code,InitialSpaces,TabSize);
}

wxsCodeGen::~wxsCodeGen()
{
    //dtor
}

void wxsCodeGen::AppendCodeReq(wxsWidget* Widget,wxsCodeParams& ThisParams)
{
    if ( !Widget )
        return;

    Code.Append( Widget->GetProducingCode(ThisParams) );
    Code.Append(_T('\n'));

    int Cnt = Widget->GetChildCount();

    wxsCodeParams ChildParams;

    ChildParams.UniqueNumber = ThisParams.UniqueNumber + 1;

    if ( Widget->GetInfo().Sizer )
    {
        ChildParams.ParentName = ThisParams.ParentName;
        ChildParams.IsDirectParent = false;
    }
    else
    {
        if ( Widget->GetParent() )
        {
            ChildParams.ParentName = Widget->GetBaseProperties().VarName.c_str();
        }
        else
        {
            ChildParams.ParentName = _T("this");
        }
        ChildParams.IsDirectParent = true;
    }

    for ( int i=0; i<Cnt; i++ )
    {
        AppendCodeReq(Widget->GetChild(i),ChildParams);
        ChildParams.UniqueNumber++;
    }

    Code.Append( Widget->GetFinalizingCode(ThisParams) );
    Code.Append(_T('\n'));

    ThisParams.UniqueNumber = ChildParams.UniqueNumber - 1;
}

void wxsCodeGen::BeautyCode(wxString& Code,int Spaces,int TabSize)
{
    wxString NewCode;

    const wxChar* Ptr = Code.c_str();

    for (;;)
    {
        // Cutting off initial part
        while ( *Ptr == _T(' ') || *Ptr==_T('\t') || *Ptr==_T('\n') || *Ptr==_T('\r') ) Ptr++;

        if ( !*Ptr ) break;

        // Adding spaces at the beginning of line
        NewCode.Append(_T(' '),Spaces);

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
                NewCode.Append(_T(' '),Spaces);
                NewCode.Append(_T("{\n"));
                Spaces += TabSize;
                break;

            case _T('}'):
                NewCode.Append(_T('\n'));
                Spaces -= TabSize;
                if ( Spaces < 0 ) Spaces = 0;
                NewCode.Append(_T(' '),Spaces);
                NewCode.Append(_T("}\n"));
                break;
        }
    }

    Code = NewCode;
}
