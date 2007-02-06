#ifndef WXSMENUBAR_H
#define WXSMENUBAR_H

#include "../wxstool.h"

/** \brief Class representing menu bar in wxFrame resources */
class wxsMenuBar: public wxsTool
{
    public:

        wxsMenuBar(wxsItemResData* Data);

    protected:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return true; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage);
        void ShowMenuEditor();
};



#endif
