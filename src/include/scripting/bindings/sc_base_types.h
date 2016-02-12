/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SC_BASE_TYPES_H
#define SC_BASE_TYPES_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/animate.h>
#include <wx/checkbox.h>

#include <globals.h>
#include <settings.h>
#include <manager.h>
#include <logmanager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <macrosmanager.h>
#include <uservarmanager.h>
#include <pluginmanager.h>
#include <scriptingmanager.h>
#include <compilerfactory.h>
#include <compiletargetbase.h>
#include <cbproject.h>
#include <cbeditor.h>
#include <scripting/sqrat.h>
#include <scripting/bindings/sc_cb_vm.h>
#include <scripting/bindings/sc_binding_util.h>
#include <scripting/bindings/sq_wx/sq_wx_type_handler.h>

#define DEFINE_SQRAT_ENUM(N) namespace Sqrat                        \
{                                                                   \
template<>                                                          \
struct Var<N> {                                                     \
    N value;                                                        \
    Var(HSQUIRRELVM v, SQInteger idx) {                             \
        if (!Sqrat::Error::Occurred(v)) {                \
            SQInteger tmp = 0;                                      \
            sq_getinteger(v,idx,&tmp);                              \
            value = static_cast<N>(tmp);                            \
        }                                                           \
    }                                                               \
    static void push(HSQUIRRELVM v, const N value) {                \
        sq_pushinteger(v,static_cast<SQInteger>(value));            \
    }                                                               \
};                                                                   \
}

DEFINE_SQRAT_ENUM(TemplateOutputType);
DEFINE_SQRAT_ENUM(FileType);
DEFINE_SQRAT_ENUM(SearchDirs);
DEFINE_SQRAT_ENUM(SupportedPlatforms);
DEFINE_SQRAT_ENUM(TargetFilenameGenerationPolicy);
DEFINE_SQRAT_ENUM(ModuleType);
DEFINE_SQRAT_ENUM(PrintColourMode);
DEFINE_SQRAT_ENUM(PrintScope);
DEFINE_SQRAT_ENUM(PCHMode);
DEFINE_SQRAT_ENUM(MakeCommand);
DEFINE_SQRAT_ENUM(TargetType);
DEFINE_SQRAT_ENUM(OptionsRelation);
DEFINE_SQRAT_ENUM(OptionsRelationType);
DEFINE_SQRAT_ENUM(wxPathNormalize);
DEFINE_SQRAT_ENUM(wxPathFormat);
DEFINE_SQRAT_ENUM(wxAnimationType);
DEFINE_SQRAT_ENUM(wxCheckBoxState);



#endif // SC_BASE_TYPES_H
