/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CB_KODERS_H_INCLUDED
#define CB_KODERS_H_INCLUDED

#include "cbplugin.h" // for "class cbToolPlugin"

class KodersDialog;

class CB_Koders : public cbToolPlugin
{
public:
  /** Constructor. */
  CB_Koders();
  /** Destructor. */
  virtual ~CB_Koders();

  /** @brief Execute the plugin.
    *
    * This is the only function needed by a cbToolPlugin.
    * This will be called when the user selects the plugin from the "Plugins"
    * menu.
    */
  virtual int Execute();

  /** This method is called by Code::Blocks core modules (EditorManager,
    * ProjectManager etc) and is used by the plugin to add any menu
    * items it needs in the module's popup menu. For example, when
    * the user right-clicks on a project file in the project tree,
    * ProjectManager prepares a popup menu to display with context
    * sensitive options for that file. Before it displays this popup
    * menu, it asks all attached plugins (by asking PluginManager to call
    * this method), if they need to add any entries
    * in that menu. This method is called.\n
    * If the plugin does not need to add items in the menu,
    * just do nothing ;)
    * @param type the module that's preparing a popup menu
    * @param menu pointer to the popup menu
    * @param data pointer to FileTreeData object (to access/modify the file tree)
    */
  void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);

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

private:
  bool IsReady();
  void OnSearchKoders(wxCommandEvent& event);

  KodersDialog* TheDialog;

  DECLARE_EVENT_TABLE()
};

#endif // CB_KODERS_H_INCLUDED
