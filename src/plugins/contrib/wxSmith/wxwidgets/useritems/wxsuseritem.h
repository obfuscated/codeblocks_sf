#ifndef WXSUSERITEM_H
#define WXSUSERITEM_H

#include "../wxswidget.h"

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

        wxsUserItem(
            wxsItemResData* Data,
            wxsItemInfo* Info
            );

        virtual ~wxsUserItem();

    private:

        class PropertyItem
        {
            wxString m_Name;

        };

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString ExpandCodeVariable(const wxString& VarName);


        wxBitmap m_PreviewBitmap;       ///< \brief Bitmap used for preview
        wxSize   m_DefaultSize;         ///< \brief default size set when there's no size given
        wxString m_CppCodeTemplate;     ///< \brief Template for code
};



#endif
