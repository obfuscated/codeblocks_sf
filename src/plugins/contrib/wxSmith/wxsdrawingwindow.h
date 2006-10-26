#ifndef WXSDRAWINGWINDOW_H
#define WXSDRAWINGWINDOW_H

#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <wx/event.h>

//#define FETCHING_SYSTEM1
#define FETCHING_SYSTEM2

/** \brief Class allowing drawing over it's surface.
 *
 * This window is upgrade of standard wxScrolledWindow.
 * It allows painting over it's surface and over surface
 * of it's children. It also handles all mouse and keyboard
 * events.
 */
class wxsDrawingWindow: public wxScrolledWindow
{
    public:

        /** \brief Ctor */
        wxsDrawingWindow(wxWindow* Parent,wxWindowID id);

        /** \brief Dctor */
        virtual ~wxsDrawingWindow();

        /** \brief Function fetching bitmap from this window
         *
         * \note Regions which were not visible to the screen
         *       won't be valid in this bitmap.
         */
        const wxBitmap& GetBitmap() { return *Bitmap; }

        /** \brief Function notifying about change of window content
         *
         * This function invalidates bitmap previously fetched from
         * window area. So it will be fetched again.
         */
        void ContentChanged();

        /** \brief Function repainting window
         *
         * Repainting window content may be done in two ways.
         * First is by raising paint event (so by caling wxWindow::Refresh),
         * in current implementation this require refetching bitmap from
         * wndow area (just like calling ContentChanged). Second way is by
         * calling this function. It does not raise any events if not needed
         * and draws using wxClientDC object. It may be used when extra
         * graphics has been changed only.
         */
        void FullRepaint();

        /** \brief Function (un)blocking background fetching */
        inline void BlockFetch(bool Block=true) { IsBlockFetch = Block; }

    protected:

        /** \brief Function used for drawing additional data
         *
         * You should override this function and paint additional
         * graphics inside this.
         */
        virtual void PaintExtra(wxDC* DC) = 0;

    private:

        void PanelPaint(wxPaintEvent& event);
        void PanelMouse(wxMouseEvent& event);
        void PanelKeyboard(wxKeyEvent& event);

        #ifdef FETCHING_SYSTEM2
        void OnFetchSequence(wxCommandEvent& event);
        #endif

        /** \brief Function stating sequence fetching editor's background
         *
         * This sequence may be splitted into few smaller events so it's not
         * granted that after finishing StartFetchingSequence, the content of
         * Bitmap is valid.
         */
        void StartFetchingSequence();

        /** \brief Function copying screen data into bitmap */
        void FetchScreen();

        /** \brief Hiding all children except Panel to avoid some random repaints */
        void HideChildren();

        /** \brief Showing all children except Panel just before fetching preview */
        void ShowChildren();

        class DrawingPanel;
        DrawingPanel* Panel;
        bool PaintAfterFetch;
        bool WaitTillHideChildren;
        bool IsBlockFetch;
        wxBitmap* Bitmap;

        #ifdef FETCHING_SYSTEM2
        bool DuringFetch;
        bool WaitingForPaint;
        #endif

        DECLARE_EVENT_TABLE()
};

#endif
