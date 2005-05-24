#ifndef WXSPROJECT_H
#define WXSPROJECT_H

/******************************************************************************/
/* Predefines                                                                 */
/******************************************************************************/

class wxSmith;
class wxsWidget;
class wxsWindowEditor;
class wxsDialogRes;
class wxsFrameRes;
class wxsPanelRes;

/******************************************************************************/

#include <cbproject.h>
#include <vector>
#include <tinyxml/tinyxml.h>


/** Directory relative to project's main file where wxSmith internal
 *  data should be stored.
 */
static const wxString wxSmithSubDirectory(wxT("wxsmith"));

/** Name of main configuration for wxSmith */
static const wxString wxSmithMainConfigFile(wxT("wxsmith.cfg"));

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
        
		wxsProject(wxSmith* Plugin);
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
        virtual IntegrationState BindProject(cbProject* Project);
        
        /** Forcing project to save it's main configuration file */
        virtual void SaveProject();
        
        /** Getting plugin associated with this project */
        inline wxSmith* GetPlugin() { return Plugin; }
        
        /** Getting C::B project */
        inline cbProject* GetCBProject() { return Project; }
		
    protected:
        
        /** Function loading all data from xml source */
        virtual bool LoadFromXml(TiXmlNode* MainNode);
        
        /** Function generating xml data */
        virtual TiXmlDocument* GenerateXml();
        
        /** Function loading dialog resource from xml tree */
        void AddDialogResource(
            const char* FileName,
            const char* ClassName,
            const char* SourceName,
            const char* HeaderName);
        
        /** Function loading frame resource from xml tree */
        void AddFrameResource(
            const char* FileName,
            const char* ClassName,
            const char* SourceName,
            const char* HeaderName);
        
        /** Function loading panel resource from xml tree */
        void AddPanelResource(
            const char* FileName,
            const char* ClassName,
            const char* SourceName,
            const char* HeaderName);
        
	private:
	
        /** Deleting dialog resource from project
         *
         * This should be called from Resource's destructor
         */
        void DeleteDialog(wxsDialogRes* Resource);
        
        /** Function building tree for resources in this project */
        void BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd);        
        
        /** Diagnostic function dumping structure of given xml tree to wxTreeCtrl */
        static void DumpXml(const TiXmlNode* Elem, wxTreeCtrl* Tree,wxTreeItemId id);

        /** checling if given file is insided current project */
        bool CheckProjFileExists(const char* FileName);

        /** Function clearing project structures */
        inline void Clear();            

        
        IntegrationState Integration;   ///< Current integration state
        wxFileName ProjectPath;         ///< Base Directory of C::B project (where project filr is stored)
        wxFileName WorkingPath;         ///< Directory where wxSmith's private data will be stored
        cbProject* Project;             ///< Project associated with project
        wxSmith* Plugin;                ///< Current wxSmith plugin
        wxTreeItemId TreeItem;          ///< Tree item where project's resources are kept
        
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
        
        bool DuringClear;               ///< Set to true wneh inside Clear call
        
        friend class wxsDialogRes;
};


/** Data used by resource tree */

class wxsResourceTreeData: public wxTreeItemData
{
    public: 
        
        wxsResourceTreeData(wxsWidget* _Widget):    Type(tWidget) { Widget = _Widget; }
        wxsResourceTreeData(wxsDialogRes* _Dialog): Type(tDialog) { Dialog = _Dialog; }
        wxsResourceTreeData(wxsFrameRes* _Frame):   Type(tFrame)  { Frame  = _Frame;  }
        wxsResourceTreeData(wxsPanelRes* _Panel):   Type(tPanel)  { Panel  = _Panel;  }
        
        enum TypeT
        {
            None,
            tWidget,
            tDialog,
            tFrame,
            tPanel
        };
        
        TypeT Type;
  
        union
        {
            wxsWidget* Widget;
            wxsDialogRes* Dialog;
            wxsFrameRes* Frame;
            wxsPanelRes* Panel;
        };
};


#endif // WXSPROJECT_H
