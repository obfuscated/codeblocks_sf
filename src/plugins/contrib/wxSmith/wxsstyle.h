#ifndef __WXSSTYLE_H
#define __WXSSTYLE_H

/* ************************************************************************** */
/*  Available style flags                                                     */
/* ************************************************************************** */

const unsigned int wxsSFXRC    = 0x00000001;    ///< This style can be used in XRC mode
const unsigned int wxsSFWin    = 0x00000002;    ///< This style can be used in Windows (any)
const unsigned int wxsSFWin95  = 0x00000004;    ///< This style can be used in Win95
const unsigned int wxsSFWinCE  = 0x00000008;    ///< This style can be used in WinCE
const unsigned int wxsSFGTK    = 0x00000010;    ///< This style can be used in GTK
const unsigned int wxsSFMAC    = 0x00000020;    ///< This style can be used in MAC
const unsigned int wxsSFMotif  = 0x00000040;    ///< This style can be used in Motif
const unsigned int wxsSFMGL    = 0x00000080;    ///< This style can be used in MGL
const unsigned int wxsSFOS2    = 0x00000100;    ///< This style can be used in OS2
const unsigned int wxsSFX11    = 0x00000200;    ///< This style can be used in X11
const unsigned int wxsSFAll    = 0x00000FFF;    ///< This style can be used anywhere
const unsigned int wxsSFExt    = 0x80000000;    ///< This is extended style

/** Structure describing one widget's stule */
struct wxsStyle
{
    wxString Name;          ///< Style name
    unsigned int Value;     ///< Value of style
    unsigned int Flags;     ///< Style flags

    /** Checking if this is style category */
    inline bool IsCategory() { return Value == ((unsigned int)-1); }

    /** Checking if this is extended style */
    inline bool IsExtra() { return ( Flags & wxsSFExt ) != 0; }
};

/* ************************************************************************** */
/*  Usefull defines used while creating set of widget's styles                */
/* ************************************************************************** */

/** Declaring list in header file */
#define WXS_ST_DECLARE(name)                      \
    extern wxsStyle* name;

/** Beginning definition of array (source file) */
#define WXS_ST_BEGIN(name)                      \
    static wxsStyle __##name##__[] = {

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
    { _T(name), ((unsigned int)-1) },

/** Ending creation of list */
#define WXS_ST_END(name)                        \
    { _T(""), 0 } };                            \
    wxsStyle* name = __##name##__;

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
