#ifndef WXSSPLITTERWINDOWEX_H
#define WXSSPLITTERWINDOWEX_H

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/event.h>

/** This class extends behaviour of standard splitter window
 *  which fixes some bugs with them when using wxDockIt
 */
class WXSCLASS wxsSplitterWindowEx : public wxPanel
{
	public:

        /** Ctor */
		wxsSplitterWindowEx(wxWindow* Parent);

		/** Dctor */
		virtual ~wxsSplitterWindowEx();

		/** Function splitting window */
		void Split(wxWindow* Top,wxWindow* Bottom,int SashPosition = 0);

		/** Getting splitter window associated with this object */
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

#endif // WXSSPLITTERWINDOW_H
