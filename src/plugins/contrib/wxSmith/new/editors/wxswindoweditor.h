#ifndef WXSWINDOWEDITOR_H
#define WXSWINDOWEDITOR_H

#include "../wxseditor.h"
#include "../resources/wxswindowres.h"
#include "wxswinundobuffer.h"
#include "wxscorrector.h"
#include <wx/hashset.h>
#include <wx/scrolwin.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/bmpbuttn.h>

class wxsWindowEditorContent;

/** \brief This is root class for editing wxWidgets window resources
 */
class wxsWindowEditor : public wxsEditor
{
	public:

        /** \brief Ctor */
		wxsWindowEditor(wxWindow* parent,wxsResource* Resource);

        /** \brief Dctor */
		virtual ~wxsWindowEditor();

		/** \brief Saving resource */
		virtual bool Save();

		/** \brief Returns true if resource is modified, false otherwise */
		virtual bool GetModified();

		/** \brief Set the resources's modification state to \c modified. */
		virtual void SetModified(bool modified);

		/** \brief Checking if can Undo */
		virtual bool CanUndo();

		/** \brief Ckecing if can Redo */
		virtual bool CanRedo();

		/** \brief Checking if we can cut */
		virtual bool HasSelection();

		/** \brief Checking if we can paste */
		virtual bool CanPaste();

		/** \brief Undoing */
		virtual void Undo();

		/** \brief Redoing */
		virtual void Redo();

		/** \brief Cutting */
		virtual void Cut();

		/** \brief Copying */
		virtual void Copy();

		/** \brief Pasting */
		virtual void Paste();

		/** \brief Reloading images in all editors */
		static void ReloadImages();

        /** \brief Function notifying that selection has changed
         *
         * This function is called from wxsWindowRes class after each
         * selection change.
         */
        void SelectionChanged();


        /** \brief Function notifying that properties of given item has changed
         *  \note You should call wxsItem::NotifyPropertyChange rather than this
         *        function.
         */
        void NotifyChange(wxsItem* Changed);

    protected:

        /** \brief Getting wxsWindowRes pointer to currently edited resource */
        inline wxsWindowRes* GetWinRes() { return (wxsWindowRes*)GetResource(); }

        /** \brief Getting project class of current resource file */
        inline wxsProject* GetProject() { return GetResource()->GetProject(); }

        /** \brief Getting current selection (main selected item) */
        inline wxsItem* GetCurrentSelection() { return GetWinRes()->GetRootSelection(); }

        /** \brief Helper function for fetching root item */
        inline wxsItem* RootItem() { return GetWinRes()->GetRootItem(); }

	private:

        WX_DECLARE_HASH_SET(wxsWindowEditor*,wxPointerHash,wxPointerEqual,WindowSet);
        WX_DEFINE_ARRAY(wxsItem*,ItemArray);

        wxsWindowEditorContent* m_Content;  ///< \brief Window with content area
        wxNotebook* m_WidgetsSet;           ///< \brief Notebook with all widgets inside
        wxBoxSizer* m_VertSizer;            ///< \brief Root sizer of this editor
        wxBoxSizer* m_HorizSizer;           ///< \brief Horizontal sizer managing items below palette
        wxBoxSizer* m_QPSizer;              ///< \brief Sizer for quick properties
        wxBoxSizer* m_OpsSizer;             ///< \brief Sizer for operations pane
        wxScrolledWindow* m_QPArea;         ///< \brief Scrolled window containing all QuickProps sturr
        wxBitmapButton* m_InsIntoBtn;
        wxBitmapButton* m_InsBeforeBtn;
        wxBitmapButton* m_InsAfterBtn;
        wxBitmapButton* m_DelBtn;
        wxBitmapButton* m_PreviewBtn;
        wxBitmapButton* m_QuickPanelBtn;
        wxWindow* m_TopPreview;             ///< \brief Top window of preview
        int m_InsType;                      ///< \brief Current insertion type
        int m_InsTypeMask;                  ///< \brief Current insertion type mask
        bool m_QuickPropsOpen;              ///< \brief Set to true if quick properties panel is opened
        bool m_DontStoreUndo;               ///< \brief When set to true, FinishChange() won't create new undo entry, usefull in Undo/Redo operations

        static wxImage m_InsIntoImg;
        static wxImage m_InsBeforeImg;
        static wxImage m_InsAfterImg;
        static wxImage m_DelImg;
        static wxImage m_PreviewImg;
        static wxImage m_QuickPropsImgOpen;
        static wxImage m_QuickPropsImgClose;
        static wxImage m_SelectedImg;
        static WindowSet m_AllEditors;
        static bool m_ImagesLoaded;

        static const int itBefore = 0x01;
        static const int itAfter  = 0x02;
        static const int itInto   = 0x04;

        wxsWinUndoBuffer* m_UndoBuff;       ///< \brief Undo buffer
        wxsCorrector* m_Corrector;          ///< \brief Data corrector

		/* Event handlers */
        void OnMouseClick(wxMouseEvent& event);
        void OnButton(wxCommandEvent& event);
        void OnInsInto(wxCommandEvent& event);
        void OnInsBefore(wxCommandEvent& event);
        void OnInsAfter(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);
        void OnQuickProps(wxCommandEvent& event);

        /** \brief Initializing images */
        static void InitializeImages();

        /** \brief Building palette */
        void BuildPalette(wxNotebook* Palette);

        /** \brief Selecting one item and unselecting all others
         *
         * This function does not update screen nor resource tree but
         * states inside wxsItem classes only.
         */
        void SelectOneItem(wxsItem* ItemToSelect);


        /** \brief Function inserting new widget */
        void InsertRequest(const wxString& Name);

        /** \brief Function adding item before given one */
        bool InsertBefore(wxsItem* New,wxsItem* Ref);

        /** \brief Function adding item after given one */
        bool InsertAfter(wxsItem* New,wxsItem* Ref);

        /** \brief Function adding item into given one */
        bool InsertInto(wxsItem* New,wxsItem* Ref);

        /** \brief Inisializing resource change */
        virtual void OnChangeInit();

        /** \brief Finalizing resource change */
        virtual void OnChangeFinish();

        /** \brief Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);

        /** \brief Setting new insertion type */
        void SetInsertionType(int Type);

        /** \brief Rebuilding pictures on insertion type buttons */
        void RebuildInsTypeIcons();

        /** \brief Rebuilding picture on Quick Props button */
        void RebuildQuickPropsIcon();

        /** \brief Rebuilding all icons */
        void RebuildIcons();

        /** \brief Building icon for one button */
        void BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,int ButtonType);

        /** \brief Opening or closiung Quick Props panel */
        void ToggleQuickPropsPanel(bool Open);

        /** \brief Refreshing content of Quick Props panel */
        void RebuildQuickProps(wxsItem* Selection);

        /** \brief Rebuilding preview (and updating selection inside preview) */
        void RebuildPreview();

        /** \brief Rebuinding content of resource browser for this resource */
        void RebuildTree();

        /** \brief Updating things related to selected item */
        void UpdateSelection();

        /** \brief Storing state of resource tree required to rebuind it's new content */
        void StoreTreeState();


        /** \brief Function checking if given item or any of it's children is selected */
        bool HasSelection(wxsItem* Item);

        /** \brief Getting selection skipping child items when parent is selected */
        static void GetSelectionNoChildren(ItemArray& Array,wxsItem* Item);

        /** \brief Destroying all selected items
         * \note This function calls BeginChange() and EndChange()
         *       so it shouldn't be used as some internal function
         *       manipulating data, it additionally changes selection.
         */
        void KillSelection();

        /** \brief Sestroying selected items starting from item in argument
         *
         * This function scans recursively items and their children starting from
         * Item parameter. If item is selected, it's killed.
         * \param Item initial item to be searched for selected items
         * \note This function does NOT call BeginChange and EndChange() so
         *       it may be used as internal function manipulating data.
         * \warning To ease operations in parameterless KillSelection, this function
         *          won't delete Item if it's selected (this avoid deleting root item
         *          of resource)
         */
        void KillSelection(wxsItem* Item);

        /** \brief Getting item which will be used as reference item when adding new
         *         items
         */
        wxsItem* GetReferenceItem(int& InsertionType);

        friend class wxsWindowEditorContent;

        DECLARE_EVENT_TABLE()
};

#endif
