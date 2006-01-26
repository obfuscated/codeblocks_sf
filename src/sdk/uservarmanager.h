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
        static UserVariableManager* instance;

    public:
        wxString Replace(const wxString& variable);
        void Configure();
};

#endif // USER_VARIABLE_MANAGER_H
