#ifndef WXSUSERITEM_H
#define WXSUSERITEM_H

#include "wxsgenericpropertyvaluelist.h"
#include "../wxswidget.h"

class wxsUserItemDescription;

/** \brief This is base class for simple user -defined items.
 *
 * Currently only widgets can be defined using this approach.
 * Other limitation is that item's content is taken from static
 * bitmap only and thus no dynamic content can be shown inside
 * editor when changing item's properties
 */
class wxsUserItem : public wxsWidget
{
    public:

        /** \brief Ctor
         *  \param Data resource data
         *  \param Info item's info
         *  \param Properties set of properties (it will be deleted in constructor)
         */
        wxsUserItem(
            wxsItemResData* Data,
            wxsUserItemDescription* Description,
            wxsItemInfo* Info,
            wxsGenericPropertyValueList* Properties,
            const wxBitmap& PreviewBitmap,
            const wxSize& DefaultSize
            );

        /** \brief Dctor */
        virtual ~wxsUserItem();

        /** \brief Adding stuff related to one coding language */
        void AddLanguage(wxsCodingLang Language,const wxString& CodeTemplate,const wxArrayString& DeclarationHeaders,const wxArrayString& DefinitionHeaders);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString ExpandCodeVariable(const wxString& VarName);

        wxBitmap        m_PreviewBitmap;            ///< \brief Bitmap used for preview
        wxSize          m_DefaultSize;              ///< \brief default size set when there's no size given
        wxString        m_CppCodeTemplate;          ///< \brief Template for code
        wxArrayString   m_CppDeclarationHeaders;    ///< \brief Header files used in declaration
        wxArrayString   m_CppDefinitionHeaders;     ///< \brief Header files used in definition of resource

        wxsGenericPropertyValueList* m_Properties;  ///< \brief List of properties
        wxsUserItemDescription* m_Description;      ///< \brief Item's description
};



#endif
