#ifndef WXSBOOLPROPERTY_H
#define WXSBOOLPROPERTY_H

#include "../wxsproperties.h"

class wxsBoolProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param Bool - variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxsBoolProperty(wxsProperties* Properties,bool &Bool);

		/** Dctor */
		virtual ~wxsBoolProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        bool& Value;
        wxPGId PGId;
};

#endif // WXSBOOLPROPERTY_H
