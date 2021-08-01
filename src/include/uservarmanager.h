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

/** \brief Virtual class representing UI for UserVariableManager
 */
class UserVarManagerUI
{
public:
    virtual ~UserVarManagerUI() {};
    /** \brief Show an information window to the user
     *
     * This window
     * \param title const wxString& Title of the information window
     * \param msg const wxString& Content of the info window
     */
    virtual void DisplayInfoWindow(const wxString &title,const wxString &msg) = 0;

    /** \brief Open the variable editor window and if the not exist create the variables from the set
     *
     * \param var Variables to create
     */
    virtual void OpenEditWindow(const wxArrayString &var = wxArrayString()) = 0;

    /** \brief Open Dialog that asks the user for a variable
     *
     * \param parent wxWindow* Parent window
     * \param old const wxString& Old variable to display as selected
     * \return virtual wxString Global variable the user has selected
     *
     */
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

        static const wxString cBase;
        static const wxString cDir;
        static const wxChar   cSlash;
        static const wxString cSlashBase;
        static const wxString cInclude;
        static const wxString cLib;
        static const wxString cObj;
        static const wxString cBin;
        static const wxString cCflags;
        static const wxString cLflags;
        static const wxString cSets;
        static const wxArrayString builtinMembers;
};

#endif // USER_VARIABLE_MANAGER_H
