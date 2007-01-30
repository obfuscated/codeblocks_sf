#ifndef WXSBASEPROPERTIES_H
#define WXSBASEPROPERTIES_H

#include "wxsexproperties.h"
#include "../wxsadvqpp.h"
#include "../wxscodinglang.h"

// TODO: Use flags also wneh creating source code / preview - this will
//       prevent using properties which are not used by item

/** \brief Structure holding most commonly used properties
 *
 * This class doesn't support style since it require too many informations
 * from base class.
 *
 * \note All availability flags used inside this class are declared in wxsItem
 */
class wxsBaseProperties: public wxsPropertyContainer
{
    public:


        wxsPositionData m_Position;       ///< \brief Position
        wxsSizeData m_Size;               ///< \brief Size
        bool m_Enabled;                   ///< \brief If false, widget is disabled (true by deefault)
        bool m_Focused;                   ///< \brief If true, widget is focused (false by default)
        bool m_Hidden;                    ///< \brief If true, widget is hidden (false by default)
        wxsColourData m_Fg;               ///< \brief Foreground colour when using custom colour
        wxsColourData m_Bg;               ///< \brief Background colour when using custom colour
        wxsFontData m_Font;               ///< \brief Font
        wxString m_ToolTip;               ///< \brief Tooltip
        wxString m_HelpText;              ///< \brief Help text

        /** \brief Constructor - it will be used to set default values */
        wxsBaseProperties():
            m_Enabled(true),
            m_Focused(false),
            m_Hidden(false),
            m_Fg(wxsCOLOUR_DEFAULT),
            m_Bg(wxsCOLOUR_DEFAULT),
            m_ToolTip(wxEmptyString),
            m_HelpText(wxEmptyString)
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
        void SetupWindow(wxWindow* Window,long Flags);

        /** \brief Function generating code setting up some properties on given window
         *
         * Created code will do the same as SetupWindow function
         */
        void BuildSetupWindowCode(wxString& Code,const wxString& WindowName,const wxString& AccessPrefix,wxsCodingLang Language);

        /** \brief Function generating QPPChild Panel for most frequently used
         *         properties.
         */
        void AddQPPChild(wxsAdvQPP* QPP,long Flags);

    protected:

        virtual void OnEnumProperties(long Flags);
};

#endif
