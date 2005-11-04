#ifndef WXSPALETTE_H
#define WXSPALETTE_H

#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/timer.h>

#include "wxsevent.h"
#include "wxspaletteheader.h"

class WXSCLASS wxSmith;
class WXSCLASS wxsWidget;
class WXSCLASS wxsResource;

class WXSCLASS wxsPalette : public wxPanel
{
	public:

        static const int itBefore = 0x01;
        static const int itAfter  = 0x02;
        static const int itInto   = 0x04;

        /** Constructor */
		wxsPalette(wxWindow* Parent,int PageNum);

		/** Destructor */
		virtual ~wxsPalette();

		/** Getting singleton object - one palette inside whole plugin*/
		static wxsPalette* Get() { return Singleton; }

        /** Getting current insertion type */
        int GetInsertionType() { return Header.GetInsertionType(); }

        /** Function deleting currently selected widget */
        void DeleteRequest();

        /** Function showing preview of currently selected resource */
        void PreviewRequest();

        /** Function refreshing icons for widgets */
        void RefreshIcons();

	private:

        /** Currently selected resource */
        wxsResource* SelectedRes;

        /** Currently selected widget */
        wxsWidget* SelectedWidget;

        /** Palette header object */
        wxsPaletteHeader Header;

        /** Page number */
        int PageNum;

        /** Timer object used to flush code changes */
        wxTimer Timer;

        /** Panel used as background for widgets */
        wxPanel* WidgetsSpace;

        /** Singleton object */
        static wxsPalette* Singleton;

        /** Funcntion creating palette of widgets in given window */
        void CreateWidgetsPalette(wxWindow* Wnd);

        /** Function inserting widget with giwen name */
        void InsertRequest(const wxString& Name);

        /** Function adding new widget before given one */
        void InsertBefore(wxsWidget* NewWidget,wxsWidget* Reference);

        /** Function adding new widget after given one */
        void InsertAfter(wxsWidget* NewWidget,wxsWidget* Reference);

        /** Function adding new widget into given one */
        void InsertInto(wxsWidget* NewWidget,wxsWidget* Reference);

        /** Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask) { Header.SetInsertionTypeMask(Mask); }

        /* Event-processing functions */
        void OnButton(wxCommandEvent& event);
		void OnSelectRes(wxsEvent& event);
		void OnUnselectRes(wxsEvent& event);
		void OnSelectWidget(wxsEvent& event);
		void OnUnselectWidget(wxsEvent& event);
		void OnTimer(wxTimerEvent& event);

        DECLARE_EVENT_TABLE()

};

#endif // WXSPALETTE_H
