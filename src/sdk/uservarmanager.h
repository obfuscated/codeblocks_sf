#ifndef USER_VARIABLE_MANAGER_H
#define USER_VARIABLE_MANAGER_H

#include "settings.h"

class DLLIMPORT UserVariableManager
{
        friend class Manager;
        friend class MacrosManager;
        static UserVariableManager* instance;

        static UserVariableManager* Get();
        static void Free();

        UserVariableManager();

    public:
        wxString Replace(const wxString& variable);
        void Configure();
};

#endif // USER_VARIABLE_MANAGER_H
