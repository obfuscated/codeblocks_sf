#ifndef WXSPROPERTIESMAN_H
#define WXSPROPERTIESMAN_H

#include "widget.h"
#include "wxsevent.h"
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/event.h>

class wxsPropertiesMan: public wxEvtHandler
{
	public:
		wxsPropertiesMan();
		virtual ~wxsPropertiesMan();
		static wxsPropertiesMan* Get() { return Singleton; }

		virtual void SetActiveWidget(wxsWidget* NewActive);

		inline wxsWidget* GetActiveWidget() { return CurrentWidget; }

	private:
        wxsWidget* CurrentWidget;
        wxScrolledWindow* PropertiesPanel;
        wxScrolledWindow* EventsPanel;
        static wxsPropertiesMan* Singleton;

		friend class wxSmith;

		void OnSelectWidget(wxsEvent& event);
		void OnUnselectWidget(wxsEvent& event);
		void OnSelectRes(wxsEvent& event);
		void OnUnselectRes(wxsEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif // WXSPROPERTIES_H
