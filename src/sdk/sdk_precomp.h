/** Code::Blocks SDK precompiled headers support for internal usage.
  * This is precompiled with -DEXPORT_LIB -DEXPORT_EVENTS
  */

#ifndef SDK_PRECOMP_H
#define SDK_PRECOMP_H

#ifndef __WXMSW__
	// For non-windows platforms, one PCH (sdk.h) is enough...
	#include <sdk.h>
#else

#if ( defined(CB_PRECOMP) && !defined(WX_PRECOMP) )
    #define WX_PRECOMP
#endif // CB_PRECOMP

// basic wxWidgets headers
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#ifdef CB_PRECOMP
    // some common wxWidgets headers
    #include <wx/socket.h>
    #include <wx/notebook.h>
    #include <wx/image.h>
    #include <wx/frame.h>
    #include <wx/panel.h>
    #include <wx/dialog.h>
    #include <wx/msgdlg.h>
    #include <wx/fs_zip.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/font.h>
    #include <wx/hashmap.h>
    #include <wx/list.h>
    #include <wx/thread.h>
    #include <wx/menu.h>
    #include <wx/imaglist.h>
    #include <wx/process.h>
    #include <wx/splitter.h>
    #include <wx/sizer.h>
    #include <wx/txtstrm.h>
    #include <wx/timer.h>
    #include <wx/datetime.h>
    #include <wx/print.h>
    #include <wx/dynarray.h>
    #include <wx/event.h>
    #include <wx/dynarray.h>
    #include <wx/hashmap.h>
    #include <wx/string.h>
    #include <wx/filename.h>
    #include <wx/confbase.h>
    #include <wx/treectrl.h>
    #include <wx/radiobut.h>
    #include <wx/checkbox.h>
    #include <wx/textctrl.h>
    #include <wx/combobox.h>
    #include <wx/spinctrl.h>
    #include <wx/listctrl.h>
    #include <wx/button.h>
    #include <wx/stattext.h>
    #include <wx/wxscintilla.h>

    // basic headers
    #include <gcc-attribs.h>
    #include <settings.h>
    #include <globals.h>
    #include <licenses.h>
    #include <sdk_events.h>
    #include <sanitycheck.h>
    #include <cbexception.h>

    // absolute base classes
    #include <editorbase.h>
    #include <compileoptionsbase.h>
    #include <compiletargetbase.h>
    #include <projectbuildtarget.h>
    #include <openfilestree.h>
    #include <cbeditor.h>
    #include <cbplugin.h>
    #include <cbproject.h>
    #include <cbworkspace.h>

    // managers
    #include <manager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <messagelog.h>
    #include <messagemanager.h>
    #include <projectmanager.h>
    #include <menuitemsmanager.h>
    #include <scriptingmanager.h>
    #include <toolsmanager.h>
    #include <templatemanager.h>
    #include <macrosmanager.h>
    #include <pluginmanager.h>
    #include <personalitymanager.h>
    #include <uservarmanager.h>

    // other base classes
    #include <compileroptions.h>
    #include <compiler.h>
    #include <compilerfactory.h>
    #include <managedthread.h>
    #include <pipedprocess.h>
    #include <simplelistlog.h>
    #include <simpletextlog.h>
    #include <workspaceloader.h>
    #include <xtra_classes.h>
    #include <xtra_res.h>
#endif // CB_PRECOMP

#endif // !__WXMSW__

#endif // SDK_PRECOMP_H
