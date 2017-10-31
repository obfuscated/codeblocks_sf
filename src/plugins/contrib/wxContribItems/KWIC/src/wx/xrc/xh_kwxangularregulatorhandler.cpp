/**************************************************************************//**
 * \file		xh_kwxangularregulatorhandler.cpp
 * \author	Gary Harris
 * \date		24/4/2010.
 *
 * This file is part of wxSmithKWIC.
 *
 * wxSmithKWIC - an add-on for wxSmith, Code::Blocks' GUI editor.					\n
 * Copyright (C) 2010 Gary Harris.
 *
 * wxSmithKWIC is free software: you can redistribute it and/or modify
 * it under the terms of the KWIC License
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * KWIC License for more details.
 *
 * You should have received a copy of the KWIC License along with this
 * program.  If not, see <http://www.koansoftware.com/kwic/kwic-license.htm>.
 *
 *****************************************************************************/
#include "wx/xrc/xh_kwxangularregulatorhandler.h"
#include "wx/KWIC/AngularRegulator.h"

// Register with wxWidgets' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(kwxAngularRegulatorHandler, wxXmlResourceHandler)

kwxAngularRegulatorHandler::kwxAngularRegulatorHandler()
{
    // this call adds support for all wxWindows class styles
    // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
    AddWindowStyles();

    // if MyControl class supports e.g. MYCONTROL_DEFAULT_STYLE
    // you should use:
    //     XRC_ADD_STYLE(MYCONTROL_DEFAULT_STYLE);
}

wxObject *kwxAngularRegulatorHandler::DoCreateResource()
{
    // the following macro will init a pointer named "control"
    // with a new instance of the MyControl class, but will NOT
    // Create() it!
    XRC_MAKE_INSTANCE(control, kwxAngularRegulator)

    control->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), wxBORDER_NONE);
    control->SetRange(GetLong(wxT("range_min"), 0), GetLong(wxT("range_max"), 220));
	control->SetAngle(GetLong(wxT("angle_min"), -20), GetLong(wxT("angle_max"), 200));
    control->SetExtCircleColour(GetColour(wxT("external_circlecolour"), *wxLIGHT_GREY));
    control->SetIntCircleColour(GetColour(wxT("internal_circle_colour"), *wxLIGHT_GREY));
    control->SetKnobBorderColour(GetColour(wxT("knob_border_colour"), *wxBLACK));
    control->SetKnobColour(GetColour(wxT("knob_colour"), *wxLIGHT_GREY));
    control->SetLimitsColour(GetColour(wxT("range_tag_colour"), *wxBLACK));
    control->SetTagsColour(GetColour(wxT("tag_colour"), *wxBLACK));
	int i = 1;
	while(1){
		wxString s = wxString::Format(wxT("tag_%d_value"), i);
		if(!HasParam(s)){
			break;
		}
		control->AddTag(GetLong(s));
		i++;
	}
    control->SetValue(GetLong(wxT("value"), 0));

    SetupWindow(control);

    return control;
}

bool kwxAngularRegulatorHandler::CanHandle(wxXmlNode *node)
{
    // this function tells XRC system that this handler can parse
    // the <object class="MyControl"> tags
    return IsOfClass(node, wxT("kwxAngularRegulator"));
}
