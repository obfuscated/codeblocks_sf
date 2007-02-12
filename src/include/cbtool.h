#ifndef CBTOOL_H
#define CBTOOL_H

#include <wx/string.h>

class cbTool
{
  public:
    enum eLaunchOption
    {
        LAUNCH_NEW_CONSOLE_WINDOW,
        LAUNCH_HIDDEN,
        LAUNCH_VISIBLE,
        LAUNCH_VISIBLE_DETACHED
    };

    cbTool() { m_MenuId = -1; }
    // getters
    wxString GetName() const {return m_Name;}
    wxString GetCommand() const {return m_Command;}
    wxString GetParams() const {return m_Params;}
    wxString GetWorkingDir() const {return m_WorkingDir;}
    eLaunchOption GetLaunchOption() const {return m_LaunchOption;}
    int GetMenuId() const {return m_MenuId;}
    // setters
    void SetName(const wxString& Name) {m_Name = Name;}
    void SetCommand(const wxString& Command) {m_Command = Command;}
    void SetParams(const wxString& Params) {m_Params = Params;}
    void SetWorkingDir(const wxString& WorkingDir) {m_WorkingDir = WorkingDir;}
    void SetLaunchOption(eLaunchOption LaunchOption) {m_LaunchOption = LaunchOption;}
    void SetMenuId(int MenuId) {m_MenuId = MenuId;}
  private:
    wxString m_Name;
    wxString m_Command;
    wxString m_Params;
    wxString m_WorkingDir;
    eLaunchOption m_LaunchOption;
    int m_MenuId;
};

#endif // CBTOOL_H
