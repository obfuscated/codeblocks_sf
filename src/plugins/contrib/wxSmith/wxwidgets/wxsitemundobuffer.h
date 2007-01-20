#ifndef WXSITEMUNDOBUFFER_H
#define WXSITEMUNDOBUFFER_H

#include <wx/arrstr.h>

/** \brief This class is used to handle all Undo and Redo actions.
 *
 * This class bases on idea that any class can be represented
 * as Xml structure which can be hold inside simple string.
 * Each uundo position entry holds whole structure of resource, what
 * in case of standard resources should not exceed 50 kB. In case
 * of memory consumption problems this can be changed to hold differences
 * between two xml nodes only.
 */
class wxsItemUndoBuffer
{
	public:

        /** \brief Ctor */
		wxsItemUndoBuffer(int MaxEnteries=100);

		/** \brief Dctor */
		~wxsItemUndoBuffer();

		/** \brief Checkign if we can undo */
		inline bool CanUndo() { return m_CurrentPos > 0; }

		/** \brief Checking if we can redo */
		inline bool CanRedo() { return m_CurrentPos < GetCount() - 1; }

		/** \brief Checking if current undo position is modified relatively to form saved on disk */
		inline bool IsModified() { return m_CurrentPos != m_SavedPos; }

		/** \brief Adding new undo position
		 *  \param XmlData xml form of reosurce stored inside string
		 */
		void StoreChange(const wxString& XmlData);


		/** \brief Setting lastest undo buffer to saved state (equialent to content on files) */
		inline void Saved() { m_SavedPos = m_CurrentPos; }

		/** \brief Undoing
		 * \return Xml data previously provided in StoreChange or empty string if can not undo
         */
        const wxString& Undo();

        /** \brief Redoing
		 * \return Xml data previously provided in StoreChange or empty string if can not undo
         */
        const wxString& Redo();

	private:

        /** \brief Getting number of enteries in undo array */
        inline int GetCount() { return (int)m_Enteries.Count(); }

        wxArrayString m_Enteries;   ///< \brief Array enteries
        int m_CurrentPos;           ///< \brief Current position in undo buffer
        int m_SavedPos;             ///< \brief Undo position representing not-changed resource (in form it's on disk)
        int m_MaxEnteries;          ///< \brief Max enteries in undo buffer
};

#endif
