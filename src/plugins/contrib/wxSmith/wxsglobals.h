#ifndef __WXSGLOBALS_H
#define __WXSGLOBALS_H

#include <wx/string.h>
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

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
    _T("//*)")


/** Flags defining type of resource edit mode */
enum wxsResEditMode
{
	wxsResFile   = 0x0001,  ///< Setting this flag tells that resource will be loaded from external file (f.ex xrc)
	wxsResSource = 0x0002,  ///< Setting this flag tells that this resource is integrated with source code
	/*-----------------*/
	wxsResBroken = 0x8000   ///< Setting this flag tells that resource was broken (f.ex. errors in code integration)
};

/** Constant used to notify that there's no colour used */
const wxUint32 wxsNO_COLOUR = wxSYS_COLOUR_MAX + 1;

/** Constant used to notify that custom colour (not system) is used */
const wxUint32 wxsCUSTOM_COLOUR = wxPG_COLOUR_CUSTOM;

#endif
