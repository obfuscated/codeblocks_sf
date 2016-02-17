/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SC_PLUGIN_H
#define SC_PLUGIN_H

//#include "sc_base_types.h"

#include <wx/dynarray.h>
#include <wx/event.h>
#include <deque>
#include <scripting/squirrel/squirrel.h>
#include <scripting/sqrat.h>
#include <scripting/bindings/sc_cb_vm.h>

#ifndef CB_PRECOMP
    #include "pluginmanager.h"
    #include "menuitemsmanager.h"
    #include <cbproject.h>
#endif // CB_PRECOMP



class FileTreeData;
class wxMenu;
//class sq_wxDialog;

namespace ScriptBindings
{


namespace ScriptPluginWrapper
{
wxArrayInt CreateMenu(wxMenuBar* mbar);
wxArrayInt CreateModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data);

SQInteger RegisterPlugin(HSQUIRRELVM vm);

void OnScriptMenu(int id);
void OnScriptModuleMenu(int id);
} // namespace ScriptPluginWrapper


class cbScriptPlugin : public wxEvtHandler
{
public:

    cbScriptPlugin(Sqrat::Object obj);
    virtual ~cbScriptPlugin();


    /** \brief Event function called on a click in a menu
     *
     * This function calls the specific handler function in the script
     * \param evt wxMenuEvent& the Menu event
     * \return void
     *
     */
    void OnMenu(wxMenuEvent &evt);

    /** \brief Event function called on a click in a menu
     *
     * This function calls the specific handler function in the script
     * \param evt wxMenuEvent& the Menu event
     * \return void
     *
     */
    void OnModulMenu(wxMenuEvent &evt);

    virtual void BuildModuleMenu(cb_optional const ModuleType type, cb_optional wxMenu* menu, cb_optional const FileTreeData* data = nullptr);

    virtual void BuildMenu(cb_optional wxMenuBar* menuBar)
    {
        CreateMenus();
    };


    /** \brief Return the PluginInfo object
     *
     * \return PluginInfo
     *
     */
    PluginInfo  GetInfo()
    {
        return m_info;
    };
    wxString    GetName()
    {
        return m_info.name;
    };


    /** \brief Registers a c::b event for the script
     *
     * \param evt wxEventType   Type of the event
     * \param func wxString     Name of the function to be called in script
     * \return int
     *
     */
    int RegisterCBEvent(wxEventType evt, wxString func);


    /** \brief Event Handler for C::B events.
     *
     * Interface between script and C::B. This function calls the event handler in the script
     * \param evt CodeBlocksEvent&
     * \return void
     *
     */
    void OnCBEvt(CodeBlocksEvent& evt);


    int CreateMenus();

    /** \brief Run the "Execute" function from the plugin
     *
     * \return int  1 on success
     *             -1 Execute throw an error (The error was reported to the user by ScriptManager::DisplayError()
     *             -2 "Execute" could not be found in the plugin script
     *
     */
    int Execute();

    Sqrat::Object GetObject()
    {
        return m_object;
    };

    wxString GetScriptFile()                {return m_script_file;};


    void SetScriptFile(wxString file)
    {
        m_script_file = file;
    };


    friend SQInteger ScriptPluginWrapper::RegisterPlugin(HSQUIRRELVM vm);

protected:
    void SetInfo(PluginInfo info)
    {
        m_info = info;
    };

private:

    PluginInfo m_info;

    typedef std::map<wxEventType,wxString> cb_evt_func_map;
    cb_evt_func_map m_cb_evt_map;

    struct menu_entry
    {
        int index;
        wxString entry_string;
        int type;
    };

    typedef std::map<int,menu_entry>   cb_menu_id_to_idx;  // [id] -> idx
    cb_menu_id_to_idx m_menu_to_idx_map;
    cb_menu_id_to_idx m_modul_menu_to_idx_map;

    bool m_AttachedToMainWindow;

    MenuItemsManager m_menu_manager;
    Sqrat::Object m_object;
    wxString m_script_file;

};
/** \brief Get a pointer to the registered Plugin presented in the object
 *
 * \param obj Sqrat::Object
 * \return cbScriptPlugin*  if _nullptr_ is returned a error occurred and the function has thrown a error to the vm. The squirrel function has to return SQ_ERROR
 *
 */
cbScriptPlugin* GetPluginFromObject(StackHandler& sa,Sqrat::Object obj);

}

#endif // SC_PLUGIN_H

