/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifndef CB_SC_TYPEINFO_ALL_H
#define CB_SC_TYPEINFO_ALL_H

/// @file
/// This file contains type traits for all C++ classes/structs known to Squirrel through our
/// binding.
/// The trait provide information about:
///  * the type tag which should  be unique;
///  * the squirrel name of the class/struct;
///  * the base class of the class/struct;

namespace ScriptBindings
{

enum class TypeTag : uint32_t
{
    wxString = 0x8000,
    wxColour,
    wxPoint,
    wxSize,
    wxArrayString,
    wxFileName,
    cbEditor,
    ConfigManager,
    EditorBase,
    EditorManager,
    PluginInfo,
    ScriptingManager,
    UserVariableManager
};

template<>
struct TypeInfo<wxString> {
    static const uint32_t typetag = uint32_t(TypeTag::wxString);
    static constexpr const SQChar *className = _SC("wxString");
    using baseClass = void;
};

template<>
struct TypeInfo<wxArrayString> {
    static const uint32_t typetag = uint32_t(TypeTag::wxArrayString);
    static constexpr const SQChar *className = _SC("wxArrayString");
    using baseClass = void;
};

template<>
struct TypeInfo<wxColour> {
    static const uint32_t typetag = uint32_t(TypeTag::wxColour);
    static constexpr const SQChar *className = _SC("wxColour");
    using baseClass = void;
};

template<>
struct TypeInfo<wxPoint> {
    static const uint32_t typetag = uint32_t(TypeTag::wxPoint);
    static constexpr const SQChar *className = _SC("wxPoint");
    using baseClass = void;
};

template<>
struct TypeInfo<wxSize> {
    static const uint32_t typetag = uint32_t(TypeTag::wxSize);
    static constexpr const SQChar *className = _SC("wxSize");
    using baseClass = void;
};

template<>
struct TypeInfo<wxFileName> {
    static const uint32_t typetag = uint32_t(TypeTag::wxFileName);
    static constexpr const SQChar *className = _SC("wxFileName");
    using baseClass = void;
};

template<>
struct TypeInfo<cbEditor> {
    static const uint32_t typetag = uint32_t(TypeTag::cbEditor);
    static constexpr const SQChar *className = _SC("cbEditor");
    using baseClass = void;
};

template<>
struct TypeInfo<ConfigManager> {
    static const uint32_t typetag = uint32_t(TypeTag::ConfigManager);
    static constexpr const SQChar *className = _SC("ConfigManager");
    using baseClass = void;
};

template<>
struct TypeInfo<EditorBase> {
    static const uint32_t typetag = uint32_t(TypeTag::EditorBase);
    static constexpr const SQChar *className = _SC("EditorBase");
    using baseClass = void;
};

template<>
struct TypeInfo<EditorManager> {
    static const uint32_t typetag = uint32_t(TypeTag::EditorManager);
    static constexpr const SQChar *className = _SC("EditorManager");
    using baseClass = void;
};

template<>
struct TypeInfo<PluginInfo> {
    static const uint32_t typetag = uint32_t(TypeTag::PluginInfo);
    static constexpr const SQChar *className = _SC("PluginInfo");
    using baseClass = void;
};

template<>
struct TypeInfo<ScriptingManager> {
    static const uint32_t typetag = uint32_t(TypeTag::ScriptingManager);
    static constexpr const SQChar *className = _SC("ScriptingManager");
    using baseClass = void;
};

template<>
struct TypeInfo<UserVariableManager> {
    static const uint32_t typetag = uint32_t(TypeTag::UserVariableManager);
    static constexpr const SQChar *className = _SC("UserVariableManager");
    using baseClass = void;
};

} // namespace ScriptBindings

#endif // CB_SC_TYPEINFO_ALL_H

