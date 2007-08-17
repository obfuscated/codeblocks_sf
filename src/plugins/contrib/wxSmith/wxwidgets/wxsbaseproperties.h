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

        bool m_ParentFromArg;             ///< \brief Use parent from constructor's argument
        wxString m_VarName;               ///< \brief Variable name
        bool m_IsMember;                  ///< \brief Swith between local and global variable
        wxString m_IdName;                ///< \brief Name of identifier
        bool m_IdFromArg;                 ///< \brief Set to true if we should consider taking id from constructor's argument
        wxString m_Subclass;              ///< \brief Subclass (class used instead of original base class)
        wxsPositionData m_Position;       ///< \brief Data for position in case of custom value
        bool m_PositionFromArg;           ///< \brief If set to true, we should consider taking position from constructor's argument
        wxsSizeData m_Size;               ///< \brief Data for size in case of custom value
        bool m_SizeFromArg;               ///< \brief If set to true, we should consider taking position from constructor's argument
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
            m_ParentFromArg(true),
            m_IsMember(true),
            m_IdFromArg(false),
            m_PositionFromArg(false),
            m_SizeFromArg(false),
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

        /** \brief Adding some special xml read of data */
        void SpecialXmlRead(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Adding some special xml write of data */
        void SpecialXmlWrite(TiXmlElement* Elem,bool IsXRC,bool IsExtra,long Flags,const wxString& ClassName);

    protected:

        virtual void OnEnumProperties(long Flags);
};

#endif
