#ifndef WXSCUSTOMWIDGET_H
#define WXSCUSTOMWIDGET_H

#include "../wxswidget.h"

/** \brief Class for custom widgets
 *
 * Using custom widget class, User can add it's own widgets here.
 * Because widgets in XRC must be threated differently from the ones
 * created using source code, XRC will use additional xml configuration,
 * source code will must have creating code defined.
 */
class wxsCustomWidget: public wxsWidget
{
    public:
        wxsCustomWidget(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return true; }
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        void RebuildXmlData();
        bool RebuildXmlDataDoc();

        wxString m_CreatingCode;
        wxString m_Style;
        wxString m_XmlData;
        TiXmlDocument m_XmlDataDoc;

};

#endif
