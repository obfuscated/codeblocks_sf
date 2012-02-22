/**************************************************************************//**
 * \file		xh_kwxangularmeterhandler.cpp
 * \author	Gary Harris
 * \date		23/4/2010.
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
#include "wx/xrc/xh_kwxangularmeterhandler.h"
#include "wx/KWIC/AngularMeter.h"

// Register with wxWidgets' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(kwxAngularMeterHandler, wxXmlResourceHandler)

kwxAngularMeterHandler::kwxAngularMeterHandler()
{
    // this call adds support for all wxWindows class styles
    // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
    AddWindowStyles();

    // if MyControl class supports e.g. MYCONTROL_DEFAULT_STYLE
    // you should use:
    //     XRC_ADD_STYLE(MYCONTROL_DEFAULT_STYLE);
}

wxObject *kwxAngularMeterHandler::DoCreateResource()
{
    // the following macro will init a pointer named "control"
    // with a new instance of the MyControl class, but will NOT
    // Create() it!
    XRC_MAKE_INSTANCE(control, kwxAngularMeter)

    control->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize());
    control->SetNumTick(GetLong(wxT("num_ticks")));
    control->SetRange(GetLong(wxT("range_min"), 0), GetLong(wxT("range_max"), 220));
	control->SetAngle(GetLong(wxT("angle_min"), -20), GetLong(wxT("angle_max"), 200));
	int i = 1;
	while(1){
		wxString s = wxString::Format(wxT("sector_%d_colour"), i);
		if(!HasParam(s)){
			break;
		}
        // Setting the number of sectors each time around is not ideal but the alternative is to pre-process the XML.
        control->SetNumSectors(i);
		control->SetSectorColor(i - 1, GetColour(s, *wxWHITE));
		i++;
	}
   	control->DrawCurrent(GetBool(wxT("show_value"), true));
    control->SetNeedleColour(GetColour(wxT("needle_colour"), *wxRED));
    control->SetBackColour(GetColour(wxT("background_colour"), control->GetBackgroundColour()));
	control->SetBorderColour(GetColour(wxT("border_colour"), control->GetBackgroundColour()));
	// Avoid error if the font node isn't present.
	if(HasParam(wxT("font"))){
		wxFont font = GetFont();
		control->SetTxtFont(font);
	}
    control->SetValue(GetLong(wxT("value"), 0));

    SetupWindow(control);

    return control;
}

bool kwxAngularMeterHandler::CanHandle(wxXmlNode *node)
{
    // this function tells XRC system that this handler can parse
    // the <object class="MyControl"> tags
    return IsOfClass(node, wxT("kwxAngularMeter"));
}
