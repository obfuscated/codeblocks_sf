#ifndef WXSSTRINGLISTPROPERTY_H
#define WXSSTRINGLISTPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"


class wxsStringListProperty : public wxsProperty
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

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

		wxArrayString& Array;
		int* Selected;
		int SortedFlag;

        wxPGId PGId;
        wxPGId SelId;

        void BuildChoices(wxPropertyGrid* Grid);
        void RebuildChoices(wxPropertyGrid* Grid);
        bool IsSorted();
};

#endif
