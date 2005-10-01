#ifndef WXSPALETTE_H
#define WXSPALETTE_H

#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/timer.h>

#include "wxsevent.h"

class wxSmith;
class wxsWidget;
class wxsResource;

class wxsPalette : public wxPanel
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
        int GetInsertionType();
        
	private:
	
        /** Currently selected resource */
        wxsResource* SelectedRes;
        
        /** Currently selected widget */
        wxsWidget* SelectedWidget;
        
        /* Checkboxes */
        wxRadioButton* AddBefore;
        wxRadioButton* AddAfter;
        wxRadioButton* AddInto;
        
        /** Current insertion type */
        int InsType;
        
        /** Current insertion type mask */
        int InsTypeMask;
        
        /** Page number */
        int PageNum;
        
        /** Timer object used to flush code changes */
        wxTimer Timer;
        
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
        
        /** Function deleting currently selected widget */
        void DeleteRequest();
        
        /** Function showing preview of currently selected resource */
        void PreviewRequest();
        
        /** Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);
        
        /* Event-processing functions */
        void OnRadio(wxCommandEvent& event);
        void OnButton(wxCommandEvent& event);
		void OnSelectRes(wxsEvent& event);
		void OnUnselectRes(wxsEvent& event);
		void OnSelectWidget(wxsEvent& event);
		void OnUnselectWidget(wxsEvent& event);
		void OnTimer(wxTimerEvent& event);
        
        DECLARE_EVENT_TABLE()
        
};

#endif // WXSPALETTE_H
