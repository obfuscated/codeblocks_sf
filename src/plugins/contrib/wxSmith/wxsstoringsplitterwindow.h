#ifndef WXSSTORINGSPLITTERWINDOW_H
#define WXSSTORINGSPLITTERWINDOW_H

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/event.h>

/** \brief This class extends behaviour of standard splitter window
 *         fixing some bugs with them when using docking systems
 */
class wxsStoringSplitterWindow : public wxPanel
{
	public:

        /** \brief Ctor */
		wxsStoringSplitterWindow(wxWindow* Parent);

		/** \brief Function splitting window */
		void Split(wxWindow* Top,wxWindow* Bottom,int SashPosition = 0);

		/** \brief Getting splitter window associated with this object */
		inline wxSplitterWindow* GetSplitter() { return Splitter; }

    private:

        void OnSize(wxSizeEvent& event);
        void OnSplitterChanging(wxSplitterEvent& event);
        void OnSplitterChanged(wxSplitterEvent& event);

        int SplitterFixup(int ProposedPosition);

        wxSplitterWindow* Splitter;
        int SplitPosition;
        DECLARE_EVENT_TABLE()
};

#endif
