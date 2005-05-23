#ifndef WXS2INTPROPERTY_H
#define WXS2INTPROPERTY_H

#include "../wxsproperties.h"

class wxs2IntPropertyWindow;
class wxs2IntProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param Int1 - first variable which will be monitored
         * \param Int2 - second variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxs2IntProperty(wxsProperties* Properties,int &Int1,int &Int1, bool AlwaysUpdate=true);
		
		/** Dctor */
		virtual ~wxs2IntProperty();
		
        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();
        
    protected:
        
        /** This function must create window which will be responsible for
         *  editing property's value */
        virtual wxWindow* BuildEditWindow(wxWindow* Parent);
        
        /** This function makes additional correction for value, must always
         *  return acceptable one. It can be declared inside derived classes
         *  to extend abilities of IntProperty (f.ex. inly odd numbers)
         */
        virtual void CorrectValues(int &Value1, int &Value2) { }
        
        /** This funcytion must update content of currently created editor window
         *  taking it's value prop current property
         */
        virtual void UpdateEditWindow();
        
	private:
	
        int& Value1;
        int& Value2;
        bool AlwUpd;
        wxs2IntPropertyWindow* Window;
        
        friend class wxs2IntPropertyWindow;
};

#endif // WXS2INTPROPERTY_H
