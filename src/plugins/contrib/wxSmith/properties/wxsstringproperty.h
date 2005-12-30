#ifndef WXSSTRINGPROPERTY_H
#define WXSSTRINGPROPERTY_H

#include "../wxsproperties.h"

class wxsStringProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param String - variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxsStringProperty(wxsProperties* Properties,wxString& String, bool AlwaysUpdate,bool Long=false);

		/** Dctor */
		virtual ~wxsStringProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual wxString CorrectValue(const wxString& Value) { return Value; }

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        wxString& Value;
        bool AlwUpd;
        bool IsLong;
        wxPGId PGId;
};

#endif // WXSSTRINGPROPERTY_H
