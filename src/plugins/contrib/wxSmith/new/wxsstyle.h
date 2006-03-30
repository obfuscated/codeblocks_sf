#ifndef __WXSSTYLE_H
#define __WXSSTYLE_H

/* ************************************************************************** */
/*  Available style flags                                                     */
/* ************************************************************************** */

const long wxsSFXRC    = 0x00000001;    ///< This style can be used in XRC mode
const long wxsSFWin    = 0x00000002;    ///< This style can be used in Windows (any)
const long wxsSFWin95  = 0x00000004;    ///< This style can be used in Win95
const long wxsSFWinCE  = 0x00000008;    ///< This style can be used in WinCE
const long wxsSFGTK    = 0x00000010;    ///< This style can be used in GTK+ (any GTK)
const long wxsSFGTK12  = 0x00000020;    ///< This style can be used in GTK (1.2 only)
const long wxsSFGTK20  = 0x00000040;    ///< This style can be used in GTK (1.2 only)
const long wxsSFOSX    = 0x00000080;    ///< This style can be used in MAC (any OSX port Cocoa or Carbon)
const long wxsSFCOCOA  = 0x00000100;    ///< This style can be used in MAC (OSX port Cocoa API)
const long wxsSFCARBON = 0x00000200;    ///< This style can be used in MAC (MacOS for Carbon CFM)
const long wxsSFMotif  = 0x00000400;    ///< This style can be used in Motif
const long wxsSFMGL    = 0x00000800;    ///< This style can be used in MGL
const long wxsSFOS2    = 0x00001000;    ///< This style can be used in OS2
const long wxsSFX11    = 0x00002000;    ///< This style can be used in X11
const long wxsSFPALMOS = 0x00004000;    ///< This style can be used in PALMOS
const long wxsSFUNIV   = 0x00008000;    ///< This style can be used in wxUNIV
const long wxsSFAll    = 0x0000FFFF;    ///< This style can be used anywhere
const long wxsSFExt    = 0x80000000;    ///< This is extended style

/** Structure describing one widget's stule */
struct wxsStyle
{
    wxString Name;  ///< Style name
    long Value;     ///< Value of style
    long Flags;     ///< Style flags

    /** Checking if this is style category */
    inline bool IsCategory() const{ return Value == ((long)-1); }

    /** Checking if this is extended style */
    inline bool IsExtra() const { return ( Flags & wxsSFExt ) != 0; }
};

/* ************************************************************************** */
/*  Usefull defines used while creating set of widget's styles                */
/* ************************************************************************** */

/** Beginning definition of array (source file) */
#define WXS_ST_BEGIN(name)                      \
    static wxsStyle name[] = {

/** Adding new style into list
 *
 * This style will be set as available on all platforms
 */
#define WXS_ST(name)                            \
    { _T(#name), name, wxsSFAll & (~wxsSFExt) },

/** Adding new extended style into list
 *
 * This style will be set as available on all platforms
 */
#define WXS_EXST(name) \
    { _T(#name), name, wxsSFAll | wxsSFExt },

/** Adding new style with platform masks
 *
 * \param name - style's name
 * \param Include - flags with included platforms (use wxsSFAll if excluding only)
 * \param Exclude - flags with excluded platforms (use 0 when including only)
 * \param InXRC - true if this style can be used when resource uses XRC files
 */
#define WXS_ST_MASK(name,Include,Exclude,InXRC)      \
    { _T(#name), name,                               \
        ((Include) & (~(Exclude)) & (~(wxsSFExt|wxsSFXRC))) | \
        ((InXRC) ? wxsSFXRC : 0 ) },

/** Adding new extended style with platform masks
 *
 * \param name - style's name
 * \param Include - flags with included platforms (use wxsSFAll if excluding only)
 * \param Exclude - flags with excluded platforms (use 0 when including only)
 * \param InXRC - true if this style can be used when resource uses XRC files
 */
#define WXS_EXST_MASK(name,Include,Exclude,InXRC)    \
    { _T(#name), name,                               \
        ((Include) & (~(Exclude)) & (~wxsSFXRC)) |   \
        ((InXRC) ? wxsSFXRC : 0 ) | wxsSFExt },

/** Beginning new  category */
#define WXS_ST_CATEGORY(name)                   \
    { _T(name), ((long)-1) },

/** Ending creation of list */
#define WXS_ST_END()                            \
    { _T(""), 0 } };                            \

/** adding all default window's style */
#define WXS_ST_DEFAULTS()                       \
    WXS_ST_CATEGORY("wxWindow")                 \
    WXS_ST(wxSIMPLE_BORDER)                     \
    WXS_ST(wxDOUBLE_BORDER)                     \
    WXS_ST(wxSUNKEN_BORDER)                     \
    WXS_ST(wxRAISED_BORDER)                     \
    WXS_ST(wxSTATIC_BORDER)                     \
    WXS_ST(wxNO_BORDER)                         \
    WXS_ST(wxTRANSPARENT_WINDOW)                \
    WXS_ST(wxTAB_TRAVERSAL)                     \
    WXS_ST(wxWANTS_CHARS)                       \
    WXS_ST(wxNO_FULL_REPAINT_ON_RESIZE)         \
    WXS_ST(wxVSCROLL)                           \
    WXS_ST(wxHSCROLL)                           \
    WXS_ST(wxALWAYS_SHOW_SB)                    \
    WXS_ST(wxCLIP_CHILDREN)                     \
    WXS_ST(wxFULL_REPAINT_ON_RESIZE)

#endif
