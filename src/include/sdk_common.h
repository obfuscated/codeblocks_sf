#ifndef SDK_COMMON_H
#define SDK_COMMON_H

//This file should be included only by sdk.h and sdk_precomp.h
//It includes all the common and necessary header files for precompilation.

/*
 * Enable backwards-compatibility for gcc 3.3 and lower.
 * Although the compiler does not support precompiled headers, the build might still use them.
 * We might significantly reduce the compile time for old compilers, by undefining CB_PRECOMP and thus
 * not including every header file twice.
 * This also allows us to reliably shortcut some includes for compilers that *do* support precompilation.
 */
#if defined(__GNUC__) && !defined(__APPLE__)
    #if ( (__GNUC__ < 3) || ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) ) )
        #undef CB_PRECOMP
    #endif
#endif // __GNUC__ && !__APPLE__


#if ( defined(CB_PRECOMP) && !defined(WX_PRECOMP) )
    #define WX_PRECOMP
#endif // CB_PRECOMP

// basic wxWidgets headers : this one itself will check for precompiled headers
// and if so will include a list of wx headers, at the bottom we add some more headers
// in the case of precompilation (note : some headers are in both lists)
// so even if NO CB_PRECOMP we can still have WX_PRECOMP turned on in this "wxprec" header
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include "prep.h" // this is deliberately not inside the #ifdef block

#ifdef CB_PRECOMP

    // some common wxWidgets headers
    #include <wx/arrstr.h>
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/checklst.h>
    #include <wx/choice.h>
    #include <wx/colordlg.h>
    #include <wx/combobox.h>
    #include <wx/confbase.h>
    #include <wx/datetime.h>
    #include <wx/dialog.h>
    #include <wx/dir.h>
    #include <wx/dynarray.h>
    #include <wx/event.h>
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/font.h>
    #include <wx/frame.h>
    #include <wx/fs_zip.h>
    #include <wx/hashmap.h>
    #include <wx/image.h>
    #include <wx/imaglist.h>
    #include <wx/intl.h>
    #include <wx/list.h>
    #include <wx/listbox.h>
    #include <wx/listctrl.h>
    #include <wx/log.h>
    #include <wx/menu.h>
    #include <wx/menuitem.h>
    #include <wx/msgdlg.h>
    #include <wx/notebook.h>
    #include <wx/panel.h>
    #include <wx/print.h>
    #include <wx/process.h>
    #include <wx/radiobox.h>
    #include <wx/radiobut.h>
    #include <wx/regex.h>
    #include <wx/sizer.h>
    #include <wx/socket.h>
    #include <wx/spinctrl.h>
    #include <wx/splitter.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/textctrl.h>
    #include <wx/thread.h>
    #include <wx/timer.h>
    #include <wx/toolbar.h>
    #include <wx/treectrl.h>
    #include <wx/txtstrm.h>
    #include <wx/utils.h>
    #include <wx/wfstream.h>
    #include <wx/wxscintilla.h>
    #include <wx/xrc/xmlres.h>

    // basic headers
    #include "settings.h"
    #include "globals.h"
    #include "sdk_events.h"
    #include "cbexception.h"

    // absolute base classes
    #include "editorbase.h"
    #include "cbeditor.h"
    #include "compileoptionsbase.h"
    #include "compiletargetbase.h"
    #include "projectbuildtarget.h"
    #include "projectfile.h"
    #include "cbplugin.h"
    #include "cbproject.h"
    #include "cbtool.h"
    #include "cbworkspace.h"
    #include "messagelog.h"
    #include "simpletextlog.h"
    #include "simplelistlog.h"
    #include "compilerfactory.h"
    #include "compiler.h"
    #include "workspaceloader.h"
    #include "editorcolourset.h"
    #include "pipedprocess.h"

    // managers
    #include "manager.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "messagemanager.h"
    #include "projectmanager.h"
    #include "menuitemsmanager.h"
    #include "scriptingmanager.h"
    #include "toolsmanager.h"
    #include "templatemanager.h"
    #include "macrosmanager.h"
    #include "pluginmanager.h"
    #include "personalitymanager.h"
    #include "uservarmanager.h"
    #include "filemanager.h"

    // other base files
    #include "xtra_res.h"
    #include "safedelete.h"
    #include "infowindow.h"
    #include "licenses.h"

#endif // CB_PRECOMP

#endif // SDK_COMMON_H
