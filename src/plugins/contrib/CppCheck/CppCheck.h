/***************************************************************
 * Name:      CppCheck.h
 * Purpose:   Code::Blocks CppCheck plugin: main functions
 * Author:    Lieven de Cock (aka killerbot)
 * Created:   12/11/2009
 * Copyright: (c) Lieven de Cock (aka killerbot)
 * License:   GPL
 **************************************************************/

#ifndef CPPCHECK_H_INCLUDED
#define CPPCHECK_H_INCLUDED

#include "cbplugin.h" // the base class we 're inheriting

class TextCtrlLogger;
class CppCheckListLog;

class CppCheck : public cbToolPlugin
{
public:
    CppCheck();
    ~CppCheck();
    void OnAttach(); // fires when the plugin is attached to the application
    void OnRelease(bool appShutDown); // fires when the plugin is released from the application
    int Execute();
private:
    void WriteToLog(const wxString& Text);
    void AppendToLog(const wxString& Text);
    bool DoCppCheckVersion();

    TextCtrlLogger*  m_CppCheckLog;      //!< log tab in the message pane
    CppCheckListLog* m_ListLog;          //!< log tab to click/double click to take you to offending line of code
    wxString         m_CppCheckApp;      //!< The path to the application (provided by the user on request)
    int              m_LogPageIndex;     //!< index of our log tab (can this change during run time ??)
    int              m_ListLogPageIndex; //!< index of our list log tab
};

#endif // CPPCHECK_H_INCLUDED
