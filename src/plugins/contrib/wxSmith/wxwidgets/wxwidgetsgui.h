#ifndef WXWIDGETSGUI_H
#define WXWIDGETSGUI_H

#include "../wxsgui.h"
#include "../wxscodinglang.h"

/** \brief Class responsible for operations on wxWidgets-based projects */
class wxWidgetsGUI : public wxsGUI
{
    DECLARE_CLASS(wxWidgetsGUI)
    public:
        /** \brief Ctor */
        wxWidgetsGUI(wxsProject* Project);

        /** \brief Dctor */
        virtual ~wxWidgetsGUI();

    private:

        virtual cbConfigurationPanel* OnBuildConfigurationPanel(wxWindow* Parent);
        virtual void OnRebuildApplicationCode();
        virtual bool OnCheckIfApplicationManaged();
        virtual bool OnCreateApplicationBinding();
        virtual void OnReadConfig(TiXmlElement* element);
        virtual void OnWriteConfig(TiXmlElement* element);

        /** \brief Checking if file with given name has source code prepared to be managed by wxSmith */
        bool IsAppSourceManaged(const wxString& FileName,wxsCodingLang Lang);

        /** \brief Checking if given project file can be adopted to wxSmith */
        bool ScanForApp(ProjectFile* File);

        /** \brief Adding smith bindings to given file */
        bool AddSmithToApp(const wxString& RelativeFileName,wxsCodingLang Lang);

        /** \brief Creating new file with application class */
        bool CreateNewApp(const wxString& FileName);

        /** \brief Getting name of class used inside IMPLEMENT_APP() macro call */
        wxString GetAppClassName(const wxString& Source,wxsCodingLang Lang);

        /** \brief Enumerating all resources in project which could be set as main resource in wxWidgets */
        void EnumerateMainResources(wxArrayString& Names);

        /* Some helper functions used while scanning source files */
        static inline bool IsWhite(wxChar Ch);
        static inline bool IsWhite(const wxString& Str,int Pos);
        static inline bool Match(const wxString& Str,int Pos,wxChar Ch);

        wxString m_AppFile;               ///< \brief Source file defining application
        wxArrayString m_LoadedResources;  ///< \brief List of automatically loaded resource files
        wxString m_MainResource;          ///< \brief Resource used by default
        bool m_CallInitAll;               ///< \brief True if wxXmlResource::Get()->InitAllHandlers() should be called while initialization
        bool m_CallInitAllNecessary;      ///< \brief True if should call wxXmlResource::Get()->InitAllHandlers() only when necessary
        wxsCodingLang m_AppLanguage;      ///< \brief Coding language for main app file

        friend class wxWidgetsGUIConfigPanel;
        friend class wxWidgetsGUIAppAdoptingDlg;
};

#endif
