#ifndef MACROSMANAGER_H
#define MACROSMANAGER_H

#include "settings.h"
#include "sanitycheck.h"
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
		DECLARE_SANITY_CHECK
};

#endif // MACROSMANAGER_H

