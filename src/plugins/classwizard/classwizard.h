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
        virtual void OnRelease(bool appShutDown);
        virtual int Configure(){ return -1; }
		virtual int Execute();
};

extern "C"
{
    PLUGIN_EXPORT cbPlugin* GetPlugin();
};

#endif // CLASSWIZARD_H
