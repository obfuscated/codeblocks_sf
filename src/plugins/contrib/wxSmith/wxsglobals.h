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

/** Macro returning pointer to wxSmith plugin class */
#define wxsPLUGIN()    wxSmith::Get()

/** Macro returning current resource tree */
#define wxsTREE()  wxsPLUGIN()->GetResourceTree()

/** Macro returning current palette */
#define wxsPALETTE() wxsPalette::Get()

/** Macro returing current properties window */
#define wxsPROPERTIES() wxsPropertiesMan::Get()

/** Macro returning widgets factory */
#define wxsFACTORY() wxsWidgetFactory::Get()

/** Macro generating widget with given name */
#define wxsGEN(Name,Resource) wxsFACTORY()->Generate(Name,Resource)

/** Macro destrtoying widget */
#define wxsKILL(Widget) wxsFACTORY()->Kill(Widget)


/** Enum for drag assist types */
enum wxsDragAssistTypes
{
    wxsDTNone = 0,
    wxsDTSimple,
    wxsDTColourMix
};

/** Getting current drag assist mode from configuration */
#define wxsDWAssistType ConfigManager::Get()->Read(_T("/wxsmith/dragassisttype"),(long)wxsDTColourMix)

/** Reading Drag target colour from configuration */
#define wxsDWTargetCol ConfigManager::Get()->Read(_T("/wxsmith/dragtargetcol"),0x608CDFL)

/** Reading Drag paretn colour from configuration */
#define wxsDWParentCol ConfigManager::Get()->Read(_T("/wxsmith/dragparentcol"),0x0D177BL)

/** Reading fetch deelay from configuration */
#define wxsDWFetchDelay ConfigManager::Get()->Read(_T("/wxsmith/backfetchdelay"),50L)

#endif
