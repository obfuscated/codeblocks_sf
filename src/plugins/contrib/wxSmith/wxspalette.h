#ifndef WXSPALETTE_H
#define WXSPALETTE_H

#include <wx/panel.h>
#include <wx/radiobut.h>


class wxSmith;
class wxsWidget;

class wxsPalette : public wxPanel
{
	public:
	
        static const int itBefore = 0x01;
        static const int itAfter  = 0x02;
        static const int itInto   = 0x04;
	
        /** Constructor */
		wxsPalette(wxWindow* Parent,wxSmith* Plugin);
		
		/** Destructor */
		virtual ~wxsPalette();
		
		/** Getting singleton object - one palette inside whole plugin*/
		static wxsPalette* Get() { return Singleton; }
		
        /** Getting current insertion type */
        int GetInsertionType();
        
        /** Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);
		
	private:
	
        /** Plugin object */
        wxSmith* Plugin;
        
        /* Checkboxes */
        wxRadioButton* AddBefore;
        wxRadioButton* AddAfter;
        wxRadioButton* AddInto;
        
        /* Current insertion type */
        int InsType;
        
        /* Current insertion type mask */
        int InsTypeMask;
        
        /** Singleton object */
        static wxsPalette* Singleton;
        
        /** Funcntion creating palette of widgets in given window */
        void CreateWidgetsPalette(wxWindow* Wnd);
        
        /** Function inserting widget with giwen name */
        void InsertRequest(const char* Name);
        
        /** Function adding new widget before given one */
        void InsertBefore(wxsWidget* NewWidget,wxsWidget* Reference);
        
        /** Function adding new widget after given one */
        void InsertAfter(wxsWidget* NewWidget,wxsWidget* Reference);
        
        /** Function adding new widget into given one */
        void InsertInto(wxsWidget* NewWidget,wxsWidget* Reference);
        
        /** Function deleting currently selected widget */
        void DeleteRequest();
        
        /* Event-processing functions */
        void OnRadio(wxCommandEvent& event);
        void OnButton(wxCommandEvent& event);
        
        
        DECLARE_EVENT_TABLE()
        
};

#endif // WXSPALETTE_H
