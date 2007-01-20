/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

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
