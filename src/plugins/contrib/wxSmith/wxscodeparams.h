#ifndef WXSCODEPARAMS_H
#define WXSCODEPARAMS_H

/** Structure containing all data needed while generating code */
struct wxsCodeParams
{
    wxString VarName;               ///< Name of varaible holding this widget
    wxString IdName;                ///< Idedntifier for this item
    wxString Style;                 ///< Widget's style in form 'wxSTYLE1|wxSTYLE2' or 0 if no style is used
    wxString Pos;                   ///< Widget's position (usually in form "wxPoint(x,y)" but may be "wxDefaultPosition")
    wxString Size;                  ///< Widget's size (usually in form "wxSize(x,y)" but may be "wxDefaultSize")
    wxString InitCode;              ///< Code initializing Enabled / Focused / Hidden flags, Colours, ToolTip and Font, should be added at the end of standard initialization
    wxString ParentName;            ///< Name of parent widget (the one which may be passed to widget constructor)
    wxString Name;                  ///< Name (used as name in constructor)

    bool IsDirectParent;            ///< False when parent item in resource tree is sizer or something that's not real widget
    int UniqueNumber;               ///< Unique number, it's granted that it will be different for any resource item (in one resource)
};

#endif
