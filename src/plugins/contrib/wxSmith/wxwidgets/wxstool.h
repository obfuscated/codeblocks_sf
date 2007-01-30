#ifndef WXSTOOL_H
#define WXSTOOL_H

#include "wxsitem.h"

/** \brief Base class for wxWidgets tools like wxTimer */
class wxsTool: public wxsItem
{
    public:

        /** \brief Default properties flags used by tools */
        static const long flTool = flVariable;

        /** \brief Ctor */
        wxsTool(
            wxsItemResData* Data,
            const wxsItemInfo* Info,
            const wxsEventDesc* EventArray = NULL,
            const wxsStyleSet* StyleSet=NULL,
            long PropertiesFlags = flTool);

        /** \brief Function checking if this tool can be added to this resource
         *  \note This function is only a wrapper to OnCanAddToResource function
         */
        inline bool CanAddToResource(wxsItemResData* Data,bool ShowMessage=false) { return OnCanAddToResource(Data,ShowMessage); }

    protected:

        /** \brief Enumerating properties specific to given tool
         *
         * This function should enumerate all extra properties
         * required by tool (extra means not enumerated in base properties
         * nor id nor variable name).
         * These properties will be placed at the beginning, right after
         * there will be Variable name and identifier and at the end, all
         * required base properties.
         */
        virtual void OnEnumToolProperties(long Flags) = 0;

        /** \brief Function which adding new items to QPP
         *
         * This function may be used to add special quick properties for
         * this item.
         *
         * All QPPChild panels will be added before additional panels
         * generic for tools.
         */
        virtual void OnAddToolQPP(wxsAdvQPP* QPP) { }

        /** \brief Easy access to style (can be used directly in wxWidgets */
        inline long Style()
        {
            return m_StyleSet ? m_StyleSet->GetWxStyle(m_StyleBits,false) : 0;
        }

        /** \brief Easy access to style code */
        inline wxString StyleCode(wxsCodingLang Language)
        {
            return m_StyleSet ? m_StyleSet->GetString(m_StyleBits,false,Language) : _T("0");
        }

        /** \brief Checking if this tool can be added to given resource
         *
         * One can override this function to limit tool's availability
         * \param Data data of checked resource
         * \param ShowMessage if true, this function should show message box
         *        notifying about problem when can not add this tool to resource
         */
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage) { return true; }

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

        /** \brief Function converting item to tool class */
        virtual wxsTool* ConvertToTool() { return this; }

        /** \brief Tools don't generate preview */
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags) { return NULL; }

        const wxsStyleSet* m_StyleSet;
        wxString m_DefaultStyle;
        long m_StyleBits;
        long m_ExStyleBits;
};



#endif
