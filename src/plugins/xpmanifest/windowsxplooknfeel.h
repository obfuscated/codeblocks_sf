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
		int Update(const ModuleType type){ return -1; }
		void BuildMenu(wxMenuBar* menuBar){ return; }
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){ return; }
		void BuildToolBar(wxToolBar* toolBar){}
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(); // fires when the plugin is released from the application
	protected:
	private:
};

extern "C"
{
	PLUGIN_EXPORT cbPlugin* GetPlugin();
};

#endif // WINDOWSXPLOOKNFEEL_H

