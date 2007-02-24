#ifndef WXSTOOLBAR_H
#define WXSTOOLBAR_H

#include "../wxstool.h"

/** \brief Class represening wxToolBar */
class wxsToolBar: public wxsTool
{
    public:

        wxsToolBar(wxsItemResData* Data);

    protected:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return true; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);
        virtual bool OnMouseDClick(wxWindow* Preview,int PosX,int PosY);
        virtual bool OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        wxsSizeData m_BitmapSize;
        wxsSizeData m_Margins;
        long        m_Packing;
        long        m_Separation;

        friend class wxsToolBarEditor;
};



#endif
