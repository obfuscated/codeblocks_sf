#ifndef WXSINTPROPERTY_H
#define WXSINTPROPERTY_H

#include "../wxsproperties.h"

class wxsIntPropertyWindow;
class wxsIntProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param Int - variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxsIntProperty(wxsProperties* Properties,int &Int, bool AlwaysUpdate);
		
		/** Dctor */
		virtual ~wxsIntProperty();
		
        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();
        
    protected:

        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif
        
            
        /** This function makes additional correction for value, must always
         *  return acceptable one. It can be declared inside derived classes
         *  to extend abilities of IntProperty (f.ex. inly odd numbers)
         */
        virtual int CorrectValue(int Value) { return Value; }
        
	private:
	
        int& Value;
        bool AlwUpd;
        
        #ifdef __NO_PROPGRGID
            wxsIntPropertyWindow* Window;
            friend class wxsIntPropertyWindow;
        #else
            wxPGId PGId;
        #endif
        
};

#endif // WXSINTPROPERTY_H
