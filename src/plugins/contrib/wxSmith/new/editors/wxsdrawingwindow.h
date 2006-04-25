#ifndef WXSDRAWINGWINDOW_H
#define WXSDRAWINGWINDOW_H

#include <wx/scrolwin.h>
#include <wx/timer.h>

/** \brief Class allowing drawing over it's surface.
 *
 * This window is upgrade of standard wxScrolledWindow.
 * It allows paining over it's surface and over surface
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
        void StartFetchingSequence();
        void FetchScreen();

        void OnSize(wxSizeEvent& event);


        class DrawingPanel;
        DrawingPanel* Panel;
        bool PaintAfterFetch;
        bool IsBlockFetch;
        wxBitmap* Bitmap;

        // NOTE: Added temporarilly
        wxTimer RepaintTimer;
        void OnRepaintTimer(wxTimerEvent&);

        DECLARE_EVENT_TABLE()
};

#endif
