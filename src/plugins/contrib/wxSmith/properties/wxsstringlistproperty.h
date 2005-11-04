#ifndef WXSSTRINGLISTPROPERTY_H
#define WXSSTRINGLISTPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"


class WXSCLASS wxsStringListProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsStringListProperty(wxsProperties* Properties,wxArrayString& Array);

        /** Ctor */
		wxsStringListProperty(wxsProperties* Properties,wxArrayString& Array,int& Selected,int SortedFlag=0);

		/** DCtor */
		virtual ~wxsStringListProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        #ifdef __NO_PROPGRGID
            virtual wxWindow* BuildEditWindow(wxWindow* Parent);
            virtual void UpdateEditWindow();
            void EditList();
        #else
            virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
            virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
            virtual void UpdatePropGrid(wxPropertyGrid* Grid);
        #endif

	private:

		wxArrayString& Array;
		int* Selected;
		int SortedFlag;

		#ifdef __NO_PROPGRGID
            friend class wxsStringListPropertyWindow;
        #else
            wxPGId PGId;
            wxPGId SelId;

            void BuildChoices(wxPropertyGrid* Grid);
            void RebuildChoices(wxPropertyGrid* Grid);
            bool IsSorted();
        #endif
};

#endif
