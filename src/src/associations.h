#ifndef ASSOCIATIONS_H
#define ASSOCIATIONS_H

#include <wx/intl.h>
#ifdef __WXMSW__
	#include <wx/msw/registry.h>
	#include <shlobj.h> // for SHChangeNotify()
	#define DDE_SERVICE	_T("CODEBLOCKS")
	#define DDE_TOPIC	_T("CodeBlocksDDEServer")
	#ifdef __CBDEBUG__
        #include <windows.h>
        #include <wincon.h>
        #include <wx/log.h>
	#endif
#endif

namespace Associations
{
    void Set();
    void SetBatchBuildOnly();
    bool Check();
};

#endif // ASSOCIATIONS_H
