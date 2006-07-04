#ifndef USER_VARIABLE_MANAGER_H
#define USER_VARIABLE_MANAGER_H

#include "settings.h"
#include "manager.h"
#include "macrosmanager.h"

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
        wxString Replace(const wxString& variable);

        void Preempt(const wxString& variable);
        void Arrogate();
        bool Exists(const wxString& variable) const;

        void Configure();
        void Migrate();
};

#endif // USER_VARIABLE_MANAGER_H
