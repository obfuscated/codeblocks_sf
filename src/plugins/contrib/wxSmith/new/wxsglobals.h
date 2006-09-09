#ifndef __WXSGLOBALS_H
#define __WXSGLOBALS_H

#include <wx/string.h>
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include <wx/dialog.h>
#include <wx/bitmap.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>

#include <cbplugin.h>

////////////////////////////////////////////////////////////////////////////////
//
//  Misc functions
// ================
//
//

/** \brief Changing given string to it's representation in C++.
 *
 *  Could be used when need to throw some string to generated code
 */
wxString wxsGetCString(const wxString& Source);

/** \brief Changing given string to it's representation in wxWidgets */
wxString wxsGetWxString(const wxString& Source);

/** \brief Replacing one string inside source code with another.
 *
 *  This function grants that it replace strings only
 *  if it won't break c++ identifiers
 */
void wxsCodeReplace(wxString& Code,const wxString& Old,const wxString& New);

/** \brief Checkign if given word is a valid c++ Identifier */
bool wxsValidateIdentifier(const wxString& Name);


////////////////////////////////////////////////////////////////////////////////
//
//  Constants
// ===========
//

// Availability flags used in window resource and in properties filtering
static const long wxsFLFile     = 0x0001;       ///< \brief Edition in file mode
static const long wxsFLSource   = 0x0002;       ///< \brief Edition in source mode
static const long wxsFLMixed    = 0x0004;       ///< \brief Edition in mixed mode
static const long wxsFLVariable = 0x0008;       ///< \brief Flag turning on variable
static const long wxsFLId       = 0x0010;       ///< \brief Flag turning on identifier

////////////////////////////////////////////////////////////////////////////////
//
//  Enums
// =======
//

/** \brief Enum for drag assist types */
enum wxsDragAssistTypes
{
    wxsDTNone = 0,  ///< \brief Nothing will be done to ease dragging
    wxsDTSimple,    ///< \brief Only border aruond widgets will be drawn to help dragging
    wxsDTColourMix  ///< \brief Some widgets will change colour while dragging
};


////////////////////////////////////////////////////////////////////////////////
//
//  Macros
// ========
//

/** \brief Macro declaring beginning of any block of code */
#define wxsBBegin() \
    _T("//(*")

/** \brief Macro used to produce headers for block of code */
#define wxsBHeader(HName,CName) \
    wxsBBegin() _T(HName) _T("(") _T(CName) _T(")")

/** \brief Macro producing header for block of code where class is specified as argument fo Printf */
#define wxsBHeaderF(HName) \
    wxsBHeader(HName,"%s")

/** \brief Macro containing end of code block */
#define wxsBEnd()   \
    _T("//*)")


/** \brief Macro generating widget with given name */
#define wxsGEN(Name,Resource) wxsItemFactory::Get()->Generate(Name,Resource)

/** \brief Macro destroying widget
 * \warning Item must be manually unbinded from it's parent using wxsParent::UnbindChild,
 *          otherwise parent will keep pointer to invalid Item,
 */
#define wxsKILL(Widget) wxsItemFactory::Get()->Kill(Widget)

/** \brief Macro adding new code to coder */
#define wxsADDCODE(FileName,Header,End,Code) wxsCoder::Get()->AddCode(FileName,Header,End,Code,true,false,false)

/** \brief Macro fetching code from file */
#define wxsGETCODE(FileName,Header,End) wxsCoder::Get()->GetCode(FileName,Header,End,false,false)

/** \brief Getting current drag assist mode from configuration */
#define wxsDWAssistType Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/dragassisttype"),(long)wxsDTColourMix)

/** \brief Reading Drag target colour from configuration */
#define wxsDWTargetCol Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/dragtargetcol"),0x608CDFL)

/** \brief Reading Drag paretn colour from configuration */
#define wxsDWParentCol Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/dragparentcol"),0x0D177BL)

/** \brief Reading fetch deelay from configuration */
#define wxsDWFetchDelay Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/backfetchdelay"),50L)

/** \brief Reading icon size inside palette */
#define wxsDWPalIconSize Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/paletteiconsize"),16L)

/** \brief Reading icon size inside palette */
#define wxsDWToolIconSize Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/tooliconsize"),32L)


/** \brief Misc macro displaying error message for unknown language */
#define wxsLANGMSG(Scope,Language) \
    DBGLOG(_T("wxSmith: Unknown coding language in %s (id: %d)"),_T(#Scope),Language);

#endif
