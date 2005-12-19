#ifndef WXSSTRINGLISTCHECKPROPERTY_H
#define WXSSTRINGLISTCHECKPROPERTY_H

#include <wx/propgrid/propdev.h>

#include "../wxsproperties.h"
#include "../widget.h"

WX_DEFINE_ARRAY(bool,wxsArrayBool);

class WXSCLASS wxsStringListCheckProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsStringListCheckProperty(wxsProperties* Properties,wxArrayString& Array,wxsArrayBool& Checks,int SortedFlag=0);

		/** DCtor */
		virtual ~wxsStringListCheckProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:
	
        static bool PropertyGridCallback(wxPropertyGrid* propGrid,wxPGProperty* property,wxPGCtrlClass* ctrl,int data);
        
        bool EditValues(wxPropertyGrid* propGrid);
        void RebuildData(wxPropertyGrid* propGrid);

		wxArrayString& Array;
		wxsArrayBool& Checks;
		int SortedFlag;

        wxPGId PGId;
};

#endif
