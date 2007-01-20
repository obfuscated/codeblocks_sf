#ifndef WXSPLITTERWINDOW_H
#define WXSPLITTERWINDOW_H

#include "../wxscontainer.h"

class wxsSplitterWindow: public wxsContainer
{
	public:

		wxsSplitterWindow(wxsItemResData* Data);

    private:

        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumContainerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);

        long SashPos;
        long MinSize;
        long Orientation;
};



#endif
