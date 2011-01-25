/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBAUIBOOK_H_INCLUDED
#define CBAUIBOOK_H_INCLUDED

#include <wx/aui/auibook.h>
#include <wx/dynarray.h>

class wxTipWindow;

WX_DEFINE_ARRAY_PTR(wxAuiTabCtrl*,cbAuiTabCtrlArray);

/** \brief A notebook class
  * This class is derived from wxAuiNotebook, to enhance its abilities.
  * It adds the ability to store (and restore) the visible tab-order, because
  * auinotebook-tabs can be reordered with drag and drop.
  * Another added feature is the possibility to add tooltips to the tabs belonging
  * to added panes.
 */
class cbAuiNotebook : public wxAuiNotebook
{
    public:
        /** \brief cbAuiNotebook constructor
         *
         * \param pParent the parent window, usually the app-window
         * \param id the notebook id
         * \param pos the position
         * \param size the size
         * \param style the notebook style
         *
         */
        cbAuiNotebook(wxWindow* pParent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE);
        /** cbAuiNotebook destructor  */
        virtual ~cbAuiNotebook();

        /** \brief Advances the selection
         *
         * In contrast to the base-classes function, it uses the visible tab-order, not the order
         * of creation and jumps to the first tab, if the last is reached (and vice versa)
         * \param forward if false direction is backwards
         */
        void AdvanceSelection(bool forward = true);
        /** \brief Save layout of the notebook
         * \return wxString the serialized layout
         * @remarks Not used at the moment, because it's not (yet) possible to restore the layout,
         * due to limitations of the base class.
         *
         */
        wxString SavePerspective();
        /** \brief Loads serialized notebook layout
         * \param layout the serialized layout
         * \return bool true if successfull
         * @remarks Not implemented. Don't use it.
         *
         */
        bool LoadPerspective(const wxString& layout){return false;}
        /** \brief Get the tab position
         *
         * Returns the position of the tab as it is visible.
         * Starts with 0
         * \param index the index of the tab in order of creation
         * \return int the visible position
         *
         */
        int GetTabPositionFromIndex(int index);
        /** \brief Set a tab tooltip
         *
         * Sets the tooltip for the tab belonging to win.
         * Starts the dwell timer and the stopwatch if it is not already done.
         * \param win the pane that belongs to the tab
         * \param msg the tooltip
         * \return void
         * @remarks Uses the name of the wxWindow to store the message
         *
         */
        void SetTabToolTip(wxWindow* win, wxString msg);
        /** \brief Allow tooltips
         *
         * Allows or forbids tooltips.
         * Cancels already shown tooltips, if allow is false
         * \param allow if false toltips are not allowed
         * \return void
         *
         */
        void AllowToolTips(bool allow = true);
        /** \brief Enable or disable tabtooltips globally
         *
         * \param use If true tooltips are allowed
         * \return void
         *
         */
        void UseToolTips(bool use = true);
        /** \brief Set the time before a tab-tooltip kicks in
         *
         * \param time The dwell time
         * \return void
         *
         */
        void SetDwellTime(long time = 1000){m_DwellTime = time;}
        /** \brief Minmize free horizontal page
         *
         * Moves the active tab to the rightmost place,
         * to show as many tabs as possible.
         * \return void
         *
         */
        void MinimizeFreeSpace();
        /** \brief Delete Page
         *
         * Calls the base-class function and after that
         * MinmizeFreeSpace(), needed to hook into the close-events.
         * The system generated close event has to be veto'd, and Close()
         * has to be called manually, so we can handle it ourselves.
         * \param The index of the tab to be closed
         * \return true if successfull
         *
         */
        bool DeletePage(size_t page);
        /** \brief Remove Page
         *
         * Calls the base-class function and after that
         * MinmizeFreeSpace(), needed to hook into the close-events.
         * The system generated close event has to be veto'd, and Close()
         * has to be called manually, so we can handle it ourselves.
         * \param The index of the tab to be closed
         * \return true if successfull
         *
         */
        bool RemovePage(size_t page);
        /** \brief Move page
         *
         * Moves the tab containing page to new_idx
         * \param page The page to move (e.g. cbEditor*)
         * \param new_idx The index the page should be moved to
         * \return true if successfull
         *
         */
        bool MovePage(wxWindow* page, size_t new_idx);
        /** \brief Set zoomfactor for builtin editors
         *
         * Sets the zoomfactor for all visible builtin
         * editors.
         * \param zoom zoomfactor to use
         * \return void
         *
         */
        void SetZoom(int zoom);
    protected:
        /** \brief Handle the navigation key event
         *
         * Tries to handle the navigation key-event and use "our" AdvanceSelection().
         * \param event
         * \return void
         * @remarks Works not reliable, due to OS/wxWidgets-limitations
         *
         */
#if wxCHECK_VERSION(2, 9, 0)
        void OnNavigationKeyNotebook(wxNavigationKeyEvent& event);
#else
        void OnNavigationKey(wxNavigationKeyEvent& event);
#endif
        /** \brief OnIdle
         *
         * \param event unused
         * \return void
         *
         */
        void OnIdle(wxIdleEvent& /*event*/);
        /** \brief Check whether the mouse is over a tab
         *
         * \param event unused
         * \return void
         *
         */
        void OnDwellTimerTrigger(wxTimerEvent& /*event*/);
        /** \brief Catch doubleclick-events from wxTabCtrl
         *
         * Sends cbEVT_CBAUIBOOK_LEFT_DCLICK, if doubleclick was on a tab,
         * event-Id is the notebook-Id, event-object is the pointer to the window the
         * tab belongs to.
         * \param event holds the wxTabCtrl, that sends the event
         * \return void
         *
         */
        void OnTabCtrlDblClick(wxMouseEvent& event);
        /** \brief Catch resize-events and call MinimizeFreeSpace()
         *
         * \param event unused
         * \return void
         *
         */
        void OnResize(wxSizeEvent& event);
        /** \brief Updates the array, that holds the wxTabCtrls
         *
         * \return void
         *
         */
        void UpdateTabControlsArray();
        /** \brief Shows tooltip for win
         *
         * \param win
         * \return void
         *
         */
        void ShowToolTip(wxWindow* win);
        /** \brief Cancels tooltip
         *
         * \return void
         *
         */
        void CancelToolTip();
        /** \brief Holds the wxTabCtrls used by the notebook
         * @remarks Should be updated with UpdateTabControlsArray(),
         * before it's used
         */
        cbAuiTabCtrlArray m_TabCtrls;
        /** \brief Stopwatch used to determine how long the mouse has not
         * been moved over a tab.
         */
        wxStopWatch m_StopWatch;
        /** \brief Timer used to check the mouse-position
         */
        wxTimer* m_pDwellTimer;
        /** \brief The actual shown tooltip or nullptr
         */
        wxTipWindow* m_pToolTip;
        /** \brief Position of the mouse, at last dwell timmer event.
         *
         * Used to determine whether the mouse was moved or not.
         */
        wxPoint m_LastMousePosition;
        /** \brief Position of tooltip
         *
         * Used to determine, whether a tooltiop is already shown at
         * the actual mouse-position
         */
        wxPoint m_LastShownAt;
        /** \brief Last time the dwell timer triggered an event
         *
         * Used to determine how long the mouse has not been moved over a tab .
         */
        long m_LastTime;
        /** \brief Enable or disable tab tooltips
         *
         */
        bool m_UseTabTooltips;
        /** \brief Tab tooltip dwell time
         *
         */
        long m_DwellTime;
        /** \brief If false, tooltips are temporary forbidden
         *
         * Needed to not interfere with context-menus etc.
         */
        bool m_AllowToolTips;
        /** \brief If true, zoom for all editors
         * is set in next OnIdle-call
         *
         */
        bool m_SetZoomOnIdle;
        /** \brief Holds the id of the dwell timer
         */
        static const long idNoteBookTimer;

        DECLARE_EVENT_TABLE()
};

#endif // CBAUIBOOK_H_INCLUDED
