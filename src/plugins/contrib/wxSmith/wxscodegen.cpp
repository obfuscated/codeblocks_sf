#include "wxscodegen.h"

wxsCodeGen::wxsCodeGen(wxsWidget* Widget,int InitialSpaces,int TabSize)
{
    wxsCodeParams Params;

    Params.ParentName = "Parent";
    Params.IsDirectParent = true;
    Params.UniqueNumber = 1;

    AppendCodeReq(Widget,Params);
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
    Code.Append('\n');

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
        ChildParams.ParentName = Widget->GetBaseParams().VarName.c_str();
        ChildParams.IsDirectParent = true;
    }

    for ( int i=0; i<Cnt; i++ )
    {
        AppendCodeReq(Widget->GetChild(i),ChildParams);
        ChildParams.UniqueNumber++;
    }

    Code.Append( Widget->GetFinalizingCode(ThisParams) );
    Code.Append('\n');

    ThisParams.UniqueNumber = ChildParams.UniqueNumber - 1;
}

void wxsCodeGen::BeautyCode(wxString& Code,int Spaces,int TabSize)
{
    wxString NewCode;

    const char* Ptr = Code.c_str();

    for (;;)
    {
        // Cutting off initial part
        while ( *Ptr == ' ' || *Ptr=='\t' || *Ptr=='\n' || *Ptr=='\r' ) Ptr++;

        if ( !*Ptr ) break;

        // Adding spaces at the beginning of line
        NewCode.Append(' ',Spaces);
        
        // Adding characters till the end of line or till some other circumstances
        
        while ( *Ptr && *Ptr!='{' && *Ptr!='}' && *Ptr != '\n' && *Ptr != '\r' )
            NewCode.Append(*Ptr++);
            
        if ( !*Ptr )
        {
            NewCode.Append('\n');
            break;
        }
        
        switch ( *Ptr )
        {
            case '\n':
            case '\r':
                NewCode.Append('\n');
                break;
                
            case '{':
                NewCode.Append('\n');
                NewCode.Append(' ',Spaces);
                NewCode.Append("{\n");
                Spaces += TabSize;
                break;
                
            case '}':
                NewCode.Append('\n');
                Spaces -= TabSize;
                if ( Spaces < 0 ) Spaces = 0;
                NewCode.Append(' ',Spaces);
                NewCode.Append("}\n");
                break;
        }
    }
    
    Code = NewCode;
}
