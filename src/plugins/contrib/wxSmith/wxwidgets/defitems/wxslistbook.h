#ifndef WXSLISTBOOK_H
#define WXSLISTBOOK_H

#include "../wxscontainer.h"

/** \brief Listbook container */
class wxsListbook : public wxsContainer
{
	public:

		wxsListbook(wxsItemResData* Data);

    private:

        virtual void OnEnumContainerProperties(long Flags);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);
        virtual wxsPropertyContainer* OnBuildExtra();
        virtual wxString OnXmlGetExtraObjectClass();
        virtual void OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long PreviewFlags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnMouseClick(wxWindow* Preview,int PosX,int PosY);
        virtual bool OnIsChildPreviewVisible(wxsItem* Child);
        virtual bool OnEnsureChildPreviewVisible(wxsItem* Child);

        void UpdateCurrentSelection();

        wxsItem* m_CurrentSelection;
};

#endif
