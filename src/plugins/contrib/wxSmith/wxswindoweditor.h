#ifndef WXSWINDOWEDITOR_H
#define WXSWINDOWEDITOR_H

#include <wx/wx.h>

#include "wxsproject.h"
#include "wxseditor.h"
#include "wxscoder.h"
#include "wxsevent.h"

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
		
    protected:

        /** Getting wxsWindowRes pointer to currently edited resource */
        inline wxsWindowRes* GetWinRes() { return (wxsWindowRes*)GetResource(); }

	private:

		/** Scrolled window, parent for preview */
        wxScrolledWindow* Scroll;

		/* Event handlers */
        void OnMouseClick(wxMouseEvent& event);
        void OnSelectWidget(wxsEvent& event);
        void OnUnselectWidget(wxsEvent& event);

        /** New layer used for dragging widgets */
        wxsDragWindow* DragWnd;
        
        /** Undo buffer */
        wxsWinUndoBuffer* UndoBuff;
        
        DECLARE_EVENT_TABLE()
};


#endif // WXSWINDOWEDITOR_H
