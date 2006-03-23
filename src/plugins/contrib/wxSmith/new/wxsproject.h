#ifndef WXSPROJECT_H
#define WXSPROJECT_H

#include <cbproject.h>
#include <tinyxml/tinyxml.h>
#include "wxsprojectconfig.h"

// Forward declarations

class wxSmith;
class wxsResource;

/** \brief Name of directory relative to project's main file where wxSmith internal
 *  data should be stored.
 */
static const wxString wxSmithSubDirectory(_T("wxsmith"));

/** \brief Name of main configuration for wxSmith */
static const wxString wxSmithMainConfigFile(_T("wxsmith.cfg"));

/** \brief This class integrates current project with wxsmith. */
class wxsProject
{
	public:

        /** \brief Type defining current state of integration with C::B project */
        enum IntegrationState
        {
            NotBinded = 0,  ///< \brief This wxsProjecyt has not been integrated with C::B project
            NotWxsProject,  ///< \brief Associated C::B project is not using wxSmith abilities
            Integrated      ///< \brief Associated C::B project is fully integrated with wxSmith project
        };

        /** \brief Ctor */
		wxsProject(cbProject* Project);

		/** \brief Dctor */
		~wxsProject();

        /** \brief Getting current type of integration with C::B project */
        inline IntegrationState GetIntegration() { return Integration; }

        /** \brief Function saving project's main configuration file if necessary */
        void SaveProject();

        /** \brief Getting C::B project */
        inline cbProject* GetCBProject() { return Project; }

        /** \brief Generating full name of internal wxSmith file */
        wxString GetInternalFileName(const wxString& FileName);

        /** \brief Getting full name of project's file */
        wxString GetProjectFileName(const wxString& FileName);

        /** \brief Adding wxSmith configuration to this project */
        bool AddSmithConfig();

        /** \brief Searching for resource with given name */
        wxsResource* FindResource(const wxString& Name);

        /** \brief Getting modified state */
        bool GetModified() { return Modified; }

        /** \brief Displaying configuration dialog */
        void Configure();

        /** \brief Getting configuration for this project */
        inline wxsProjectConfig& GetConfig() { return Config; }

        /** \brief Function enumerating resources */
        void EnumerateResources(wxArrayString& Array,bool MainOnly=false);

        /** \brief Function returning main project path */
        wxString GetProjectPath();

        /** \brief Function returning path of directory where all wxsmith files are stored */
        wxString GetInternalPath();

        /** \brief Function returning true if wxSmith manages application source for this project */
        bool IsAppManaged();

        /** \brief Function returning true if specified app source file seems to be managed by wxSmith */
        bool IsAppSourceManaged(const wxString& FileName);

        /** \brief Checking if given file is inside current project */
        bool CheckProjFileExists(const wxString& FileName);

        /** \brief Adding external resource */
        bool AddResource(wxsResource* Resource);

        /** \brief Function trying to open editor for project with given file
         * \param FileName name of file used by this resource
         *
         * This function should check if resources are using this file
         * and if yes, it should open valid editor for it. Given FileName
         * can be ANY file used by resource to make it open.
         */
        bool TryOpenEditor(const wxString& FileName);

	private:

        /** \brief Function loading all data from xml source */
        bool LoadFromXml(TiXmlNode* MainNode);

        /** \brief Function generating xml data */
        TiXmlDocument* GenerateXml();

        /** \brief Function building tree for resources in this project */
        void BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd);

        /** \brief Rebuilding application code */
        void RebuildAppCode();

        IntegrationState Integration;   ///< \brief Current integration state
        wxFileName ProjectPath;         ///< \brief Base Directory of C::B project (where project file is stored)
        wxFileName WorkingPath;         ///< \brief Directory where wxSmith's private data will be stored
        cbProject* Project;             ///< \brief Project associated with project
        wxTreeItemId TreeItem;          ///< \brief Tree item where project's resources are kept
        wxTreeItemId DialogId;          ///< \brief Tree item for dialog resourcecs
        wxTreeItemId FrameId;           ///< \brief Tree item for frame resources
        wxTreeItemId PanelId;           ///< \brief Tree item for panel resources

        wxsProjectConfig Config;        ///< \brief Configuration of project

        WX_DEFINE_ARRAY(wxsResource*,ResourcesT);

        ResourcesT Resources;           ///< \brief Array of resources
        bool Modified;                  ///< \brief Set to true when there was any change inside wxSmith project

        friend class wxsWindowRes;
};

#endif
