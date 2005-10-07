#ifndef WINDOWSXPLOOKNFEEL_H
#define WINDOWSXPLOOKNFEEL_H

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // much of the SDK is here

class WindowsXPLookNFeel : public cbToolPlugin
{
	public:
		WindowsXPLookNFeel();
		~WindowsXPLookNFeel();
		int Configure(){ return -1; }
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	protected:
	private:
};

CB_DECLARE_PLUGIN();

#endif // WINDOWSXPLOOKNFEEL_H

