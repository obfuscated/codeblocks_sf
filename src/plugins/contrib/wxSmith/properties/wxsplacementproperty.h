#ifndef WXSPLACEMENTPROPERTY_H
#define WXSPLACEMENTPROPERTY_H

#include "../wxsproperties.h"

class wxsPlacementProperty : public wxsProperty
{
	public:

        /** Ctor 
         * \param Placement - placement flags
         * \param Expand - expand flag
         * \param Shaped - shaped flag
         * \param ExpandName - name of property handling expand flag
         * \param ShapedName - name of property handling shaped flag
         */
		wxsPlacementProperty(
            int& Placement,
            bool& Expand,
            bool& Shaped,
            const wxString& ExpandPropName = _("Expand"),
            const wxString& ShapedPropName = _("Shaped"));

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        int &PlacementType;
        bool &Expand;
        bool &Shaped;
        wxString ExpandName;
        wxString ShapedName;

        wxPGId PGId;
        wxPGId ExpandId;
        wxPGId ShapedId;
};

#endif
