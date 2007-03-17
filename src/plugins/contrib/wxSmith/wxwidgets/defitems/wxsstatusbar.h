#ifndef WXSSTATUSBAR_H
#define WXSSTATUSBAR_H

#include "../wxstool.h"

class wxsStatusBar: public wxsTool
{
    public:

        wxsStatusBar(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return true; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage);
        virtual bool OnCanAddToParent(wxsParent* Parent,bool ShowMessage);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);
        virtual void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        virtual void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id);
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        void UpdateArraysSize(int Size);

        WX_DEFINE_ARRAY(wxPGId,IdsArray);

        int         m_Fields;
        wxArrayInt  m_Widths;
        wxArrayInt  m_Styles;
        wxArrayBool m_VarWidth;

        wxPGId     m_FieldsId;
        IdsArray   m_ParentIds;
        IdsArray   m_WidthsIds;
        IdsArray   m_StylesIds;
        IdsArray   m_VarWidthIds;
};



#endif
