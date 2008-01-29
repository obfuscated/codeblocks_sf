/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

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

        /** \brief Getting coding language of application's source */
        inline wxsCodingLang GetLanguage() { return m_AppLanguage; }

        /** \brief Setting new coding language for applicatino's source */
        inline void SetLanguage(wxsCodingLang Language) { m_AppLanguage = Language; }

        /** \brief Getting application source file */
        inline const wxString& GetAppSourceFile() { return m_AppFile; }

        /** \brief Setting new application source file
         *  \note remember to call RebuildApplicationCode to regenerate sources
         *  \note This functin will also set valid language according to file's extension
         */
        void SetAppSourceFile(const wxString& NewAppFile);

        /** \brief Getting array of loaded resources */
        inline wxArrayString& GetLoadedResources() { return m_LoadedResources; }

        /** \brief Getting main resource name */
        inline const wxString& GetMainResourceName() { return m_MainResource; }

        /** \brief Setting new main resource
         *  \note remember to call RebuildApplicationCode to regenerate sources
         */
        inline void SetMainResourceName(const wxString& Resource) { m_MainResource = Resource; }

        /** \brief Getting Some Initialization config */
        inline void GetInitParams(bool& CallInitAll,bool& CallInitAllNecessary) { CallInitAll = m_CallInitAll; CallInitAllNecessary = m_CallInitAllNecessary; }

        /** \brief Setting initialization config */
        inline void SetInitParams(bool CallInitAll=true,bool CallInitAllNecessary=true) { m_CallInitAll=CallInitAll; m_CallInitAllNecessary = CallInitAllNecessary; }

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
