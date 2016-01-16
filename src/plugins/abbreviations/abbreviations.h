/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ABBREVIATIONS_H_INCLUDED
#define ABBREVIATIONS_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <cbplugin.h> // for "class cbPlugin"

WX_DECLARE_STRING_HASH_MAP(wxString, AutoCompleteMap);

WX_DECLARE_STRING_HASH_MAP(AutoCompleteMap*, AutoCompLanguageMap);

extern wxString defaultLanguageStr;

class Abbreviations : public cbPlugin
{
public:
    /** Constructor. */
    Abbreviations();
    /** Destructor. */
    virtual ~Abbreviations();

    /** Return the plugin's configuration priority.
      * This is a number (default is 50) that is used to sort plugins
      * in configuration dialogs. Lower numbers mean the plugin's
      * configuration is put higher in the list.
      */
    virtual int GetConfigurationPriority() const { return 50; }

    /** Return the configuration group for this plugin. Default is cgUnknown.
      * Notice that you can logically OR more than one configuration groups,
      * so you could set it, for example, as "cgCompiler | cgContribPlugin".
      */
    virtual int GetConfigurationGroup() const { return cgEditor; }

    /** Return plugin's configuration panel.
      * @param parent The parent window.
      * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
      */
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

    /** This method is called by Code::Blocks and is used by the plugin
      * to add any menu items it needs on Code::Blocks's menu bar.\n
      * It is a pure virtual method that needs to be implemented by all
      * plugins. If the plugin does not need to add items on the menu,
      * just do nothing ;)
      * @param menuBar the wxMenuBar to create items in
      */
    virtual void BuildMenu(wxMenuBar* menuBar);

protected:
    /** Any descendent plugin should override this virtual method and
      * perform any necessary initialization. This method is called by
      * Code::Blocks (PluginManager actually) when the plugin has been
      * loaded and should attach in Code::Blocks. When Code::Blocks
      * starts up, it finds and <em>loads</em> all plugins but <em>does
      * not</em> activate (attaches) them. It then activates all plugins
      * that the user has selected to be activated on start-up.\n
      * This means that a plugin might be loaded but <b>not</b> activated...\n
      * Think of this method as the actual constructor...
      */
    virtual void OnAttach();

    /** Any descendent plugin should override this virtual method and
      * perform any necessary de-initialization. This method is called by
      * Code::Blocks (PluginManager actually) when the plugin has been
      * loaded, attached and should de-attach from Code::Blocks.\n
      * Think of this method as the actual destructor...
      * @param appShutDown If true, the application is shutting down. In this
      *         case *don't* use Manager::Get()->Get...() functions or the
      *         behaviour is undefined...
      */
    virtual void OnRelease(bool appShutDown);

public:
    /** Try to auto-complete the current word.
      *
      * This has nothing to do with code-completion plugins. Editor auto-completion
      * is a feature that saves typing common blocks of code, e.g.
      *
      * If you have typed "forb" (no quotes) and select auto-complete, then
      * it will convert "forb" to "for ( ; ; ){ }".
      * If the word up to the caret position is an unknown keyword, nothing happens.
      *
      * These keywords/code pairs can be edited in the editor configuration
      * dialog.
      */
    static void AutoComplete(cbEditor& ed);

    static void ExchangeTabAndSpaces(AutoCompleteMap& map);

private:
    /** \brief Function returing singleton instance */
    static Abbreviations* Get() { return m_Singleton; }

    void DoAutoComplete(cbEditor* ed);
    static Abbreviations* m_Singleton;            ///< \brief Singleton object

    /** \brief Registering scripting stuff */
    void RegisterScripting();

    /** \brief unregistering scripting stuff */
    void UnregisterScripting();

    void LoadAutoCompleteConfig();
    void SaveAutoCompleteConfig();

    void ClearAutoCompLanguageMap();
    AutoCompleteMap* GetCurrentACMap(cbEditor* ed);

private:
    void OnEditAutoComplete(cb_unused wxCommandEvent& event);
    void OnEditMenuUpdateUI(wxUpdateUIEvent& event);

public:
  void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);

private:
    friend class AbbreviationsConfigPanel;
    AutoCompLanguageMap m_AutoCompLanguageMap;

    int             m_EditorHookId;
    bool            m_IsAutoCompVisible; //!< is AutoComp opened by Abbreviations
private:
    DECLARE_EVENT_TABLE();
};

#endif // ABBREVIATIONS_H_INCLUDED
