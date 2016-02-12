/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SCRIPTBINDINGS_H
#define SCRIPTBINDINGS_H

#include <scripting/bindings/sc_cb_vm.h>
#include <scripting/sqrat.h>



namespace ScriptBindings
{
    void RegisterBindings(HSQUIRRELVM vm);
}

#endif // SCRIPTBINDINGS_H
