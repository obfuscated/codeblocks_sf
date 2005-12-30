#ifndef WXSEVENTSEDITOR_H
#define WXSEVENTSEDITOR_H

#include <wx/scrolwin.h>
#include <wx/propgrid/propgrid.h>
#include "wxsglobals.h"

class wxsWidget;
class wxsWidgetEvents;
class wxsWindowRes;
class wxsProject;
class wxsEventDesc;

class wxsEventsEditor : public wxPropertyGrid
{
	public:

        /** Ctor */
		wxsEventsEditor(wxWindow* Parent,wxsWidget* Widget);

		/** Dctor */
		virtual ~wxsEventsEditor();

		/** Setting widget with events to be edited */
		void SetWidget(wxsWidget* Widget);

	private:

        wxsWidgetEvents* Events;
        wxsWidget* Widget;
        wxsWindowRes* Res;
        wxsProject* Proj;
        wxString SourceFile;
        wxString HeaderFile;
        wxString ClassName;

        void BuildPropertyGrid(bool UpdateOnly=false);
        void ReadPropertyGrid();
        void UpdatePropertyGrid() { BuildPropertyGrid(true); }
        void FindFunctions(const wxString& EventType,wxArrayString& Array);

        wxString GetNewFunction(wxsEventDesc* Event);
        wxString GetFunctionProposition(wxsEventDesc* Event);
        bool CreateNewFunction(wxsEventDesc* Event,const wxString& NewFunctionName);

        void OnPropertyChanged(wxPropertyGridEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif // WXSEVENTSEDITOR_H
