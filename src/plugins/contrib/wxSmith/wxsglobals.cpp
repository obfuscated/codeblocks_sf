#include "wxsglobals.h"

wxString GetCString(const wxString& Source)
{
    wxString Result = _T("\"");
    
    int Len = Source.Length();
    
    for ( int i=0; i<Len; i++ )
    {
        wxChar ch = Source.GetChar(i);

        if ( ch < _T(' ') )
        {
            switch ( ch )
            {
                case _T('\n') : Result.Append(_T("\\n")); break;
                case _T('\t') : Result.Append(_T("\\t")); break;
                case _T('\v') : Result.Append(_T("\\v")); break;
                case _T('\b') : Result.Append(_T("\\b")); break;
                case _T('\r') : Result.Append(_T("\\r")); break;
                case _T('\f') : Result.Append(_T("\\f")); break;
                case _T('\a') : Result.Append(_T("\\a")); break;
                default   :
                    {
                        wxString Formater = wxString::Format(_T("\\%d%d%d"),
                            ( ch >> 6 ) & 7,
                            ( ch >> 3 ) & 7,
                            ( ch >> 0 ) & 7 );
                        Result.Append(Formater.c_str());
                    }
            }
        }
        else 
        {
            switch ( ch )
            {
                case _T('\\'): Result.Append(_T("\\\\")); break;
                case _T('\?'): Result.Append(_T("\\\?")); break;
                case _T('\''): Result.Append(_T("\\\'")); break;
                case _T('\"'): Result.Append(_T("\\\"")); break;
                default  : Result.Append(ch);
            }
        }
    }

    Result.Append(_T('\"'));
    return Result;
}

wxString GetWxString(const wxString& Source)
{
	return wxString::Format(_T("_(%s)"),GetCString(Source).c_str());
}

/** \brief set of names which can not be used as widget names
 *
 * This names must be placed in alphabetical order
 */
static const wxChar* DeadNames[] = 
{
    _T("asm"), _T("auto"), _T("bool"), _T("break"), _T("case"), _T("catch"), 
    _T("char"), _T("class"), _T("const"), _T("const_cast"), _T("continue"), 
    _T("default"), _T("delete"), _T("do"), _T("double"), _T("dynamic_cast"), 
    _T("else"), _T("enum"), _T("explicit"), _T("export"), _T("extern"), 
    _T("false"), _T("float"), _T("for"), _T("friend"), _T("goto"), _T("if"),
    _T("inline"), _T("int"), _T("long"), _T("mutable"), _T("namespace"), 
    _T("new"), _T("operator"), _T("private"), _T("protected"), _T("public"), 
    _T("register"), _T("reinterpret_cast"), _T("return"), _T("short"), 
    _T("signed"), _T("sizeof"), _T("sizeritem"), _T("static"), 
    _T("static_cast"), _T("struct"), _T("switch"), _T("template"), _T("this"), 
    _T("throw"), _T("true"), _T("try"), _T("typedef"), _T("typeid"), 
    _T("typename"), _T("union"), _T("unsigned"), _T("using"), _T("virtual"), 
    _T("void"), _T("volatile"), _T("wchar_t"), _T("while")
};

static const int DeadNamesLen = sizeof(DeadNames) / sizeof(DeadNames[0]);

bool ValidateIdentifier(const wxString& NameStr)
{
    const wxChar* Name = NameStr.c_str();
    if ( !Name ) return false;
    
    if (( *Name < _T('a') || *Name > _T('z') ) &&
        ( *Name < _T('A') || *Name > _T('Z') ) &&
        ( *Name != _T('_') ))
    {
        return false;
    }
    
    while ( *++Name )
    {
        if (( *Name < _T('a') || *Name > _T('z') ) &&
            ( *Name < _T('A') || *Name > _T('Z') ) &&
            ( *Name < _T('0') || *Name > _T('9') ) &&
            ( *Name != _T('_') ))
        {
            return false;
        }
    }
   
    int Begin = 0;
    int End = DeadNamesLen-1;
    
    Name = NameStr.c_str();
    
    while ( Begin <= End )
    {
        int Middle = ( Begin + End ) >> 1;
        
        int Res = wxStrcmp(DeadNames[Middle],Name);
        
        if ( Res < 0 )
        {
            Begin = Middle+1;
        }
        else if ( Res > 0 )
        {
            End = Middle-1;
        }
        else 
        {
            return false;
        }
        
    }
    
    return true;
}
