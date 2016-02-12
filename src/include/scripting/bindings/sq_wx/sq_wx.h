/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SQ_WX_H
#define SQ_WX_H

#include <scripting/squirrel/squirrel.h>
#include <scripting/sqrat.h>
#include <scripting/bindings/sq_wx/sq_wx_type_handler.h>


namespace ScriptBindings
{
    namespace SQ_WX_binding
    {

        void bind_wxString(HSQUIRRELVM vm);
    }

    void bind_wx_util_dialogs(HSQUIRRELVM vm);
    void Register_wxGlobals(HSQUIRRELVM vm);
    void bind_wxConstants(HSQUIRRELVM vm);
}

#endif // SQ_WX_H
