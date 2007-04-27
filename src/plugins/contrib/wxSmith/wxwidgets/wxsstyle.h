#ifndef __WXSSTYLE_H
#define __WXSSTYLE_H

#ifndef WX_PRECOMP
    #include <wx/arrstr.h>
#endif

#include "../wxscodinglang.h"
#include <wx/dynarray.h>

// TODO: Think about non-macro implementation

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

/** \brief Class managing giving set of styles
 *
 * This class is managing style sets, one per one set, NOT per instance of
 * item. It means that item must have extra long variables to remember style
 * settings.
 */
class wxsStyleSet
{
    public:

        /** \brief Ctor, takes array of styles
         * \param Default default style
         */
        wxsStyleSet(const wxChar* DefaultStyle=_T(""));

        /** \brief Adding new style to array */
        void AddStyle(const wxChar* Name,long Value,long Flags);

        /** \brief Notifying the end of style declaration */
        void EndStyle();

        /** \brief Dctor */
        ~wxsStyleSet();

        /** \brief Getting names array */
        inline const wxArrayString& GetNames(bool IsExtra) const { return IsExtra?ExStyleNames:StyleNames; }

        /** \brief Getting style bits array (each style has unique bit) */
        inline const wxArrayLong& GetBits(bool IsExtra) const { return IsExtra?ExStyleBits:StyleBits; }

        /** \brief Getting values array (real values used inside wxWidgets) */
        inline const wxArrayLong& GetValues(bool IsExtra) const { return IsExtra?ExStyleValues:StyleValues; }

        /** \brief Getting style flags array */
        inline const wxArrayLong& GetFlags(bool IsExtra) const { return IsExtra?ExStyleFlags:StyleFlags; }

        /** \brief Getting default style bits */
        inline long GetDefaultBits(bool IsExtra) const { return IsExtra?0:Default; }

        /** \brief Generating Bitfield from given string (where styles are separated through '|') */
        long GetBits(const wxString& Style,bool IsExtra) const;

        /** \brief Converting given style set bitfield to wxString usning given language (note that CPP is same format like the one used in XRC files) */
        wxString GetString(long Bits,bool IsExtra,wxsCodingLang Language) const;

        /** \brief Converting style bits to value which can be used in wxWidgets */
        long GetWxStyle(long StyleBits,bool IsExtra=false) const;

    private:

        const wxChar* DefaultStr;
        long Default;
        wxArrayString StyleNames;
        wxArrayLong StyleBits;
        wxArrayLong StyleValues;
        wxArrayLong StyleFlags;

        wxArrayString ExStyleNames;
        wxArrayLong ExStyleBits;
        wxArrayLong ExStyleValues;
        wxArrayLong ExStyleFlags;
};

/* ************************************************************************** */
/*  Usefull defines used while creating set of widget's styles                */
/* ************************************************************************** */

/** Beginning definition of array (source file) */
#define WXS_ST_BEGIN(name,DefaultStyle)                             \
    const wxsStyleSet* Get##name##StyleSet();                       \
    static const wxsStyleSet* name = Get##name##StyleSet();         \
    const wxsStyleSet* Get##name##StyleSet()                        \
    {                                                               \
        static wxsStyleSet Set(DefaultStyle);                       \

/** Adding new style into list
 *
 * This style will be set as available on all platforms
 */
#define WXS_ST(name)                                                \
        Set.AddStyle(_T(#name), name, wxsSFAll & ~wxsSFExt);

/** Adding new extended style into list
 *
 * This style will be set as available on all platforms
 */
#define WXS_EXST(name)                                              \
        Set.AddStyle(_T(#name), name, wxsSFAll | wxsSFExt);

/** Adding new style with platform masks
 *
 * \param name - style's name
 * \param Include - flags with included platforms (use wxsSFAll if excluding only)
 * \param Exclude - flags with excluded platforms (use 0 when including only)
 * \param InXRC - true if this style can be used when resource uses XRC files
 */
#define WXS_ST_MASK(name,Include,Exclude,InXRC)                     \
        Set.AddStyle(_T(#name), name,                               \
            ((Include) & (~(Exclude)) & (~(wxsSFExt|wxsSFXRC))) |   \
            ((InXRC) ? wxsSFXRC : 0 ) );

/** Adding new extended style with platform masks
 *
 * \param name - style's name
 * \param Include - flags with included platforms (use wxsSFAll if excluding only)
 * \param Exclude - flags with excluded platforms (use 0 when including only)
 * \param InXRC - true if this style can be used when resource uses XRC files
 */
#define WXS_EXST_MASK(name,Include,Exclude,InXRC)                   \
        Set.AddStyle(_T(#name), name,                               \
            ((Include) & (~(Exclude)) & (~wxsSFXRC)) |              \
            ((InXRC) ? wxsSFXRC : 0 ) | wxsSFExt );

/** Beginning new  category */
#define WXS_ST_CATEGORY(name)                                       \
        Set.AddStyle(_T(#name),-1, 0);

/** Ending creation of list */
#define WXS_ST_END()                                                \
        Set.EndStyle();                                             \
        return &Set;                                                \
    }

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
