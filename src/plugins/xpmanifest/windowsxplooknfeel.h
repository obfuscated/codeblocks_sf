#ifndef WINDOWSXPLOOKNFEEL_H
#define WINDOWSXPLOOKNFEEL_H

#include "cbplugin.h" // the base class we 're inheriting

#ifndef __WXMSW__
    #error This plugin is for use under Win32 only!
#endif

class WindowsXPLookNFeel : public cbToolPlugin
{
	public:
		WindowsXPLookNFeel();
		~WindowsXPLookNFeel();
		int Configure(){ return -1; }
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
};

#endif // WINDOWSXPLOOKNFEEL_H

