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
         * \param ClassName name of resource class
         */
        void BindExternalResource(const wxString& FileName,const wxString& ClassName);

        /** \brief Function checking if this resource uses xrc files */
        virtual bool UsingXRC();

        /** \brief Function returning name of declaration file - same as header file */
        virtual wxString GetDeclarationFile() { return HFile; }

        /** \brief Function returning name of header file for this resource. */
        inline wxString GetHeaderFile() { return HFile; }

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

		/** \brief Notifying about selection change
		 *
		 * This function must be called when selection does change in any
		 * way. It notifies editor/resource browser about changed selection.
		 *
		 * \param ChangedItem item that has changed it's selection state (or one of
		 *        items when changing selection in more than one items). When using
		 *        NULL, one of currently selected items will be used as root selection.
		 */
		void SelectionChanged(wxsItem* ChangedItem);

		/** \brief Getting root selection item */
		inline wxsItem* GetRootSelection() { return RootSelection; }

		/** \brief Setting root selection item */
		inline void SetRootSelection(wxsItem* NewRootSelection) { RootSelection = NewRootSelection; }

		/** \brief Notifying about content change
		 *
		 * This function validates content, recreates preview and
		 * updates source code. It should be used in case of property change
		 * outide editor.
		 */
		void NotifyChange(wxsItem* Chaged);

        /** \brief Funnction regenerating all source code
         *  \note Only content really changed will mark files as modified
         */
        void RebuildCode();

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

        /** \brief Function generating code which will load this resource
         *         from xrc.
         *
         * It is used when generating source code.
         */
        virtual wxString BuildXrcLoadingCode() = 0;

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

        /** \brief Function searching for first selected item */
        void FindFirstSelection(wxsItem* Item);

        /** \brief Function generating code with declarations
         *
         * \param RootItem root item (it's declaration will be skipped)
         * \param Code string where declarations of local variables will be added
         * \param GlobalCode string where declarations of global variables will be added
         * \return true when there's at least one local declaration, false if there's none
         *         (no matter what's number of global declarations)
         */
        bool BuildDeclarations(wxsItem* RootItem,wxString& Code,wxString& GlobalCode);

        /** \brief Function collecting code connecting event handlers */
        void AddEventHandlers(wxsItem* RootItem,wxString& Code);

        /** \brief Function building array of identifiers used inside this resource */
        void BuildIdsArray(wxsItem* RootItem,wxArrayString& Array);

        /** \brief Function building array of declaration files used in this resource */
        void BuildDeclArrays(wxsItem* RootItem,wxArrayString& DeclHeaders,wxArrayString& DefHeaders);

        /** \brief Fetching pointers to items loaded from xrc file */
        void FetchXmlBuiltItems(wxsItem* RootItem,wxString& Code);

        /** \brief Misc function used to reparse file names into unix format (cross-platform) */
        inline wxString FixFileName(wxString FileName);

        wxString    ClassName;
        wxString    WxsFile;
        wxString    SrcFile;
        wxString    HFile;
        wxString    XrcFile;
        wxsItem*    RootItem;
        wxsItem*    RootSelection;
        wxWindow*   Preview;
        bool        Modified;
        long        BasePropsFilter;
};

#endif
