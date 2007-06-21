#ifndef WXSTOOLSPACE_H
#define WXSTOOLSPACE_H

#include <wx/scrolwin.h>

class wxsItemResData;
class wxsTool;

/** \brief This class represents extra tools space in editor allowing to edit tools */
class wxsToolSpace: public wxScrolledWindow
{
    public:

        /** \brief Ctor */
        wxsToolSpace(wxWindow* Parent,wxsItemResData* Data);

        /** \brief Dctor */
        virtual ~wxsToolSpace();

        /** \brief Function shiwch must be called before changing preview */
        void BeforePreviewChanged();

        /** \brief Function which must be called after changing preview */
        void AfterPreviewChanged();

        /** \brief Function refreshing current selection (calculating new positions) */
        void RefreshSelection();

        /** \brief Checking whether there are any tools inside this resource */
        inline bool AreAnyTools() { return m_First!=0; }

    private:

        struct Entry;

        /** \brief Custom paint handler */
        void OnPaint(wxPaintEvent& event);

        /** \brief Custom mouse click event */
        void OnMouseClick(wxMouseEvent& event);

        /** \brief Custom mouse double-click event */
        void OnMouseDClick(wxMouseEvent& event);

        /** \brief Custom right click event */
        void OnMouseRight(wxMouseEvent& event);

        /** \brief Recalculating virtual space required by this window */
        void RecalculateVirtualSize();

        /** \brief Searching for tool entry at given position */
        Entry* FindEntry(int& PosX,int& PosY);

        /** \brief Entry for each tool in resource */
        struct Entry
        {
            wxsTool* m_Tool;
            Entry*   m_Next;
        };

        Entry*          m_First;        ///< \brief First tool in resource
        int             m_Count;        ///< \brief Number of enteries
        wxsItemResData* m_Data;         ///< \brief Resource's data
        bool            m_Unstable;     ///< \brief True between BeforePreviewChanged and AfterPreviewChanged to prevent some rare seg faults

        DECLARE_EVENT_TABLE()
};



#endif
