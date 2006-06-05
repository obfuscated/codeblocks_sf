#ifndef WXSPROJECT_H
#define WXSPROJECT_H



#include "wxsglobals.h"

/******************************************************************************/
/* Predefines                                                                 */
/******************************************************************************/

class wxSmith;
class wxsWidget;
class wxsWindowEditor;
class wxsResource;
class wxsDialogRes;
class wxsFrameRes;
class wxsPanelRes;
class wxsWindowRes;

/******************************************************************************/

#include <cbproject.h>
#include <vector>
#include <tinyxml/tinyxml.h>
#include "wxsresourcetree.h"

/** Directory relative to project's main file where wxSmith internal
 *  data should be stored.
 */
static const wxString wxSmithSubDirectory(_T("wxsmith"));

/** Name of main configuration for wxSmith */
static const wxString wxSmithMainConfigFile(_T("wxsmith.cfg"));

/** This class integrates current project with wxsmith.
 *
 * This class is responsible for:
 *  - adding wxSmith abilities into wxisting project
 *  - removing wxSmith elements from project
 *  - parsing project files (searching for widgest)
 *  - anything needed ? add here
 *  .
 */
class wxsProject
{

	public:

        /* Some strructures which could be used publically */

		wxsProject();
		virtual ~wxsProject();

        /** Type defining current state of integration with C::B project */
        enum IntegrationState
        {
            NotBinded = 0,  ///< This wxsProjecyt has not been integrated with C::B project
            NotWxsProject,  ///< Associated C::B project is not using wxSmith abilities
            Integrated      ///< Associated C::B project is fully integrated with wxSmith project
        };

        /** Getting current type of integration with C::B project */
        inline IntegrationState GetIntegration() { return Integration; }

        /** Connecting to existing project */
        virtual IntegrationState BindProject(cbProject* Project,TiXmlElement* Config);

//        /** Forcing project to save it's main configuration file */
//        virtual void SaveProject();

        /** Getting C::B project */
        inline cbProject* GetCBProject() { return Project; }

        /** Generating full name of internal wxSmith file */
        virtual wxString GetInternalFileName(const wxString& FileName);

        /** Getting full name of project's file */
        virtual wxString GetProjectFileName(const wxString& FileName);

        /** Adding wxSmith configuration to this project */
        virtual bool AddSmithConfig(bool Silent=false);

        /** Searching for resource with given name */
        wxsResource* FindResource(const wxString& Name);

        /** Adding previously created dialog resource */
        void AddDialog(wxsDialogRes* Dialog);

        /** Adding previously created frame resource */
        void AddFrame(wxsFrameRes* Frame);

        /** Adding previously created panel resource */
        void AddPanel(wxsPanelRes* Panel);

        /** Spreading eventg to all resource editors opened inside this project */
        void SendEventToEditors(wxEvent& event);

        /** Deleting dialog resource from project */
        void DeleteDialog(wxsDialogRes* Resource);

        /** Deleting frame resource from project */
        void DeleteFrame(wxsFrameRes* Resource);

        /** Deleting panel resource from project */
        void DeletePanel(wxsPanelRes* Resource);

        /** Getting modified state */
//        bool GetModified() { return Modified; }

        /** Displaying configuration dialog */
        void Configure();

        /** Function returing source file where app is declared */
        inline const wxString& GetAppSourceFile() { return AppFile; }

        /** Function setting soure file where app is declared */
        bool SetAppSourceFile(const wxString& NewAppFile);

        /** Function returning main resource name */
        inline const wxString& GetMainResource() { return MainResource; }

        /** Function setting main resource name */
        bool SetMainResource(const wxString& NewMainResource);

        /** Function returning value for InitAll flag */
        inline bool GetCallInitAll() { return CallInitAll; }

        /** Funtion returning valuie for InitAllNecessary flag */
        inline bool GetCallInitAllNecessary() { return CallInitAllNecessary; }

        /** Function setting values for InitAll and InitAllNecessary flags */
        void SetCallInitAll(bool NewInitAll,bool NewNecessary);

        /** Function returning array of automatically loaded resources */
        inline const wxArrayString& GetAutoLoadedResources() { return LoadedResources; }

        /** Function setting array of automatically loadeed resources */
        void SetAutoLoadedResources(const wxArrayString& Array);

        /** Function enumerating resources */
        void EnumerateResources(wxArrayString& Array,bool MainOnly=false);

        /** Function returning main project path */
        wxString GetProjectPath();

        /** Function returning path of directory where all wxsmith files are stored */
        wxString GetInternalPath();

        /** Function returning true if wxSmith manages application source for this project */
        bool IsAppManaged();

        /** Function returning true if specified app source file seems to be managed by wxSmith */
        bool IsAppSourceManager(const wxString& FileName);

        /** checling if given file is insided current project */
        bool CheckProjFileExists(const wxString& FileName);

        /** Storing wxSmith configuration to xml element */
        void XmlStore(TiXmlNode* Node);

        /** Changing modified state */
        void SetModified(bool modified);

        /** Function loading dialog resource from xml tree */
        void AddDialogResource(
            const wxString& FileName,
            const wxString& ClassName,
            const wxString& SourceName,
            const wxString& HeaderName,
            const wxString& XrcName);

        /** Function loading frame resource from xml tree */
        void AddFrameResource(
            const wxString& FileName,
            const wxString& ClassName,
            const wxString& SourceName,
            const wxString& HeaderName,
            const wxString& XrcName);

        /** Function loading panel resource from xml tree */
        void AddPanelResource(
            const wxString& FileName,
            const wxString& ClassName,
            const wxString& SourceName,
            const wxString& HeaderName,
            const wxString& XrcName);

        void RebuildTree();

    protected:

        /** Function loading all data from xml source */
        virtual bool LoadFromXml(TiXmlNode* MainNode);

        /** Function generating xml data */
        virtual TiXmlDocument* GenerateXml();


	private:

        /** Adding resourcee of given type */
        void AddWindowResource(
            const wxString& FileName,
            const wxString& ClassName,
            const wxString& SourceName,
            const wxString& HeaderName,
            const wxString& XrcName,
            const wxString& Type);

        /** Function building tree for resources in this project */
        void BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd);

        /** Function clearing project structures */
        inline void Clear();


        /** Rebuilding application code */
        void RebuildAppCode();

        IntegrationState Integration;   ///< Current integration state
        wxFileName ProjectPath;         ///< Base Directory of C::B project (where project file is stored)
        wxFileName WorkingPath;         ///< Directory where wxSmith's private data will be stored
        cbProject* Project;             ///< Project associated with project
        wxTreeItemId TreeItem;          ///< Tree item where project's resources are kept
        wxTreeItemId DialogId;          ///< Tree item for dialog resourcecs
        wxTreeItemId FrameId;           ///< Tree item for frame resources
        wxTreeItemId PanelId;           ///< Tree item for panel resources
        wxString AppFile;               ///< Source file defining application
        wxArrayString LoadedResources;  ///< List of automatically loaded resource files
        wxString MainResource;          ///< Resource used by default
        bool CallInitAll;               ///< True if wxXmlResource::Get()->InitAllHandlers() should be called while initialization
        bool CallInitAllNecessary;      ///< True if should call wxXmlResource::Get()->InitAllHandlers() only when necessary

        /* Resources */

        typedef std::vector<wxsDialogRes*> DialogListT;
        typedef std::vector<wxsFrameRes*>  FrameListT;
        typedef std::vector<wxsPanelRes*>  PanelListT;

        typedef DialogListT::iterator DialogListI;
        typedef FrameListT ::iterator FrameListI;
        typedef PanelListT ::iterator PanelListI;


        DialogListT Dialogs;
        FrameListT Frames;
        PanelListT Panels;

        bool DuringClear;               ///< Set to true when inside Clear call
        bool Modified;                  ///< Set to true when there was any change inside wxSmith project

        friend class wxsWindowRes;
        friend class wxSmithMime;
};


#endif // WXSPROJECT_H
