#ifndef WXSSTRINGPROPERTY_H
#define WXSSTRINGPROPERTY_H

#include "../wxsproperties.h"

class WXSCLASS wxsStringPropertyWindow;

class WXSCLASS wxsStringProperty : public wxsProperty
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

        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif

	private:

        wxString& Value;
        bool AlwUpd;
        bool IsLong;

        #ifdef __NO_PROPGRGID
            wxsStringPropertyWindow* Window;
            friend class wxsStringPropertyWindow;
        #else
            wxPGId PGId;
        #endif
};

#endif // WXSSTRINGPROPERTY_H
