//{Info
/*
 ** Purpose:   Code::Blocks - AutoVersioning Plugin
 ** Author:    JGM
 ** Created:   06/27/07 11:19:56 a.m.
 ** Copyright: (c) JGM
 ** License:   GPL
 */
//}

#ifndef AUTOVERSIONING_H
#define AUTOVERSIONING_H

#include <map>
#include <cbplugin.h>
#include "avConfig.h"

class wxTimer;
class wxTimerEvent;
class cbProject;

class AutoVersioning : public cbPlugin
{
public: //Constructor and Destructor
	AutoVersioning();
	~AutoVersioning();

	//Functions
	int Configure(){ return -1; }
	void BuildMenu(wxMenuBar* menuBar);
	void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
	bool BuildToolBar(wxToolBar* toolBar){return false;}
	void UpdateVersionHeader();

	//Events
	void OnAttach();
	void OnRelease(bool appShutDown);
	void OnMenuAutoVersioning(wxCommandEvent& event);
	void OnMenuCommitChanges(wxCommandEvent& event);
	void OnMenuChangesLog(wxCommandEvent& event);
	void OnUpdateUI(wxUpdateUIEvent& event);
	void OnCompilerStarted(CodeBlocksEvent& event);
	void OnCompilerFinished(CodeBlocksEvent& event);
	void OnTimerVerify(wxTimerEvent& event);
private:
	void SetVersionAndSettings(cbProject& Project, bool update = false);
	void CommitChanges();
	void GenerateChanges();
	wxString FileNormalize(const wxString& relativeFile, const wxString& workingDirectory);
//    const avConfig& GetConfig() const;
    avConfig& GetConfig();
    avVersionState& GetVersionState();

    //Private members
    wxString m_versionHeaderPath;
    wxTimer* m_timerStatus;
    int m_AutoVerHookId; //!< project loader hook ID
    std::map<cbProject*, avConfig> m_ProjectMap;
    std::map<cbProject*, avVersionState> m_ProjectMapVersionState;
    cbProject* m_Project; // keeps track of the last 'activated' project
    bool m_Modified; // have some settings been modified
    bool m_IsCurrentProjectVersioned;
    /// fires when a project is being loaded / saved
    void OnProjectLoadingHook(cbProject* project, TiXmlElement* elem, bool loading);

    /// fires when a project is being activated
    void OnProjectActivated(CodeBlocksEvent& event);

    /// fires when a project is being closed
    void OnProjectClosed(CodeBlocksEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // AUTOVERSIONING_H

