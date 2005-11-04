#ifndef WXSBASEPROPERTIES_H
#define WXSBASEPROPERTIES_H

#include <wx/string.h>
#include "wxsglobals.h"

/** This struct holds almost all comonly used properties */
struct wxsBaseProperties
{
    wxString IdName;                ///< Widget's identifier
    wxString VarName;               ///< Widget's variable used inside main window
    bool IsMember;                  ///< True if widget's variable won't be stored inside main window
    int PosX, PosY;                 ///< Widget's position
    bool DefaultPosition;           ///< Widget has default position
    int SizeX, SizeY;               ///< Widget's size
    bool DefaultSize;               ///< Widget has default size
    int Style;                      ///< Current style

    bool Enabled;                   ///< If false, widget is disabled (true by deefault)
    bool Focused;                   ///< If true, widget is focused (false by default)
    bool Hidden;                    ///< If true, widget is hidden (false by default)

    wxUint32 FgType;                ///< Type of Fg colour (wxSYS_COLOUR_XXX or wxsCUSTOM_COLOUR or wxsNO_COLOUR)
    wxColour Fg;                    ///< Foreground colour when using custom colour
    wxUint32 BgType;                ///< Type of Bg colour (wxSYS_COLOUR_XXX or wxsCUSTOM_COLOUR or wxsNO_COLOUR)
    wxColour Bg;                    ///< Background colour when using custom colour

    bool UseFont;                   ///< Must be true to allow using font
    wxFont Font;                    ///< Font

    wxString ToolTip;               ///< Tooltip


    /** Constructor - it will be used to set default values */
    wxsBaseProperties():
        IdName(_T("")),
        VarName(_T("")),
        IsMember(true),
        PosX(-1), PosY(-1),
        DefaultPosition(true),
        SizeX(-1), SizeY(-1),
        DefaultSize(true),
        Style(0),
        Enabled(true),
        Focused(false),
        Hidden(false),
        FgType(wxsNO_COLOUR),
        Fg(0,0,0),
        BgType(wxsNO_COLOUR),
        Bg(0,0,0),
        UseFont(false),
        Font(wxNullFont),
        ToolTip(_T(""))
    {
    }
};

/** This type will be used as bitfield to select used properties */
typedef unsigned long wxsBasePropertiesType;

const wxsBasePropertiesType bptPosition  = 0x0001;  ///< Item is using position
const wxsBasePropertiesType bptSize      = 0x0002;  ///< Item is using size
const wxsBasePropertiesType bptId        = 0x0004;  ///< Item is using identifier
const wxsBasePropertiesType bptVariable  = 0x0008;  ///< Item is using variable
const wxsBasePropertiesType bptStyle     = 0x0010;  ///< Item is using style
const wxsBasePropertiesType bptEnabled   = 0x0020;  ///< Item is using Enabled property
const wxsBasePropertiesType bptFocused   = 0x0040;  ///< Item is using Focused property
const wxsBasePropertiesType bptHidden    = 0x0080;  ///< Item is using Hidden property
const wxsBasePropertiesType bptColours   = 0x0100;  ///< Item is using colour properties (Fg and Bg)
const wxsBasePropertiesType bptToolTip   = 0x0200;  ///< Item is using tooltips
const wxsBasePropertiesType bptFont      = 0x0400;  ///< Item is using font

/** BasePropertiesType with no default properties */
const wxsBasePropertiesType propNone     = 0;

/** VasePropertiesTyue usede by common windows */
const wxsBasePropertiesType propWindow   = bptStyle | bptColours | bptToolTip | bptFont;

/** wxsBasePropertiesType used by common widgets in Source edit mode */
const wxsBasePropertiesType propWidgetS  = bptPosition | bptSize | bptId | bptVariable | bptStyle | bptEnabled | bptFocused | bptHidden | bptColours | bptToolTip | bptFont;

/** wxsBasePropertiesType used by common widgets in File edit mode */
const wxsBasePropertiesType propWidgetF  = bptPosition | bptSize | bptId | bptStyle | bptEnabled | bptFocused | bptHidden | bptColours | bptToolTip | bptFont;

/** wxsBasePropertiesType used by common widgets in Mixed edit mode */
const wxsBasePropertiesType propWidgetM  = propWidgetS;

/** wxsBasePropertiesType used by common sizers in Source edit mode */
const wxsBasePropertiesType propSizerS   = bptVariable;

/** wxsBasePropertiesType used by common sizers when in Mixed edit mode*/
const wxsBasePropertiesType propSizerM   = 0;

/** wxsBasePropertiesType used by common sizers when in File edit mode*/
const wxsBasePropertiesType propSizerF   = 0;

/** wxsBasePropertiesType used by spacer */
const wxsBasePropertiesType propSpacer   = bptSize;

#endif // WXSBASEPROPERTIES_H
