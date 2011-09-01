/***************************************************************
 * Name:      ThreadSearchFrame.h
 * Purpose:   Defines Application Frame
 * Author:    Pecan ()
 * Created:   2008-02-27
 * Copyright: Pecan ()
 * License:
 **************************************************************/

#ifndef SNIPSEARCHAPPMAIN_H
#define SNIPSEARCHAPPMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/docview.h> // for wxFileHistory

#include "configmanager.h"

//-#include "SnippetsSearchApp.h"
class ThreadSearch;
class EditorManager;
class ProjectManager;
class LogManager;
class wxSplitterWindow;
class CodeSnippetsEvent;

// ----------------------------------------------------------------------------
class ThreadSearchFrame: public wxFrame
// ----------------------------------------------------------------------------
{
    friend class CodeSnippetsWindow;
    public:
        ThreadSearchFrame(wxFrame *frame, const wxString& title);
        ~ThreadSearchFrame();

        bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files);

    protected:
        void TerminateRecentFilesHistory();

        wxFileHistory* m_pFilesHistory;
        wxFileHistory* m_pProjectsHistory;
        void OnClose(wxCloseEvent& event);

    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout
        };
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSizeWindow(wxSizeEvent &event);
       void OnMenuOptions(wxCommandEvent& event);

        void OnCodeSnippetsNewIndex(CodeSnippetsEvent& event);

        //Search/Find
        void OnSearchFind(wxCommandEvent& event);
        void OnSearchFindNext(wxCommandEvent& event);
        //File
        void OnFileOpen(wxCommandEvent& event);
        void OnWindowDestroy(wxEvent& event);


        bool InitThreadSearchFrame(wxFrame* appFrame, const wxString& title);
        void DoOnFileOpen(bool bProject);
        bool OpenGeneric(const wxString& filename, bool addToHistory = true);
        bool DoOpenFile(const wxString& filename, bool addToHistory = true);
        void InitializeRecentFilesHistory();
        void AddToRecentFilesHistory(const wxString& filename);
        void OnFrameActivated(wxActivateEvent& event);

        void CreateMenuBar();
        bool InitXRCStuff();
        void ComplainBadInstall();

        EditorManager*  m_pEdMan;
        ProjectManager* m_pProjectMgr;
        LogManager*     m_pMsgMan;
        int             m_bOnActivateBusy;
        ThreadSearch*   m_pThreadSearch;


        DECLARE_EVENT_TABLE()
};

#endif // SNIPSEARCHAPPMAIN_H
