#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include "cbplugin.h"

class PluginWizard : public cbToolPlugin
{
    public:
        PluginWizard();
        ~PluginWizard();

        void OnAttach();
        void OnRelease(bool appShutDown);
        int Configure(){ return -1; }
		int Execute();
};

CB_DECLARE_PLUGIN();

#endif // PLUGINWIZARD_H
