#ifndef CUSTOMEDITORPROPERTY_H
#define CUSTOMEDITORPROPERTY_H

#include <wx/intl.h>
#include "wxsproperty.h"

/** \brief Class which may be used to create properties with custom editor */
class wxsCustomEditorProperty : public wxsProperty
{
    public:

        /** \brief Ctor with different names */
        wxsCustomEditorProperty(const wxString& PGName,const wxString& DataName,int Priority=100):
            wxsProperty(PGName,DataName,Priority) {}

        /** \brief Function opening property editor
         *
         * This function should open editor (in modal) for this property
         *
         * \return true  - property value has changed,
         *         false - property has not changed
         */
        virtual bool ShowEditor(wxsPropertyContainer* Object) =0;

    protected:

        /** \brief Function giving string representation of property */
        virtual wxString GetStr(wxsPropertyContainer* Object) { return _("Click to edit"); }

        /** \brief Function parsing string representation of property
         *
         * After parsing, data should be stored in Object
         * \return true - parsing done, false - parsing error
         */
        virtual bool ParseStr(wxsPropertyContainer* Object,wxString Value) { return false; }

        /** \brief Checkig if this property is able to parse string */
        virtual bool CanParseStr() { return false; }

    private:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        static bool PGCallback(wxPropertyGridManager* propGrid,wxPGProperty* property,wxPGCtrlClass* ctrl,long data);
};

#endif
