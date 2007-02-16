#ifndef WXSWIDGET_H
#define WXSWIDGET_H

#include "wxsitem.h"
#include "wxsbaseproperties.h"
#include "wxsstyle.h"

/** \brief Class used as a base class for standard widgets without any children
 *
 * Few steps to create new widgets:
 *  \li Create widget info structure
 *  \li Create new class deriving from wxsWidget
 *  \li Create set of styles using macros defined in wxsstyle.h
 *  \li Create set of events using macros defined in wxsevent.h
 *  \li Override EnumWidgetProperties function
 *  \li Override BuildCreatingCode function (it's declared in wxsItem)
 *  \li Override BuildPreview function (also declared in wxsItem)
 *
 */
class wxsWidget: public wxsItem
{
    public:

        /** \brief Default properties flags used by widgets */
        static const long flWidget = flVariable|flId|flPosition|flSize|flEnabled|flFocused|flHidden|flColours|flToolTip|flFont|flHelpText|flSubclass;

        /** \brief Ctor
         *  \param Data data managment object handling this item
         *  \param PropertiesFlags flags filtering sed base properties
         *         (see wxsBaseProperties for details)
         *  \param Info pointer to static widget info
         *  \param EventArray pointer to static set of events
         *  \param StyleSet set of used styles, if NULL, this widget won't
         *         provide styles by default
         */
        wxsWidget(
            wxsItemResData* Data,
            const wxsItemInfo* Info,
            const wxsEventDesc* EventArray = NULL,
            const wxsStyleSet* StyleSet=NULL,
            long PropertiesFlags = flWidget);

    protected:

        /** \brief Function enumerating properties for this widget only
         *
         * This function should enumerate all extra properties
         * required by item (extra means not enumerated in base properties
         * not id or variable name).
         * These properties will be placed at the beginning, right after
         * there will be Variable name and identifier and at the end, all
         * required base properties.
         */
        virtual void OnEnumWidgetProperties(long Flags) = 0;

        /** \brief Function which adds new items to QPP
         *
         * This function may be used to add special quick properties for
         * this item.
         *
         * All QPPChild panels will be added before additional panels
         * added by widget.
         */
        virtual void OnAddWidgetQPP(wxsAdvQPP* QPP) { }

        /** \brief Easy access to position */
        inline wxPoint Pos(wxWindow* Parent)
        {
            return GetBaseProps()->m_Position.GetPosition(Parent);
        }

        /** \brief Easy access to size */
        inline wxSize Size(wxWindow* Parent)
        {
            return GetBaseProps()->m_Size.GetSize(Parent);
        }

        /** \brief Easy access to style (can be used directly in wxWidgets */
        inline long Style()
        {
            return m_StyleSet ? m_StyleSet->GetWxStyle(m_StyleBits,false) : 0;
        }

        /** \brief Function setting up standard widget properties after
         *         the window is created.
         *
         * Included properties:
         *  - Enabled
         *  - Focused
         *  - Hidden (skipped when not exact preview)
         *  - FG - Foreground colour
         *  - BG - Background colour
         *  - Font
         *  - ToolTip
         *  - HelpText
         *  - Extra style
         *
         * \param Preview window for which properties must be applied
         * \param Flags flags passed to OnBuildPreview
         * \return window after settig up properties (value of Preview is returned)
         */
        wxWindow* SetupWindow(wxWindow* Preview,long Flags);

        /** \brief Easy acces to position code */
        inline wxString PosCode(const wxString& Parent,wxsCodingLang Language)
        {
            return GetBaseProps()->m_Position.GetPositionCode(Parent,Language);
        }

        /** \brief Easy acces to size code */
        inline wxString SizeCode(const wxString& Parent,wxsCodingLang Language)
        {
            return GetBaseProps()->m_Size.GetSizeCode(Parent,Language);
        }

        /** \brief Easy access to style code */
        inline wxString StyleCode(wxsCodingLang Language)
        {
            return m_StyleSet ? m_StyleSet->GetString(m_StyleBits,false,Language) : _T("0");
        }

        /** \brief Function adding code setting up properties after window
         *         is created.
         *
         * Behaviour of this function is simillar to SetupWindow but this
         * one creates source code doing it instead of setting up real window
         */
        void SetupWindowCode(wxString& Code,wxsCodingLang Language);

        /** \brief Extensions to Codef function */
        virtual bool OnCodefExtension(wxsCodingLang Language,wxString& Result,const wxChar* &FmtChar,va_list ap);

    private:

        /** \brief Function enumerating properties with default ones
         *
         * Function enumerating item properties. The implementation
         * does call EnumContainerProperties() and adds all default properties.
         */
        virtual void OnEnumItemProperties(long Flags);

        /** \brief Function Adding QPPChild panels for base properties of this
         *         container.
         *
         * This function calls internally AddContainerQPP to add any additional
         * QPPChild panels.
         */
        virtual void OnAddItemQPP(wxsAdvQPP* QPP);

        const wxsStyleSet* m_StyleSet;
        wxString m_DefaultStyle;
        long m_StyleBits;
        long m_ExStyleBits;

};

#endif
