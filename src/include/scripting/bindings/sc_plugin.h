/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SC_PLUGIN_H
#define SC_PLUGIN_H

#include "sc_base_types.h"
#include <wx/dynarray.h>

class FileTreeData;
class wxMenu;

namespace ScriptBindings
{
    namespace ScriptPluginWrapper
    {
        wxArrayInt CreateMenu(wxMenuBar* mbar);
        wxArrayInt CreateModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data);

        void OnScriptMenu(int id);
        void OnScriptModuleMenu(int id);
    } // namespace ScriptPluginWrapper
}

#endif // SC_PLUGIN_H
