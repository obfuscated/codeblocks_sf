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

#include <wx/string.h>

class wxArrayString;
class cbProject;
class ConfigManager;
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

    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* /*parent*/);
private:
    void WriteToLog(const wxString& Text);
    void AppendToLog(const wxString& Text);

    //{ CppCheck
    struct SCppCheckAttribs
    {
      wxString InputFileName;
      wxString IncludeList;
      wxString DefineList;
    };
    typedef struct SCppCheckAttribs TCppCheckAttribs;

    int ExecuteCppCheck(cbProject* Project);
    int  DoCppCheckExecute(TCppCheckAttribs& CppCheckAttribs);
    void DoCppCheckAnalysis(const wxString& Xml);
    bool DoCppCheckParseXMLv1(TiXmlHandle& Handle);
    bool DoCppCheckParseXMLv2(TiXmlHandle& Handle);
    //} CppCheck

    //{ Vera
    int ExecuteVera(cbProject* Project);
    int  DoVeraExecute(const wxString& InputsFile);
    void DoVeraAnalysis(const wxArrayString& Result);
    //} Vera

    bool DoVersion(const wxString& app, const wxString& app_cfg);
    bool AppExecute(const wxString& app, const wxString& CommandLine, wxArrayString& Output, wxArrayString& Errors);
    wxString GetAppExecutable(const wxString& app, const wxString& app_cfg);

    TextCtrlLogger*  m_CppCheckLog;      //!< log tab in the message pane
    CppCheckListLog* m_ListLog;          //!< log tab to click/double click to take you to offending line of code
    int              m_LogPageIndex;     //!< index of our log tab (can this change during run time ??)
    int              m_ListLogPageIndex; //!< index of our list log tab

    wxString         m_PATH;
};

#endif // CPPCHECK_H_INCLUDED
