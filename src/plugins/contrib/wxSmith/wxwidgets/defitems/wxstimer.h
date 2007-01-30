#ifndef WXSTIMER_H
#define WXSTIMER_H

#include "../wxstool.h"

class wxsTimer: public wxsTool
{
    public:

        wxsTimer(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return false; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage);

        long m_Interval;
        bool m_OneShoot;
};

#endif
