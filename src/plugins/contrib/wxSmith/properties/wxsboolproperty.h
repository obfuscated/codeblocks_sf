#ifndef WXSBOOLPROPERTY_H
#define WXSBOOLPROPERTY_H

#include "../wxsproperties.h"

class wxsBoolPropertyWindow;
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
        
        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual void PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif
        
	private:
	
        bool& Value;

        #ifdef __NO_PROPGRGID
            wxsBoolPropertyWindow* Window;
            friend class wxsBoolPropertyWindow;
        #else
            wxPGId PGId;
        #endif
        
};

#endif // WXSBOOLPROPERTY_H
