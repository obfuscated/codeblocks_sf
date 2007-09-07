#ifndef WXSITEMRESDATA_H
#define WXSITEMRESDATA_H

#include "wxsitem.h"
#include "wxsparent.h"
#include "wxsitemres.h"
#include "wxscorrector.h"
#include "wxsitemundobuffer.h"
#include "wxsitemresdataobject.h"
#include "../wxsresourcetree.h"

class wxsTool;
class wxsItemEditor;

/** \brief Class holding data for item resources and operating on it */
class wxsItemResData
{
    public:

        /** \brief Ctor
         *
         * Constructor tries to load resource. Depending on what
         * parameters are empty strings, given type of file is assumed.
         *  - If Wxs, Src and Hdr file names are empty, it's only Xrc file.
         *  - If Xrc is empty but no other, it's resource not using Xrc file
         *  - If all file names are not empty, it's resoure using Xrc file
         *  - Other combinations are invalid.
         *
         * Parameters passed to constructor are GLOBAL paths (opposite to
         * wxsItemRes where names are relative to .cbp's directory).
         */
        wxsItemResData(
            const wxString& WxsFileName,
            const wxString& SrcFileName,
            const wxString& HdrFileName,
            const wxString& XrcFileName,
            const wxString& ClassName,
            const wxString& ClassType,
            wxsCodingLang   Language,
            wxsResourceItemId TreeId,
            wxsItemEditor*  Editor,
            wxsItemResFunctions* Functions
            );

        /** \brief Dctor
         *
         * \note When wxsItemResData is deleted and the
         *       data is in modified state (was not saved after modification),
         *       all changes are lost. Please call Save() to avoid this.
         */
        virtual ~wxsItemResData();

        /** \brief Loading resource
         *
         * This function (Re)loads resource from
         * files. Current resource content is moved into
         * undo buffer and resource data is replaced by the one
         * loaded from file.
         */
        bool Load();

        /** \brief Saving resource
         *
         * This function saves resource to wxs file
         * (note that Src / Hdr / Xrc files are not saved
         * because they're updated after each resource change)
         */
        bool Save();

        /** \brief Checking if resource was loaded properly
         *
         * This function may be used to check if resource was properly
         * loaded. It may be especially usual after checking if
         * constructor has loaded all data properly.
         */
        inline bool IsOk() { return m_IsOK; }

        /** \brief Function starting change of resource data
         *
         * This function Notifies that resource is going to change.
         * It locks data from other changes. Each resource
         * change must be finished with call to EndChange function.
         * Between BeginChagne and EndChange call there should not
         * be any call to gui item, so do not jump out of event
         * function before EndChange is called.
         *
         * Change of resource is any operation made on any wxsItem
         * class inside the resource (including the root class).
         */
        void BeginChange();

        /** \brief Function ending change of resource data
         *
         * This function must be paired with BeginChange() call.
         * it notifies that change of resource has been finished
         * and that it's good time to update all data on the screen
         * ans store new undo buffer entry.
         */
        void EndChange();

        /** \brief Checking if item has modified state */
        inline bool GetModified() { return m_Undo.IsModified(); }

        /** \brief Getting root item
         *  \return pointer to item on success, 0 when data wasn't initialized properly
         */
        inline wxsItem* GetRootItem() { return m_RootItem; }

        /** \brief Getting main item of selection */
        inline wxsItem* GetRootSelection() { return m_RootSelection; }

        /** \brief Getting properties filter based on current edit  mode */
        inline int GetPropertiesFilter() { return m_PropertiesFilter; }

        /** \brief Getting name of wxs file (global path) */
        inline const wxString& GetWxsFileName() { return m_SrcFileName; }

        /** \brief Getting name of source file (global path) */
        inline const wxString& GetSrcFileName() { return m_SrcFileName; }

        /** \brief Getting name of header file (global path) */
        inline const wxString& GetHdrFileName() { return m_HdrFileName; }

        /** \brief Getting name of XRC file (global path) */
        inline const wxString& GetXrcFileName() { return m_XrcFileName; }

        /** \brief Getting name of class of edited resource */
        inline const wxString& GetClassName() { return m_ClassName; }

        /** \brief Getting name class used as base for this resource (like wxDialog) */
        inline const wxString& GetClassType() { return m_ClassType; }

        /** \brief Getting language used in resource */
        inline wxsCodingLang GetLanguage() { return m_Language; }

        /** \brief Getting pointer to current source code string */
        inline wxString* GetCurentCode() { return m_CurrentCode; }

        /** \brief Searching for tree id in main resource tree for given item */
        inline bool GetTreeId(wxsResourceItemId& Id,wxsItem* Item) { return FindId(Id,Item); }

        /* ************************ */
        /*  Undo buffer operations  */
        /* ************************ */

		/** \brief Checking if can Undo */
		inline bool CanUndo() { return m_Undo.CanUndo(); }

		/** \brief Ckecing if can Redo */
		inline bool CanRedo() { return m_Undo.CanRedo(); }

		/** \brief Undoing */
		inline void Undo() { SetXmlData(m_Undo.Undo()); }

		/** \brief Redoing */
		inline void Redo() { SetXmlData(m_Undo.Redo()); }

        /** \brief Checking if current content is read only */
        inline bool IsReadOnly() { return m_ReadOnly; }

		/* ********************** */
		/*  Clipboard operations  */
		/* ********************** */

		/** \brief Checking if we can paste current clipboard content */
		bool CanPaste();

		/** \brief Cutting current selection to clipboard */
		void Cut();

		/** \brief Copying current selectin to clipboard */
		void Copy();

		/** \brief Pasting components from clipboard
		 *  \param Parent parent for new items
		 *  \param Position initial position for new items
		 */
		void Paste(wxsParent* Parent,int Position);


		/* ********************** */
		/*  Selection operations  */
		/* ********************** */

		/** \brief Checking of there's any selection */
		bool AnySelected();

		/** \brief Selecting one item */
		bool SelectItem(wxsItem* Item,bool UnselectOther);

		/** \brief Getting last selected item or 0 if there's no valid selection */
		inline wxsItem* GetLastSelection() { return m_RootSelection; }

		/* ******************* */
		/*  Operating on data  */
		/* ******************* */

		/** \brief Adding new item
		 *
		 * This function tries to add new item into
		 * given position. If it's possible, new item
		 * is added and true is returned. If it's
		 * impossible, new item is deleted internally
		 * and function returns false.
		 * \note To add tool item use InsertNewTool
		 * \param New new item
		 * \param Parent item which will become parent of New
		 * \param Position position inside Parent (if <0 or  out of range,
		 *        appending New at the end of Parent's children)
		 */
        bool InsertNew(wxsItem* New,wxsParent* Parent,int Position);

        /** \brief Adding new tool
         *
         * This function adds new tool into this resource.
         * Since tools require special threatment, they
         * need separate function.
         * \param Tool new tool
         * \return true on success, false otherwise
         */
        bool InsertNewTool(wxsTool* Tool);

        /** \brief Deleting all selected items */
        void DeleteSelected();

        /** \brief Getting number of tools */
        inline int GetToolsCount() { return (int)m_Tools.Count(); }

        /** \brief Getting tool at given index */
        inline wxsTool* GetTool(int Index) { return ((Index>=0)&&(Index<GetToolsCount())) ? m_Tools[Index] : 0; }

        /* ******************* */
        /*  Preview functions  */
        /* ******************* */

        /** \brief Checking if there's preview already */
        inline bool IsPreview() { return m_Preview!=0; }

        /** \brief Showing preview of current resource content */
        bool ShowPreview();

        /** \brief Closing window with current resource content */
        bool HidePreview();

        /** \brief Function notifying that preview has been closed externally */
        inline void NotifyPreviewClosed() { m_Preview = 0; }

        /* *********************** */
        /*  Notification handlers  */
        /* *********************** */

        /** \brief Notification of change of data
         *
         * This function is called from wxsItem objects
         * notifying about change of such item.
         */
        void NotifyChange(wxsItem* Changed);

    private:

        WX_DECLARE_STRING_HASH_MAP(TiXmlElement*,IdToXmlMapT);
        WX_DECLARE_HASH_MAP(wxsItem*,wxsResourceItemId,wxPointerHash,wxPointerEqual,ItemToIdMapT);
        WX_DEFINE_ARRAY(wxsTool*,ToolArrayT);

        /** \brief Generating string with xml data for this item
         *  \note used when creating undo enteries
         */
        wxString GetXmlData();

        /** \brief Restoring resource data from string with xml data */
        bool SetXmlData(const wxString& XmlData);

        /** \brief Rebuilding all files kept up-to-date after change in resource */
        void RebuildFiles();

        /** \brief Rebuilding sources for this resource */
        void RebuildSourceCode();

        /** \brief Rebuilding XRC file managed by this resource */
        bool RebuildXrcFile();

        // Various loading functinos
        bool SilentLoad();
        bool LoadInFileMode();
        bool LoadInMixedMode();
        bool LoadInSourceMode();
        void UpdateExtraDataReq(wxsItem* Item,IdToXmlMapT& Map);
        void RecreateRootItem();
        void LoadToolsReq(TiXmlElement* Node,bool IsXRC,bool IsExtra);

        // Various saving function
        bool SaveInFileMode();
        bool SaveInMixedMode();
        bool SaveInSourceMode();
        void SaveExtraDataReq(wxsItem* Item,TiXmlElement* Node);

        // Some misc functions
        inline void StoreUndo() { m_Undo.StoreChange(GetXmlData()); }
        bool ValidateRootSelection();
        bool ValidateRootSelectionReq(wxsItem* Item,wxsItem*& NewSelection);
        void CopyReq(wxsItem* Item,wxsItemResDataObject* Data);
        bool AnySelectedReq(wxsItem* Item);
        void StoreTreeExpandState();
        void StoreTreeExpandStateReq(wxsItem* Item);
        void RestoreTreeExpandAndSelectionState();
        void RestoreTreeExpandAndSelectionStateReq(wxsItem* Item);
        void DeleteSelectedReq(wxsItem* Item);
        void RebuildTree();
        void StoreTreeIds();
        void StoreTreeIdsReq(wxsItem* Item);
        bool FindId(wxsResourceItemId& Id,wxsItem* Item);

        // Functions used by RebuildSourceCode
        void BuildVariablesCode(wxsCodingLang Lang,wxString& LocalCode, wxString& GlobalCode);
        void BuildVariablesCodeReq(wxsCodingLang Lang,wxsItem* Item,wxString& LocalCode, wxString& GlobalCode);
        void BuildCreatingCode(wxsCodingLang Lang,wxString& Code);
        void BuildXrcLoadingCode(wxsCodingLang Lang,wxString& Code);
        void BuildXrcItemsFetchingCode(wxsCodingLang Lang,wxString& Code);
        void BuildXrcItemsFetchingCodeReq(wxsCodingLang Lang,wxsItem* Item,wxString& Code);
        void BuildEventHandlersCode(wxsCodingLang Lang,wxString& Code);
        void BuildEventHandlersCodeReq(wxsCodingLang Lang,wxsItem* Item,wxString& Code);
        void BuildIdentifiersCode(wxsCodingLang Lang,wxString& Code,wxString& IdInitCode);
        void BuildIdsArrayReq(wxsItem* Item,wxArrayString& Array);
        void BuildIncludesCode(wxsCodingLang Lang,wxString& LocalIncludes,wxString& GlobalIncludes);
        void BuildHeadersReq(wxsCodingLang Lang,wxsItem* Item,wxArrayString& LocalHeaders,wxArrayString& GlobalHeaders);

        // Wrappers to m_Functions functionality
        inline wxWindow* BuildExactPreview(wxWindow* Parent) { return m_Functions->OnBuildExactPreview(Parent,this); }

        wxString m_WxsFileName;
        wxString m_SrcFileName;
        wxString m_HdrFileName;
        wxString m_XrcFileName;
        wxString m_ClassName;
        wxString m_ClassType;
        wxsCodingLang m_Language;
        wxsResourceItemId m_TreeId;
        wxsResourceItemId m_ToolsId;
        bool m_ToolsNodeIsExpanded;
        ItemToIdMapT m_IdMap;
        wxsItemEditor* m_Editor;
        wxsItemResFunctions* m_Functions;

        wxsItem* m_RootItem;
        wxsItem* m_RootSelection;
        ToolArrayT m_Tools;
        long m_PropertiesFilter;

        wxWindow* m_Preview;

        wxsItemUndoBuffer m_Undo;
        wxsCorrector m_Corrector;

        bool m_IsOK;
        int m_LockCount;

        bool m_ReadOnly;

        wxString* m_CurrentCode;
};

#endif
