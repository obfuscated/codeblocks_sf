#ifndef WXSITEMINFO_H
#define WXSITEMINFO_H

/** \brief Set of possible item types */
enum wxsItemType
{
    wxsTInvalid = 0,     ///< Invalid item type
    wxsTWidget,          ///< Widget (childless)
    wxsTContainer,       ///< Widget with children
    wxsTSizer,           ///< Sizer
    wxsTSpacer,          ///< Spacer
    wxsTTool             ///< Tool
};

/** \brief Structure containing all global informations about item */
struct wxsItemInfo
{
    wxString        Name;           ///< Item name (equivalent to it's class)
    wxsItemType     Type;           ///< Item type
    wxString        License;        ///< Item's license
    wxString        Author;         ///< Item's author
    wxString        Email;          ///< Item's author's email
    wxString        Site;           ///< Site about this item
    wxString        Category;       ///< Item's category (used for grouping widgets, use _T() instead of _() because this string will be used for sorting and it will be translated manually)
    long            Priority;       ///< Priority used for sorting widgets inside one group, should be in range 0..100, higher priority widgets will be at the beginning of palette
    wxString        DefaultVarName; ///< Prefix for default variable name (converted to uppercase will be used as prefix for identifier)
    unsigned short  VerHi;          ///< Lower number of version
    unsigned short  VerLo;          ///< Higher number of version
    wxBitmap*       Icon32;         ///< Item's icon (32x32 pixels)
    wxBitmap*       Icon16;         ///< Item's icon (16x16 pixels)
    int             TreeIconId;     ///< Identifier of image inside resource tree
};

#endif
