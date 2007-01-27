#ifndef WXSITEMINFO_H
#define WXSITEMINFO_H

#include <wx/string.h>
#include <wx/bitmap.h>

/** \brief Set of possible item types */
enum wxsItemType
{
    wxsTInvalid = 0,     ///< \brief Invalid item type
    wxsTWidget,          ///< \brief Widget (childless)
    wxsTContainer,       ///< \brief Widget with children
    wxsTSizer,           ///< \brief Sizer
    wxsTSpacer,          ///< \brief Spacer
    wxsTTool             ///< \brief Tool
};

/** \brief Structure containing all global informations about item */
struct wxsItemInfo
{
    wxString        ClassName;      ///< \brief Item's class name
    wxsItemType     Type;           ///< \brief Item type
    wxString        License;        ///< \brief Item's license
    wxString        Author;         ///< \brief Item's author
    wxString        Email;          ///< \brief Item's author's email
    wxString        Site;           ///< \brief Site about this item
    wxString        Category;       ///< \brief Item's category (used for grouping widgets, use _T() instead of _() because this string will be used for sorting and it will be translated manually)
    long            Priority;       ///< \brief Priority used for sorting widgets inside one group, should be in range 0..100, higher priority widgets will be at the beginning of palette
    wxString        DefaultVarName; ///< \brief Prefix for default variable name (converted to uppercase will be used as prefix for identifier)
    long            Languages;      ///< \brief Coding languages used by this item
    unsigned short  VerHi;          ///< \brief Lower number of version
    unsigned short  VerLo;          ///< \brief Higher number of version
    wxBitmap        Icon32;         ///< \brief Item's icon (32x32 pixels)
    wxBitmap        Icon16;         ///< \brief Item's icon (16x16 pixels)
    bool            AllowInXRC;     ///< \brief Item can be used in XRC files
    int             TreeIconId;     ///< \brief Identifier of image inside resource tree
};

#endif
