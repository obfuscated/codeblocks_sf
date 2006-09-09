#ifndef WXSPROJECT_H
#define WXSPROJECT_H

#include <cbproject.h>
#include <tinyxml/tinyxml.h>
#include "wxsgui.h"
#include "wxsresourcetree.h"

class wxSmith;
class wxsResource;

/** \brief This class integrates current project with wxsmith. */
class wxsProject
{
	public:

        /** \brief Ctor */
		wxsProject(cbProject* Project);

		/** \brief Dctor */
		~wxsProject();

        /** \brief Getting C::B project */
        inline cbProject* GetCBProject() { return m_Project; }

        /** \brief Reading configuration from given xml node and creating resource classes
         * \note To add backward compatibility, if there's no valid configuration inside
         *       cbp file, this function looks for outdated wxsmith.cfg file inside internal
         *       directory and loads configuration from that file
         */
        void ReadConfiguration(TiXmlElement* element);

        /** \brief Storing configuration to given xml node */
        void WriteConfiguration(TiXmlElement* element);

        /** \brief Searching for resource with given name */
        wxsResource* FindResource(const wxString& Name);

        /** \brief Displaying configuration dialog */
        void Configure();

        /** \brief Function returning main project path */
        wxString GetProjectPath();

        /** \brief Function returning path of directory where all wxsmith files are stored */
        wxString GetInternalPath();

        /** \brief Adding new resource
         *
         * This function will modify CBP project's modify state
         */
        bool AddResource(wxsResource* Resource);

        /** \brief Function checking if given file can be opened from this project
         * \param FileName name of file used by this resource
         *
         * This function is used by MIME plugin to check if specified file
         * is used inside any of resources inside this project. If true, editor
         * can be opened for this resource.
         */
        bool CanOpenEditor(const wxString& FileName);

        /** \brief Function trying to open editor for project with given file
         * \param FileName name of file used by this resource
         *
         * This function should check if resources are using given file
         * and if yes, it should open valid editor for it.
         */
        bool TryOpenEditor(const wxString& FileName);

        /** \brief Getting current wxsGUI item */
        inline wxsGUI* GetGUI() { return m_GUI; }

        /** \brief Changing GUI item
         *
         * wxsProject class become owner of new wxsGUI item and will
         * delete if if it won't be longer used (when will change to
         * other wxsGUI or while destructing wxsProject class).
         */
        void SetGUI(wxsGUI* NewGUI);

	private:

        WX_DEFINE_ARRAY(wxsResource*,ResourcesT);
        WX_DECLARE_STRING_HASH_MAP(wxsResourceItemId,ResBrowserIdsT);
        typedef ResBrowserIdsT::iterator ResBrowserIdsI;

        wxString m_ProjectPath;             ///< \brief Base Directory of C::B project (where project file is stored)
        wxString m_WorkingPath;             ///< \brief Directory where wxSmith's private data will be stored
        cbProject* m_Project;               ///< \brief Project associated with project
        wxsResourceItemId m_TreeItem;       ///< \brief Tree item where project's resources are kept
//        wxsProjectConfig m_Config;          ///< \brief Configuration of project
        ResourcesT m_Resources;             ///< \brief Array of resources
        ResBrowserIdsT m_ResBrowserIds;     ///< \brief wxTreeItemId-s for resource types
        wxsGUI* m_GUI;                      ///< \brief Main GUI item
        TiXmlElement m_UnknownConfig;
        TiXmlElement m_UnknownResources;

        /** \brief Function loading all data from xml source */
        void ReadConfigurationFromXml(TiXmlNode* MainNode);

//        /** \brief Function generating xml data */
//        TiXmlDocument* GenerateXml();
//
//        /** \brief Function building tree for resources in this project */
//        void BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd);

        /** \brief Getting tree item id in resource browser associated with given resource type */
        wxsResourceItemId GetResourceTypeTreeId(const wxString& Name);

//        /** \brief Clearing list of unknown resources
//         *
//         * This function is necessary to delete pointers kept inside list
//         */
//        void ClearUnknownConfigList();
//
//        /** \brief Rebuilding application code */
//        void RebuildAppCode();
//
//        /** \brief Loading configuration from wxsmith.cfg file */
//        void TryLoadConfigFromWxsmithCfg();
//
//        /** \brief Function returning true if specified app source file seems to be managed by wxSmith
//         * \param FileName name of file which should be checked relative to directory where cbp file is located
//         *
//         * This function actually looks for some blocks of automatically generated code
//         * and accepts all files which have all required blocks. It won't check if this
//         * file is really managed or not but if it could be managed.
//         */
//        bool IsAppSourceManaged(const wxString& FileName);
//
        friend class wxsWindowRes;
};

#endif
