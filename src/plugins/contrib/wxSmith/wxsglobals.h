#ifndef __WXSGLOBALS_H
#define __WXSGLOBALS_H

#include <wx/string.h>
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <cbplugin.h>


////////////////////////////////////////////////////////////////////////////////
//
//  Misc fuunctions
// =================
//
//


/** Changing given string to it's representation in C++.
 *
 *  Could be used when need to throw some string to generated code
 */
wxString GetCString(const wxString& Source);

/** Changing given string to it's representation in wxWidgets */
wxString GetWxString(const wxString& Source);

/** checkign if given work is a valid c++ Identifier */
bool ValidateIdentifier(const wxString& Name);


////////////////////////////////////////////////////////////////////////////////
//
//  Enums
// =======
//


/** Flags defining type of resource edit mode */
enum wxsResEditMode
{
	wxsREMFile = 0, ///< In this edit mode resource is threated as resource from outside, not binded automatically into code
	wxsREMSource,   ///< In this edit mode resource is fully generated inside source file, no additional resource files needed at run-time
	wxsREMMixed,    ///< In this edit mode resource is integrated with source code but additional resource file is needed at run-time
	/*-----------------*/
	wxsREMCount     ///< Number of available edit modes
};

/** Enum for drag assist types */
enum wxsDragAssistTypes
{
    wxsDTNone = 0,  ///< Nothing will be done to ease dragging
    wxsDTSimple,    ///< Only border aruond widgets will be drawn to help dragging
    wxsDTColourMix  ///< Sme widgets will change colour while dragging
};


////////////////////////////////////////////////////////////////////////////////
//
//  Constants
// ===========
//


/** Constant used to notify that there's no colour used */
const wxUint32 wxsNO_COLOUR = wxSYS_COLOUR_MAX + 1;

/** Constant used to notify that custom colour (not system) is used */
const wxUint32 wxsCUSTOM_COLOUR = wxPG_COLOUR_CUSTOM;


////////////////////////////////////////////////////////////////////////////////
//
//  Macros
// ========
//


/** Macro used to produce headers for block of code */
#define wxsBHeader(HName,CName) \
    _T("//(*") _T(HName) _T("(") _T(CName) _T(")")

/** Macro producing header for block of code where class is specified as argument fo Printf */
#define wxsBHeaderF(HName) \
    wxsBHeader(HName,"%s")

/** Macro containing end of code block */
#define wxsBEnd()   \
    _T("//*)")

/** Macro returning pointer to wxSmith plugin class */
#define wxsPLUGIN()    wxSmith::Get()

/** Macro returning current resource tree */
#define wxsTREE()  wxsPLUGIN()->GetResourceTree()

/** Macro returning current palette */
//#define wxsPALETTE() wxsPalette::Get()

/** Macro returing current properties window */
#define wxsPROPERTIES() wxsPropertiesMan::Get()

/** Macro returning widgets factory */
#define wxsFACTORY() wxsWidgetFactory::Get()

/** Macro generating widget with given name */
#define wxsGEN(Name,Resource) wxsFACTORY()->Generate(Name,Resource)

/** Macro destrtoying widget */
#define wxsKILL(Widget) wxsFACTORY()->Kill(Widget)


/** Getting current drag assist mode from configuration */
#define wxsDWAssistType ConfigManager::Get()->Read(_T("/wxsmith/dragassisttype"),(long)wxsDTColourMix)

/** Reading Drag target colour from configuration */
#define wxsDWTargetCol ConfigManager::Get()->Read(_T("/wxsmith/dragtargetcol"),0x608CDFL)

/** Reading Drag paretn colour from configuration */
#define wxsDWParentCol ConfigManager::Get()->Read(_T("/wxsmith/dragparentcol"),0x0D177BL)

/** Reading fetch deelay from configuration */
#define wxsDWFetchDelay ConfigManager::Get()->Read(_T("/wxsmith/backfetchdelay"),50L)

/** Reading icon size inside palette */
#define wxsDWPalIconSize ConfigManager::Get()->Read(_T("/wxsmith/paletteiconsize"),16L)

/** Reading icon size inside palette */
#define wxsDWToolIconSize ConfigManager::Get()->Read(_T("/wxsmith/tooliconsize"),32L)


/** Shortcut to applications debug log */
#define DebLog Manager::Get()->GetMessageManager()->DebugLog

/** Default class declaration specifier */
#if defined(wxsDEBUG)
    #define WXSCLASS PLUGIN_EXPORT
#else
    #define WXSCLASS
#endif


#endif
