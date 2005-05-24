#ifndef WXSPROPERTIESMAN_H
#define WXSPROPERTIESMAN_H

#include "widget.h"
#include <wx/panel.h>
#include <wx/scrolwin.h>

class wxsPropertiesMan
{
	public:
		wxsPropertiesMan();
		virtual ~wxsPropertiesMan();
		static wxsPropertiesMan* Get() { return &Singleton; }
		
		virtual void SetActiveWidget(wxsWidget* NewActive);
		
		inline wxsWidget* GetActiveWidget() { return CurrentWidget; }
		
	private:
        wxsWidget* CurrentWidget;
        wxScrolledWindow* PropertiesPanel;
        static wxsPropertiesMan Singleton;
		
		friend class wxSmith;
};

#endif // WXSPROPERTIES_H
