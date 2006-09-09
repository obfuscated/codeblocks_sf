#include "wxscustomeditorproperty.h"

#include <wx/dialog.h>
#include <wx/bitmap.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

namespace   
{
    class wxsCustomEditorPropertyPropClass: public wxCustomPropertyClass
    {
        public:
            /** \brief Standard property editor */
            wxsCustomEditorPropertyPropClass(
                const wxString& label, 
                const wxString& name,
                wxsCustomEditorProperty* property,
                wxsPropertyContainer* object):
                    wxCustomPropertyClass(label,name),
                    Property(property),
                    Object(object)
            {}
            
            /** \brief Pointer to wxsProperty which created this
             *
             * Pointer will be used to call ShowEditor
             */
            wxsCustomEditorProperty* Property;
            wxsPropertyContainer* Object;
    };
}

void wxsCustomEditorProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId PGId = Grid->AppendIn(Parent,new wxsCustomEditorPropertyPropClass(GetPGName(),wxPG_LABEL,this,Object));
    Grid->SetPropertyAttribute(PGId,wxPG_CUSTOM_EDITOR,wxPGEditor_TextCtrlAndButton);
    Grid->SetPropertyAttribute(PGId,wxPG_CUSTOM_CALLBACK,(void*)PGCallback);
    Grid->SetPropertyValue(PGId,GetStr(Object));
    if ( !CanParseStr() )
    {
        Grid->LimitPropertyEditing(PGId);
    }
    PGRegister(Object,Grid,PGId);
}

bool wxsCustomEditorProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId PGId,long Index)
{
    return CanParseStr() && ParseStr(Object,Grid->GetPropertyValue(PGId).GetString());
}

bool wxsCustomEditorProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId PGId,long Index)
{
    Grid->SetPropertyValue(PGId,GetStr(Object));
    return true;
}

bool wxsCustomEditorProperty::PGCallback(wxPropertyGridManager* propGrid,wxPGProperty* property,wxPGCtrlClass* ctrl,long data)
{
    wxsCustomEditorPropertyPropClass* Prop = (wxsCustomEditorPropertyPropClass*)property;
    return Prop->Property->ShowEditor(Prop->Object);
}
