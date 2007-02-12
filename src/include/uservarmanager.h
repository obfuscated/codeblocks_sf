#ifndef USER_VARIABLE_MANAGER_H
#define USER_VARIABLE_MANAGER_H

#include "settings.h"
#include "manager.h"
#include "cbexception.h"

#ifndef CB_PRECOMP
    #include "globals.h"
#endif

class DLLIMPORT UserVariableManager : public Mgr<UserVariableManager>
{
        friend class Manager;
        friend class Mgr<UserVariableManager>;
        friend class MacrosManager;
        ConfigManager * cfg;
        wxString activeSet;
        wxArrayString preempted;

    public:
        UserVariableManager();

        UserVariableManager(const UserVariableManager& rhs) { cbThrow(_T("Can't call UserVariableManager's copy ctor!!!")); }
        virtual void operator=(const UserVariableManager& rhs){ cbThrow(_T("Can't assign an UserVariableManager* !!!")); }

        wxString Replace(const wxString& variable);

        void Preempt(const wxString& variable);
        void Arrogate();
        bool Exists(const wxString& variable) const;

        void Configure();
        void Migrate();
};

#endif // USER_VARIABLE_MANAGER_H
