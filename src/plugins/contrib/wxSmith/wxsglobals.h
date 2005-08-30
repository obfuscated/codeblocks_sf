#ifndef __WXSGLOBALS_H
#define __WXSGLOBALS_H

#include <wx/string.h>

/** Changing given string to it's representation in C++.
 *
 *  Could be used when need to throw some string to generated code
 */
wxString GetCString(const wxString& Source);
        
/** Changing given string to it's representation in wxWidgets */
wxString GetWxString(const wxString& Source);

/** checkign if given work is a valid c++ Identifier */
bool ValidateIdentifier(const wxString& Name);


/** Macro used to produce headers for block of code */
#define wxsBHeader(HName,CName) \
    _T("//(*") _T(HName) _T("(") _T(CName) _T(")")
    
/** Macro producing header for block of code where class is specified as argument fo Printf */
#define wxsBHeaderF(HName) \
    wxsBHeader(HName,"%s")
    
/** Macro containing end of code block */
#define wxsBEnd()   \
    _T("//**)")


#endif
