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
        virtual void CorrectValues(int &Value1, int &Value2) { }
    
	private:
	
        int& Value1;
        int& Value2;
        bool AlwUpd;
        
        #ifdef __NO_PROPGRGID
            wxs2IntPropertyWindow* Window;
            friend class wxs2IntPropertyWindow;
        #else
            wxPGId PGId;
            wxPGId Val1Id;
            wxPGId Val2Id;
        #endif
        
};

#endif // WXS2INTPROPERTY_H
