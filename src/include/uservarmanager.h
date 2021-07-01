/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef USER_VARIABLE_MANAGER_H
#define USER_VARIABLE_MANAGER_H

#include "settings.h"
#include "manager.h"
#include "cbexception.h"

#ifndef CB_PRECOMP
    #include "globals.h"
#endif

class UserVarManagerUI
{
public:
    virtual void DisplayInfoWindow(const wxString &title,const wxString &msg) = 0;
    virtual void OpenEditWindow(const wxArrayString &var = wxArrayString()) = 0;
    virtual wxString GetVariable(wxWindow* parent, const wxString &old) = 0;
};

class DLLIMPORT UserVariableManager : public Mgr<UserVariableManager>
{
        friend class Manager;
        friend class Mgr<UserVariableManager>;
        friend class MacrosManager;

        ConfigManager * m_CfgMan;
        wxString        m_ActiveSet;
        wxArrayString   m_Preempted;

        UserVarManagerUI* m_ui;

    public:
        UserVariableManager();
        ~UserVariableManager();

        void SetUI(UserVarManagerUI* ui);

        wxString Replace(const wxString& variable);

        wxString GetVariable(wxWindow *parent, const wxString &old);

        void Preempt(const wxString& variable);
        void Arrogate();
        bool Exists(const wxString& variable) const;

        void Configure();
        void Migrate();

        UserVariableManager& operator=(const UserVariableManager&) = delete;
        UserVariableManager(const UserVariableManager&) = delete;
};

#endif // USER_VARIABLE_MANAGER_H
