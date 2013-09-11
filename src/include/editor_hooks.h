/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITOR_HOOKS_H
#define EDITOR_HOOKS_H

#include "settings.h"

//uncomment the below line if you want to do the hook performance measure
//#define EDITOR_HOOK_PERFORMANCE_MEASURE

#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
    #include <typeinfo> // typeid
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

class cbEditor;
class cbSmartIndentPlugin;
class wxScintillaEvent;

/** Provides static functions to add hooks to the editor modification operations. */
namespace EditorHooks
{
    /** Abstract base hook functor interface. Similar to cbEventFunctor class*/
    class DLLIMPORT HookFunctorBase
    {
        public:
            virtual ~HookFunctorBase(){}
            virtual void Call(cbEditor*, wxScintillaEvent&) const = 0;

#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
            virtual const char* GetTypeName() const = 0;
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

    };

    /** Functor class for use as a editor modification operations hook.
      * Passed as the first parameter in RegisterHook() and
      * UnregisterHook().
      *
      * example:
      * EditorHooks::HookFunctorBase* myhook = new EditorHooks::HookFunctor<MyClass>(this, &MyClass::OnHookCalled);
      * int id = EditorHooks::RegisterHook(myhook);
      * ...
      * (and before your class' destruction - or earlier):
      * EditorHooks::UnregisterHook(id, true);
      *
      * Member functions used as hook callbacks must have the following signature:
      * void YourFunctionName(cbEditor*, wxScintillaEvent*)
      */
    template<class T> class HookFunctor : public HookFunctorBase
    {
        public:
            typedef void (T::*Func)(cbEditor*, wxScintillaEvent&);
            HookFunctor(T* obj, Func func) : m_pObj(obj), m_pFunc(func)
            { ; }
            virtual void Call(cbEditor* editor, wxScintillaEvent& event) const
            {
                if (m_pObj && m_pFunc)
                    (m_pObj->*m_pFunc)(editor, event);
            }

#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
            /** return the name (usually mangled C++ name for the member function) */
            virtual const char* GetTypeName() const
            {
                return typeid(m_pFunc).name();
            }
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

        protected:
            T* m_pObj;
            Func m_pFunc;
    };

    /** Register a project loading/saving hook.
      * @param functor The functor to use as a callback.
      * @return An ID. Use this to unregister your hook later.
      */
    extern DLLIMPORT int RegisterHook(HookFunctorBase* functor);
    /** Unregister a previously registered project loading/saving hook.
      * @param id The hook's ID. You should have the ID from when RegisterHook() was called.
      * @param deleteHook If true, the hook will be deleted (default). If not, it's
      * up to you to delete it.
      * @return The functor. If @c deleteHook was true, it always returns NULL.
      */
    extern DLLIMPORT HookFunctorBase* UnregisterHook(int id, bool deleteHook = true);
    /** Are there any hooks registered?
      * @return True if any hooks are registered, false if none.
      */
    extern DLLIMPORT bool HasRegisteredHooks();
    /** Call all registered hooks using the supplied parameters.
      * This is called by ProjectLoader.
      * @param editor The editor in question.
      * @param event Parameter (wxScintilla event) to provide to the registered hook
      */
    extern DLLIMPORT void CallHooks(cbEditor* editor, wxScintillaEvent& event);

    /** Provides a HookFunctor which redirects the Call() of a cbSmartIndentPlugin
      * so only the interface of cbSmartIndentPlugin has to be implemented for a new language.
      */
    class cbSmartIndentEditorHookFunctor : public HookFunctorBase
    {
        public:
            /** ctor. */
            cbSmartIndentEditorHookFunctor(cbSmartIndentPlugin* plugin);
            /** dtor. */
            virtual ~cbSmartIndentEditorHookFunctor(){}
            /** Needs to be implemented by the plugin to act(smart indent) accordingly.
              * @param editor The editor that is active and whose content is changed
              * @param event  The wxScintilla event fired to react accordingly (see cbEditor::CreateEditor, namely scintilla_events)
              */
            virtual void Call(cbEditor* editor, wxScintillaEvent& event) const;

#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
            virtual const char* GetTypeName() const
            {
                return typeid(m_plugin).name();
            }
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

        private:
            cbSmartIndentPlugin* m_plugin;
    };
}

#endif // EDITOR_HOOKS_H
