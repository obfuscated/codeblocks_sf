#ifndef MACROSMANAGER_H
#define MACROSMANAGER_H

#include "settings.h"

// forward decls;
class wxMenuBar;

class DLLIMPORT MacrosManager
{
	public:
		friend class Manager;
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
		void ReplaceMacros(wxString& buffer);
		wxString ReplaceMacros(const wxString& buffer);
	protected:
	private:
        static MacrosManager* Get();
		static void Free();
		MacrosManager();
		~MacrosManager();
};

#endif // MACROSMANAGER_H

