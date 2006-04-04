#ifndef WXSSTRINGLISTCHECKPROPERTY_H
#define WXSSTRINGLISTCHECKPROPERTY_H

#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include <wx/dialog.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>

#include "../wxsproperties.h"
#include "../widget.h"

WX_DEFINE_ARRAY(bool,wxsArrayBool);

class wxsStringListCheckProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsStringListCheckProperty(wxArrayString& Array,wxsArrayBool& Checks,int SortedFlag=0);

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        static bool PropertyGridCallback(wxPropertyGrid* propGrid,wxPGProperty* property,wxPGCtrlClass* ctrl,int data);

        bool EditValues(wxPropertyGrid* propGrid);
        void RebuildData(wxPropertyGrid* propGrid);
        bool IsSorted();

		wxArrayString& Array;
		wxsArrayBool& Checks;
		int SortedFlag;

        wxPGId PGId;
};

#endif
