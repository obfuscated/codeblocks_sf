#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <simpletextlog.h>

class PluginWizard : public cbToolPlugin
{
    public:
        PluginWizard();
        ~PluginWizard();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
        virtual int Configure(){ return -1; }
		virtual int Execute();
};

extern "C"
{
    PLUGIN_EXPORT cbPlugin* GetPlugin();
};

#endif // PLUGINWIZARD_H
