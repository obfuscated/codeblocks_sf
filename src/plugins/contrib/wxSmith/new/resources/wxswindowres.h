#ifndef WXSWINDOWRES_H
#define WXSWINDOWRES_H

#include "../wxsresource.h"

class wxsItem;

/** \brief Base class for all window resources (Dialog, Frame and Panel).
 *
 * This resource canwork in threemodes:
 *  \li Editing resource which generates complete source code
 *  \li Editing resource which generates xrc file and source code loading it
 *  \li Editing xrc file directly
 *
 * Second ant third mode are using xrc file as base, but in second mode,
 * additional informations (like event handlers) are stored in separate file.
 * In first mode, structure of resource and additional info is merged
 * in one source file.
 */
class wxsWindowRes : public wxsResource
{
	public:

        /** \brief Ctor */
		wxsWindowRes(wxsProject* Project);

		/** \brief Dctor */
		virtual ~wxsWindowRes();

		/** \brief Getting resource name
		 *
         * In case of window resources, name of resource is equivalent ti name
         * of class which holds it
         */
		virtual const wxString& GetResourceName();

		/** \brief Getting main window item */
		inline wxsItem* GetRootItem() { return RootItem; }

		/** \brief Building entry in tree browser */
		virtual void BuildTree(wxTreeCtrl* Tree,wxTreeItemId ParentId);

        /** \brief Function loading configuration of this resource from
         *         given xml element
         */
        virtual bool LoadConfiguration(TiXmlElement* Element);

        /** \brief Generating new resource with given configuration
          *
          * \warning Does not check if new files do exist
          */
        bool CreateNewResource(TiXmlElement* Element);

        /** \brief Function storing configuration of this resource to given
         *         xml element
         */
        virtual bool SaveConfiguration(TiXmlElement* Element);

        /** \brief Function binfing external resource for this class
         * \param FileName name of xrc file, MUST be absolute path
         */
        void BindExternalResource(const wxString& FileName,const wxString& ClassName);

        /** \brief Function checking if this resource uses xrc files */
        virtual bool UsingXRC();

        /** \brief Function returning name of header file for this resource.
         *
         * This header file should be added into list of includes to allow
         * using resource
         */
        virtual wxString GetHeaderFile() { return HFile; }

		/** \brief Getting name of class implementing this dialog */
		inline const wxString& GetClassName() { return ClassName; }

		/** \brief Getting name of internal wxsmith's file containing structire
		 *         of resource. The file name is relative to wxSmith internal
         *         directory.
         */
		inline const wxString& GetWxsFile() { return WxsFile; }

		/** \brief Getting name of source file implementing dialog
		 *         File name is relative to path of project's .cbp file
		 */
		inline const wxString& GetSourceFile() { return SrcFile; }

		/** \brief Getting name of xrc file
		 *         File name is relative to project's .cbp file or is global
         *         path when in File edit mode
		 */
		inline const wxString& GetXrcFile() { return XrcFile; }

        /** \brief This function should show preview in modal window */
        void ShowPreview();

        /** \brief This function should hide current preview */
        void HidePreview();

        /** \brief This function returns true if there's current preview */
        bool IsPreview();

        /** \brief Function notifying that preview has been hidden
         *
         * This function may be used from previewing window when it's
         * closing to notify that there's no more valid preview
         */
        void NotifyPreviewClosed();

        /** \brief Function getting base properties filter
         *
         * This filter consist of wxsFLFile, wxsFLSource and wxsFLMixed flags.
         * One of these flags is always on depending on current resource
         * configuration
         */
        inline long GetBasePropsFilter() { return BasePropsFilter; }

        /** \brief Function loading resource using current settings */
        bool LoadResource();

        /** \brief Function saving resource using current settings
         *
         * This function checks if Modified flag is set.
         * Resource will be saved only when it's set to true.
         */
        void SaveResource();

		/** Returns true if resource is modified, false otherwise */
		inline bool GetModified() { return Modified; }

		/** \brief Set the resources's modification state to \c modified.
		 *
		 *  Modification state should be set only when there's
		 *  open editor for this resource.
		 */
		void SetModified(bool modified = true);

		/** \brief Function generating xml string from this resource
		 *
		 * Because resource is loaded only when there's editor opened,
		 * this function will return empty string when it's not opened.
		 */
		wxString GetXmlData();

		/** \brief Function reloading resource using given string
		 *
		 * Because resource is loaded only when there's editor opened,
		 * this function will not update resource structure when
		 * editor is closed.
		 *
		 * Additionally, resource structure is checked to forbid
		 * loading invalid resources.
		 */
		bool SetXmlData(const wxString& Data);

        /** \brief Getting dialog type */
		virtual wxString GetType();

		/** \brief Checking if file is used by this resource */
		virtual bool UsingFile(const wxString& FileName);

    protected:

        /** \brief Function building root item */
        virtual wxsItem* BuildRootItem() = 0;

        /** \brief Function returning name of root item's class */
        virtual wxString GetRootItemClass() = 0;

        /** \brief Function generating preview for this resouce
         *
         * This preview is not preview used inside editor area but
         * preview used to show how resource will directly look like
         */
        virtual wxWindow* BuildPreview() = 0;

    private:

        WX_DECLARE_STRING_HASH_MAP(wxsItem*,IdToItemMapT);

        /** \brief Function which should create editor window. */
        virtual wxsEditor* CreateEditor();

        /** \brief Function notifying that editor has just been closed */
        virtual void EditorClosed();

        /** \brief Helper function saving extra data for given item.
         *
         * This function is used in mixed edit mode only.
         */
        void SaveChildrenExtra(wxsItem* Item,TiXmlElement* Element);

        /** \brief Function collecting enteries of map id_name -> wxsItem* */
        void CollectIdMap(IdToItemMapT& Map,wxsItem* Item,bool WithRoot=true);

//        /** \brief This will be used to notify about change of resource data.
//         *
//         * This function performs check of identifiers/variable names and
//         * rebuilds source code.
//         */
//        void NotifyChange();
//
//
//
//        /** \brief Funnction regenerating all source code */
//        void RebuildCode();
//
//
//        /** \brief Setting default variable names and identifiers for widgets with empty ones */
//        void UpdateWidgetsVarNameId();
//
//		/** \brief Function refreshing tree node associated with this resource */
//		void RefreshResourceTree();
//
//		/** Building resource tree */
//		void BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd,bool NoWidgets = false);
//
//		/** Changing root widget */
//		bool ChangeRootWidget(wxsWidget* NewRoot,bool DeletePrevious=true);
//
//        /** Action when selecting this resource */
//        virtual void OnSelect();
//
//    protected:
//
//        /** Creating editor object */
//        virtual wxsEditor* CreateEditor();
//
//        /** Notifying that editor has just closed
//         *
//         * In this case, resource is reloaded from wxs file (all changes
//         * should be now saved when closing editor)
//         */
//        virtual void EditorClosed();
//
//        /** Function initializing this class - it must be called in constructor
//         *  of derived class since virtual functinos can be used from top
//         *  constrructor only */
//        void Initialize();
//
//        /** Function showing preview for this resource */
//        virtual void ShowResource(wxXmlResource& Res) = 0;
//
//        /** Getting string added as constructor code for base widget */
//        virtual wxString GetConstructor() = 0;
//
//        /** Helper function giving name of resource from current window type */
//        virtual const wxChar* GetWidgetClass(bool UseRes = false) = 0;
//
//        /** Function generating code loading this resource from xrc file */
//        virtual wxString GetXrcLoadingCode() = 0;
//
//        /** Pointer to window with current preview */
//        wxWindow* Preview;
//
//	private:
//
//        /** Structure used for comparing strings */
//        struct ltstr {  bool operator()(const wxChar* s1, const wxChar* s2) const { return wxStrcmp(s1, s2) < 0; } };
//
//        /** Map string->widget used when validating variable names and identifiers */
//        typedef std::map<const wxChar*,wxsWidget*,ltstr> StrMap;
//        typedef StrMap::iterator StrMapI;
//
//        /** Creating xml tree for current widget */
//        TiXmlDocument* GenerateXml();
//
//        /** Adding declaration codes for locally stored widgets */
//        void AddDeclarationsReq(wxsWidget* Widget,wxString& LocalCode,wxString& GlobalCode,bool& WasLocal);
//
//        /** Function used internally by SetNewWidgetsIdVarName */
//        void UpdateWidgetsVarNameIdReq(StrMap& NamesMap,StrMap& IdsMap,wxsWidget* Widget);
//
//        /** Function craeting set of used names and ids for this resource */
//        void CreateSetsReq(StrMap& NamesMap, StrMap& IdsMap, wxsWidget* Widget, wxsWidget* Without = NULL);
//
//        /** Function checking and correcting base params for one widget */
//        bool CorrectOneWidget(StrMap& NamesMap,StrMap& IdsMap,wxsWidget* Changed,bool Correct);
//
//        /** Helper function used inside CkeckBaseProperties function */
//        bool CheckBasePropertiesReq(wxsWidget* Widget,bool Correct,StrMap& NamesMap,StrMap& IdsMap);
//
//        /** Function building array of identifiers */
//        void BuildIdsArray(wxsWidget* Widget,wxArrayString& Array);
//
//        /** Function building array of header files */
//        void BuildHeadersArray(wxsWidget* Widget,wxArrayString& Array);
//
//        /** Fuunction collecting code for event table for given widget */
//        static void CollectEventTableEnteries(wxString& Code,wxsWidget* Widget);
//
//        /** Function generating code fetching controls from xrc structure */
//        static void GenXrcFetchingCode(wxString& Code,wxsWidget* Widget);
//
        wxString    ClassName;
        wxString    WxsFile;
        wxString    SrcFile;
        wxString    HFile;
        wxString    XrcFile;
        wxsItem*    RootItem;
        wxWindow*   Preview;
        bool        Modified;
        long        BasePropsFilter;
};

#endif
