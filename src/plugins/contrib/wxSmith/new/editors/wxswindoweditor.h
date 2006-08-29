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

		/** \brief Function called when resource is goingto change
		 *
		 * This function notifies editor that resource is going to change.
		 * During the change, resource is considered as unstable, and won't
		 * be processed by editor. After the change you MUST call UnlockResource().
		 */
		virtual void ResourceLock();

		/** \brief Function called when resource change is finished
		 *
		 * After that call, new undo entry is created and new preview is
		 * generated for resource.
		 */
		virtual void ResourceUnlock();

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

	private:

        WX_DECLARE_HASH_SET(wxsWindowEditor*,wxPointerHash,wxPointerEqual,WindowSet);
        WX_DEFINE_ARRAY(wxsItem*,ItemArray);

        /** \brief Helper function for fetching root item */
        inline wxsItem* RootItem() { return GetWinRes()->GetRootItem(); }

        wxsWindowEditorContent* Content;///< \brief Window with content area
        wxNotebook* WidgetsSet;         ///< \brief Notebook with all widgets inside
        wxBoxSizer* VertSizer;          ///< \brief Root sizer of this editor
        wxBoxSizer* HorizSizer;         ///< \brief Horizontal sizer managing items below palette
        wxBoxSizer* QPSizer;            ///< \brief Sizer for quick properties
        wxBoxSizer* OpsSizer;           ///< \brief Sizer for operations pane
        wxScrolledWindow* QPArea;       ///< \brief Scrolled window containing all QuickProps sturr
        wxBitmapButton* InsIntoBtn;
        wxBitmapButton* InsBeforeBtn;
        wxBitmapButton* InsAfterBtn;
        wxBitmapButton* DelBtn;
        wxBitmapButton* PreviewBtn;
        wxBitmapButton* QuickPanelBtn;
        wxWindow* TopPreview;           ///< \brief Top window of preview
        int InsType;                    ///< \brief Current insertion type
        int InsTypeMask;                ///< \brief Current insertion type mask
        bool QuickPropsOpen;            ///< \brief Set to true if quick properties panel is opened
        int  ResourceLockCnt;           ///< \brief Number of resource locks

        static wxImage InsIntoImg;
        static wxImage InsBeforeImg;
        static wxImage InsAfterImg;
        static wxImage DelImg;
        static wxImage PreviewImg;
        static wxImage QuickPropsImgOpen;
        static wxImage QuickPropsImgClose;
        static wxImage SelectedImg;
        static WindowSet AllEditors;
        static bool ImagesLoaded;

        static const int itBefore = 0x01;
        static const int itAfter  = 0x02;
        static const int itInto   = 0x04;

        wxsWinUndoBuffer* UndoBuff;     ///< \brief Undo buffer
        wxsCorrector* Corrector;        ///< \brief Data corrector

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

        /** \brief Function inserting new widget */
        void InsertRequest(const wxString& Name);

        /** \brief Function adding item before given one */
        bool InsertBefore(wxsItem* New,wxsItem* Ref);

        /** \brief Function adding item after given one */
        bool InsertAfter(wxsItem* New,wxsItem* Ref);

        /** \brief Function adding item into given one */
        bool InsertInto(wxsItem* New,wxsItem* Ref);

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

        /** \brief Function building preview of current resource
         *
         * This function assumes that there's NO preview created yet
         */
        void BuildPreview();

        /** \brief Function destroying current preview */
        void KillPreview();

        /** \brief Function checking if given item or any of it's children is selected */
        bool HasSelection(wxsItem* Item);

        /** \brief Getting selection skipping child items when parent is selected */
        static void GetSelectionNoChildren(ItemArray& Array,wxsItem* Item);

        /** \brief Destroying all selected items
         *  \param Item Use RootItem() for that
         */
        void KillSelection(wxsItem* Item);

        friend class wxsWindowEditorContent;

        DECLARE_EVENT_TABLE()
};

#endif
