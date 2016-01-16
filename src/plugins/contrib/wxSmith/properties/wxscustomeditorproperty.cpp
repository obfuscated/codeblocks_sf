/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxscustomeditorproperty.h"

#include <prep.h>

#include <wx/dialog.h>
#include <wx/bitmap.h>
#include <wx/propgrid/propgrid.h>
#if !wxCHECK_VERSION(2, 9, 0)
#include <wx/propgrid/propdev.h>
#endif
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
            {
                SetEditor(wxPG_EDITOR(TextCtrlAndButton));
                SetValue(Property->GetStr(Object));
            }

            virtual bool OnEvent(
                wxPropertyGrid* propgrid,
                wxWindow* wnd_primary,
                wxEvent& event)
            {
                if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
                {
                    if(Property->ShowEditor(Object))
                    {
                        SetValueInEvent (Property->GetStr(Object));
                        return true;
                    }
                    return false;
                }
                return wxCustomPropertyClass::OnEvent(propgrid,wnd_primary,event);
            }

#if wxCHECK_VERSION(2,9,0)
            virtual wxString ValueToString(cb_unused wxVariant& value, cb_unused int argFlags = 0) const
#else
            virtual wxString GetValueAsString( int flags = 0 ) const
#endif
            {
                return Property->GetStr(Object);
            }

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
    if ( !CanParseStr() )
    {
        Grid->LimitPropertyEditing(PGId);
    }
    PGRegister(Object,Grid,PGId);
}

bool wxsCustomEditorProperty::PGRead(cb_unused wxsPropertyContainer* Object,
                                     wxPropertyGridManager*          Grid,
                                     wxPGId PGId,cb_unused long Index)
{
    return CanParseStr() && ParseStr(Object,Grid->GetPropertyValue(PGId).GetString());
}

bool wxsCustomEditorProperty::PGWrite(cb_unused wxsPropertyContainer*  Object,
                                      cb_unused wxPropertyGridManager* Grid,
                                      cb_unused wxPGId PGId,cb_unused long Index)
{
    return true;
}
