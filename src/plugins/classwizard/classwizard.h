#ifndef CLASSWIZARD_H
#define CLASSWIZARD_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <simpletextlog.h>

class ClassWizard : public cbToolPlugin
{
    public:
        ClassWizard();
        ~ClassWizard();

        virtual void OnAttach();
        virtual void OnRelease();
        virtual int Configure(){ return -1; }
		virtual int Execute();
        virtual void BuildMenu(wxMenuBar* menuBar){}
		virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){}
		virtual void BuildToolBar(wxToolBar* toolBar){}
};

extern "C"
{
    PLUGIN_EXPORT cbPlugin* GetPlugin();
};

#endif // CLASSWIZARD_H
