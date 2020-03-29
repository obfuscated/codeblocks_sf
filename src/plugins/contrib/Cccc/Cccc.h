/***************************************************************
 * Name:      Cccc.h
 * Purpose:   Code::Blocks Valgrind plugin: main functions
 * Author:    Lieven de Cock (aka killerbot)
 * Created:   12/11/2009
 * Copyright: (c) Lieven de Cock (aka killerbot)
 * License:   GPL
 **************************************************************/

#ifndef CCCC_H_INCLUDED
#define CCCC_H_INCLUDED

#include "cbplugin.h" // the base class we 're inheriting

class TextCtrlLogger;

class Cccc : public cbToolPlugin
{
public:
    Cccc();
    ~Cccc();
    void OnAttach(); // fires when the plugin is attached to the application
    void OnRelease(bool appShutDown); // fires when the plugin is released from the application
    int Execute();
private:
    void AppendToLog(const wxString& Text);

    TextCtrlLogger* m_CcccLog;      //!< log tab in the message pane
    int             m_LogPageIndex; //!< index of our log tab (can this change during run time ??)
    wxString        m_CcccApp;      //!< The path to the application (provided by the user on request)
};

#endif // CCCC_H_INCLUDED
