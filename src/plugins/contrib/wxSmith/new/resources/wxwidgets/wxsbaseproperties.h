#ifndef WXSBASEPROPERTIES_H
#define WXSBASEPROPERTIES_H

#include "properties/wxsproperties.h"
#include "wxsglobals.h"
#include "wxsadvqpp.h"

/** \brief Structure holding most commonly used properties
 *
 * This class doesn't support style since it require too many informations
 * from base class.
 */
class wxsBaseProperties: public wxsPropertyContainer
{
    public:

        // Availability flags used in this container
        static const long flPosition        = 0x0001;  ///< \brief Item is using position
        static const long flSize            = 0x0002;  ///< \brief Item is using size
        static const long flEnabled         = 0x0004;  ///< \brief Item is using Enabled property
        static const long flFocused         = 0x0008;  ///< \brief Item is using Focused property
        static const long flHidden          = 0x0010;  ///< \brief Item is using Hidden property
        static const long flColours         = 0x0020;  ///< \brief Item is using colour properties (Fg and Bg)
        static const long flToolTip         = 0x0040;  ///< \brief Item is using tooltips
        static const long flFont            = 0x0080;  ///< \brief Item is using font
        static const long flHelpText        = 0x0100;  ///< \brief Item is using help text
        static const long flContainer       = flPosition | flSize | flColours | flToolTip | flHelpText;   ///< \brief Properties used by common containers
        static const long flAll             = 0x01FF;  ///< \brief Using all base properties

        wxsPositionData Position;       ///< \brief Position
        wxsSizeData Size;               ///< \brief Size
        bool Enabled;                   ///< \brief If false, widget is disabled (true by deefault)
        bool Focused;                   ///< \brief If true, widget is focused (false by default)
        bool Hidden;                    ///< \brief If true, widget is hidden (false by default)
        wxColourPropertyValue Fg;       ///< \brief Foreground colour when using custom colour
        wxColourPropertyValue Bg;       ///< \brief Background colour when using custom colour
        wxsFontData Font;               ///< \brief Font
        wxString ToolTip;               ///< \brief Tooltip
        wxString HelpText;              ///< \brief Help text

        /** \brief Constructor - it will be used to set default values */
        wxsBaseProperties():
            Enabled(true),
            Focused(false),
            Hidden(false),
            Fg(wxsCOLOUR_DEFAULT),
            Bg(wxsCOLOUR_DEFAULT),
            ToolTip(wxEmptyString),
            HelpText(wxEmptyString)
        {}

        /** \brief Function setting up created window using some of properties
         *
         * Properties set up are:
         *  - Enabled
         *  - Focused
         *  - Hidden (skipped when not exact preview)
         *  - FG - Foreground colour
         *  - BG - Background colour
         *  - Font
         *  - ToolTip
         *  - HelpText
         */
        void SetupWindow(wxWindow* Window,bool IsExact);

        /** \brief Function generating code setting up some properties on given window
         *
         * Created code will do the same as SetupWindow function
         */
        void BuildSetupWindowCode(wxString& Code,const wxString& WindowName,wxsCodingLang Language);

        /** \brief Function generating QPPChild Panel for most frequently used
         *         properties.
         */
        void AddQPPChild(wxsAdvQPP* QPP,long Flags);

    protected:

        virtual void EnumProperties(long Flags);
};

#endif
