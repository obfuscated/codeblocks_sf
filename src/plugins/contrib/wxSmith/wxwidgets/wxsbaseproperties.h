#ifndef WXSBASEPROPERTIES_H
#define WXSBASEPROPERTIES_H

#include "wxsexproperties.h"
#include "../wxsadvqpp.h"
#include "../wxscodinglang.h"

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
        wxsSizeData m_MinSize;            ///< \brief Mininum size (not available in XRC)
        wxsSizeData m_MaxSize;            ///< \brief Maximum size (not available in XRC)
        bool m_Enabled;                   ///< \brief If false, widget is disabled (true by deefault)
        bool m_Focused;                   ///< \brief If true, widget is focused (false by default)
        bool m_Hidden;                    ///< \brief If true, widget is hidden (false by default)
        wxsColourData m_Fg;               ///< \brief Foreground colour when using custom colour
        wxsColourData m_Bg;               ///< \brief Background colour when using custom colour
        wxsFontData m_Font;               ///< \brief Font
        wxString m_ToolTip;               ///< \brief Tooltip
        wxString m_HelpText;              ///< \brief Help text
        wxString m_ExtraCode;             ///< \brief User's extra item initialization code



        /** \brief Constructor - it will be used to set default values */
        wxsBaseProperties():
            m_Enabled(true),
            m_Focused(false),
            m_Hidden(false),
            m_Fg(wxsCOLOUR_DEFAULT),
            m_Bg(wxsCOLOUR_DEFAULT),
            m_ToolTip(wxEmptyString),
            m_HelpText(wxEmptyString),
            m_ExtraCode(wxEmptyString)
        {}

        /** \brief Function generating QPPChild Panel for most frequently used
         *         properties.
         */
        void AddQPPChild(wxsAdvQPP* QPP,long Flags);

    protected:

        virtual void OnEnumProperties(long Flags);
};

#endif
