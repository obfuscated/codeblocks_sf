#ifndef __WXMSW__
  #error "This plugin is Windows only at the moment."
#endif

#ifndef HELP_PLUGIN_H
#define HELP_PLUGIN_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <cbproject.h>
#include <simpletextlog.h>
#include <sdk_events.h>
#include <settings.h> // much of the SDK is here
#include <sdk_events.h>
#include <cbplugin.h> // the base class we're inheriting
#include <wx/dynarray.h>
#include "HelpConfigDialog.h"

class HelpPlugin : public cbPlugin
{
  public:
    HelpPlugin();
    ~HelpPlugin();
    int Configure();
    void BuildMenu(wxMenuBar *menuBar);
    void BuildModuleMenu(const ModuleType type, wxMenu *menu, const wxString &arg);
    bool BuildToolBar(wxToolBar *toolBar);
    void OnAttach(); // fires when the plugin is attached to the application
    void OnRelease(bool appShutDown); // fires when the plugin is released from the application
    
  protected:
    void OnFindItem(wxCommandEvent &event);
    void OnHelp(wxCommandEvent &event);
    void AddToPopupMenu(wxMenu *menu, int id, const wxString &help);
    void AddToHelpMenu(int id, const wxString &help);
    void RemoveFromHelpMenu(int id, const wxString &help);
    void AddFile();
    wxString HelpFileFromId(int id);
    void LaunchHelp(const wxString &helpfile, const wxString &keyword = wxEmptyString);
  
  private:
    wxMenuBar *m_pMenuBar;
    HelpCommon::HelpFilesVector m_Vector;
    int m_LastId;
    
    DECLARE_EVENT_TABLE()
};

extern "C"
{
  PLUGIN_EXPORT cbPlugin *GetPlugin();
};

#endif // HELP_PLUGIN_H
