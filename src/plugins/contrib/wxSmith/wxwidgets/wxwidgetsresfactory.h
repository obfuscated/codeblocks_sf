#ifndef WXWIDGETSRESFACTORY_H
#define WXWIDGETSRESFACTORY_H

#include "../wxsresourcefactory.h"

/** \brief Factory supporting all basic wxWidgets resoruce types */
class wxWidgetsResFactory : public wxsResourceFactory
{
    public:
        wxWidgetsResFactory();
    private:
        virtual void OnAttach();
        virtual void OnRelease();
        virtual int OnGetCount();
        virtual void OnGetInfo(int Number,wxString& Name,wxString& GUI);
        virtual wxsResource* OnCreate(int Number,wxsProject* Project);
        virtual bool OnCanHandleExternal(const wxString& FileName);
        virtual wxsResource* OnBuildExternal(const wxString& FileName);
        virtual bool OnNewWizard(int Number,wxsProject* Project);
        virtual int OnResourceTreeIcon(int Number);
};


#endif
