#ifndef WXSWINUNDOBUFFER_H
#define WXSWINUNDOBUFFER_H

#include <vector>
#include <wx/string.h>

#include "wxsglobals.h"

class WXSCLASS wxsWidget;
class WXSCLASS wxsWindowRes;

/** This class is used to handle all Undo and Redo actions.
 *
 * This class bases on ideea that any class can be represented
 * as Xml structure which can be hold inside simple string.
 * Each uundo position entry holds whole structure of resource, what
 * in case of standard resources should not exceed 50 kB. In case
 * of memory consumption problems this can be changed to hold differences
 * between two xml nodes only.
 */
class WXSCLASS wxsWinUndoBuffer
{
	public:

        /** Ctor */
		wxsWinUndoBuffer(wxsWindowRes* Resource,int MaxEnteries=100);

		/** Dctor */
		virtual ~wxsWinUndoBuffer();

        /** Getting number of enteries in undo array */
        inline int GetCount() { return (int)Enteries.size(); }

        /** Getting current undo position (counting from 0) */
        inline int GetCurrent() { return CurrentPos; }

		/** Clearing undo table */
		void Clear();

		/** Checkign if we can undo */
		inline bool CanUndo() { return CurrentPos > 0; }

		/** Checking if we can redo */
		bool CanRedo() { return CurrentPos < GetCount() - 1; }

		/** Checking if current undo position is modified relatively to form saved on disk */
		bool IsModified() { return CurrentPos != SavedPos; }

		/** Adding new undo position */
		void StoreChange();

		/** Setting last store point as Saved point */
		inline void Saved() { SavedPos = CurrentPos; }

		/** Undoing
		 *  \return Resource in form before change or NULL if can not undo */
        wxsWidget* Undo();

        /** Redoing
         *  \return Resource in form after change or NULL if can not redo */
        wxsWidget* Redo();

	private:

        /** One undo position */
        struct UndoEntry
        {
        	wxString XmlData;           ///< Xml structure representing resource
        	wxString SelectedWidget;    ///< Selection (currently not used)
        };

        /** Buildign resource from given entry */
        wxsWidget* BuildResourceFromEntry(UndoEntry* Entry);

        typedef std::vector<UndoEntry*> EnteriesT;
        typedef EnteriesT::iterator EnteriesI;

        /** Undo buffer */
        EnteriesT Enteries;

        /** Resource with monitored data */
        wxsWindowRes* Resource;

        /** Current undo position */
        int CurrentPos;

        /** Undo position representing not-changed resource (in form it's on disk) */
        int SavedPos;

        /** Max enteries in undo buffer */
        int MaxEnteries;
};

#endif // WXSWINUNDOBUFFER_H
