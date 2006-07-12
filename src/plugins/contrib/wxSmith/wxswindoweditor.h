#ifndef WXSWINDOWEDITOR_H
#define WXSWINDOWEDITOR_H

#include <wx/wx.h>
#include <vector>
#include <set>

#include "wxsproject.h"
#include "wxseditor.h"
#include "wxscoder.h"
#include "wxsevent.h"
#include "wxsglobals.h"

class wxsWidget;
class wxsDragWindow;
class wxsWindowRes;
class wxsWinUndoBuffer;

class wxsWindowEditor : public wxsEditor
{
	public:
		wxsWindowEditor(wxWindow* parent, wxsWindowRes* Resource);

		virtual ~wxsWindowEditor();

		/** Creating preview for resource */
		virtual void BuildPreview();

		/** Killing previously created preview */
		virtual void KillPreview();

		/** Closing action will store code changes */
		virtual bool Close();

		/** Saving resource */
		virtual bool Save();

		/** Returns true if resource is modified, false otherwise */
		virtual bool GetModified() const;

		/** Set the resources's modification state to \c modified. */
		virtual void SetModified(bool modified);

		/** Checking if can Undo */
		virtual bool CanUndo() const;

		/** Ckecing if can Redo */
		virtual bool CanRedo() const;

		/** Checking if we can cut */
		virtual bool HasSelection() const;

		/** Checking if we can paste */
		virtual bool CanPaste() const;

		/** Undoing */
		virtual void Undo();

		/** Redoing */
		virtual void Redo();

		/** Cutting */
		virtual void Cut();

		/** Copying */
		virtual void Copy();

		/** Pasting */
		virtual void Paste();

		/** Getting undo buffer */
		inline wxsWinUndoBuffer* GetUndoBuff() { return UndoBuff; }

		/** Starting multiple-add mode.
		 *
		 * Multiple add proceedure MUST be finished with EndMultipleAdd() function
		 */
        bool StartMultipleChange();

        /** Ending multiple-add mode
         *
         * Multiple add procedure MUST begin with StartMultipleAdd() function
         */
        bool EndMultipleChange();

		/** Adding new widget before current selection */
		bool InsertBefore(wxsWidget* New,wxsWidget* Ref=NULL);

		/** Adding new widget after current selection */
		bool InsertAfter(wxsWidget* New,wxsWidget* Ref=NULL);

		/** Adding new widget into current selection */
		bool InsertInto(wxsWidget* New,wxsWidget* Ref=NULL);

		/** Reloading images in all editors */
		static void ReloadImages();

		/** Sending given event to all editors */
		static void SpreadEvent(wxEvent& event);

    protected:

        /** Getting wxsWindowRes pointer to currently edited resource */
        inline wxsWindowRes* GetWinRes() { return (wxsWindowRes*)GetResource(); }

        /** Getting const wxsWindowRes pointer to currently edited resource */
        inline const wxsWindowRes* GetWinResConst() const { return (const wxsWindowRes*)GetResourceConst(); }

        /** Getting vector of selected widges. If any-level parent of widget
         *  is also selected, widget is skipped.
         */
        inline void GetSelectionNoChildren(std::vector<wxsWidget*>& Vector);

        /** Getting current selected widget (the one with black borders) */
        wxsWidget* GetSelection();

	private:

//        wxScrolledWindow* Scroll;       ///< Scrolled window, parent for preview
        wxNotebook* WidgetsSet;         ///< Notebook with all widgets inside
        wxBoxSizer* VertSizer;          ///< Root sizer of this editor
        wxBoxSizer* HorizSizer;         ///< Horizontal sizer managing items below palette
        wxBoxSizer* QPSizer;            ///< Sizer for quick properties
        wxBoxSizer* OpsSizer;           ///< Sizer for operations pane
        wxScrolledWindow* QPArea;       ///< Scrolled window containing all QuickProps sturr
        wxScrolledWindow* OpsBackground;
        wxBitmapButton* InsIntoBtn;
        wxBitmapButton* InsBeforeBtn;
        wxBitmapButton* InsAfterBtn;
        wxBitmapButton* DelBtn;
        wxBitmapButton* PreviewBtn;
        wxBitmapButton* QuickPanelBtn;

        int InsType;                    ///< Current insertion type
        int InsTypeMask;                ///< Current insertion type mask

        bool QuickPropsOpen;            ///< Set to true if quick properties panel is opened

        static wxImage InsIntoImg;
        static wxImage InsBeforeImg;
        static wxImage InsAfterImg;
        static wxImage DelImg;
        static wxImage PreviewImg;
        static wxImage QuickPropsImgOpen;
        static wxImage QuickPropsImgClose;
        static wxImage SelectedImg;
        static std::set<wxsWindowEditor*> AllEditors;
        static bool ImagesLoaded;

        static const int itBefore = 0x01;
        static const int itAfter  = 0x02;
        static const int itInto   = 0x04;

        /** New layer used for dragging widgets */
        wxsDragWindow* DragWnd;

        /** Undo buffer */
        wxsWinUndoBuffer* UndoBuff;

		/* Event handlers */
        void OnMouseClick(wxMouseEvent& event);
        void OnSelectWidget(wxsEvent& event);
        void OnUnselectWidget(wxsEvent& event);
        void OnButton(wxCommandEvent& event);
        void OnInsInto(wxCommandEvent& event);
        void OnInsBefore(wxCommandEvent& event);
        void OnInsAfter(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);
        void OnQuickProps(wxCommandEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnRelayout(wxCommandEvent& event);
//        void OnContentScroll(wxScrollEvent& event);
//        void OnContentSize(wxSizeEvent& event);

        /** Flag for MultipleAddMode */
        bool InsideMultipleChange;

        /** Initializing images */
        static void InitializeImages();

        /** Building palette */
        void BuildPalette(wxNotebook* Palette);

        /** Function inserting new widget */
        void InsertRequest(const wxString& Name);

        /** Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);

        /** Setting new insertion type */
        void SetInsertionType(int Type);

        /** Rebuilding pictures on insertion type buttons */
        void RebuildInsTypeIcons();

        /** Rebuilding picture on Quick Props button */
        void RebuildQuickPropsIcon();

        /** Rebuilding all icons */
        void RebuildIcons();

        /** Building icon for one button */
        void BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,bool Selected,bool Enabled);

        /** Opening or closiung Quick Props panel */
        void ToggleQuickPropsPanel(bool Open);

        /** Refreshing content of Quick Props panel */
        void RebuildQuickProps(wxsWidget* Selection);

        DECLARE_EVENT_TABLE()
};


#endif // WXSWINDOWEDITOR_H
