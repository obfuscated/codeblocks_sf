#ifndef WXSWINDOWEDITOR_H
#define WXSWINDOWEDITOR_H

#include <wx/wx.h>
#include <vector>

#include "wxsproject.h"
#include "wxseditor.h"
#include "wxscoder.h"
#include "wxsevent.h"
#include "wxsglobals.h"

class WXSCLASS wxsWidget;
class WXSCLASS wxsDragWindow;
class WXSCLASS wxsWindowRes;
class WXSCLASS wxsWinUndoBuffer;

class WXSCLASS wxsWindowEditor : public wxsEditor
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
		virtual bool GetModified();

		/** Set the resources's modification state to \c modified. */
		virtual void SetModified(bool modified);

		/** Checking if can Undo */
		virtual bool CanUndo();

		/** Ckecing if can Redo */
		virtual bool CanRedo();

		/** Checking if we can cut */
		virtual bool CanCut();

		/** Checking if we can copy */
		virtual bool CanCopy();

		/** Checking if we can paste */
		virtual bool CanPaste();

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

    protected:

        /** Getting wxsWindowRes pointer to currently edited resource */
        inline wxsWindowRes* GetWinRes() { return (wxsWindowRes*)GetResource(); }

        /** Getting vector of selected widges. If any-level parent of widget
         *  is also selected, widget is skipped.
         */
        void GetSelectionNoChildren(std::vector<wxsWidget*>& Vector);

	private:

        wxScrolledWindow* Scroll;   ///< Scrolled window, parent for preview
        wxNotebook* WidgetsSet;     ///< Notebook with all widgets inside
        wxBoxSizer* VertSizer;      ///< Root sizer of this editor
        wxBoxSizer* HorizSizer;     ///< Horizontal sizer managing items below palette
        wxBoxSizer* QPSizer;        ///< Sizer for quick properties
        wxBoxSizer* OpsSizer;       ///< Sizer for operations pane

        wxImage InsIntoImg;
        wxImage InsBeforeImg;
        wxImage InsAfterImg;
        wxImage DelImg;
        wxImage PreviewImg;
        wxImage QuickPropsImgOpen;
        wxImage QuickPropsImgClose;

		/* Event handlers */
        void OnMouseClick(wxMouseEvent& event);
        void OnSelectWidget(wxsEvent& event);
        void OnUnselectWidget(wxsEvent& event);


        /** New layer used for dragging widgets */
        wxsDragWindow* DragWnd;

        /** Undo buffer */
        wxsWinUndoBuffer* UndoBuff;

        /** Flag for MultipleAddMode */
        bool InsideMultipleChange;

        /** Initializing images */
        void InitializeImages();

        /** Building palette */
        void BuildPalette(wxNotebook* Palette);

        DECLARE_EVENT_TABLE()
};


#endif // WXSWINDOWEDITOR_H
