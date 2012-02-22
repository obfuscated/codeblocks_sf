/**************************************************************************//**
 * \file		xh_kwxlcddisplayhandler.cpp
 * \author	Gary Harris
 * \date		19/4/2010.
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
#include "wx/xrc/xh_kwxlcddisplayhandler.h"
#include "wx/KWIC/LCDWindow.h"

// Register with wxWidgets' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(kwxLCDDisplayHandler, wxXmlResourceHandler)

kwxLCDDisplayHandler::kwxLCDDisplayHandler()
{
    // this call adds support for all wxWindows class styles
    // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
    AddWindowStyles();

    // if MyControl class supports e.g. MYCONTROL_DEFAULT_STYLE
    // you should use:
    //     XRC_ADD_STYLE(MYCONTROL_DEFAULT_STYLE);
}

wxObject *kwxLCDDisplayHandler::DoCreateResource()
{
    // the following macro will init a pointer named "control"
    // with a new instance of the MyControl class, but will NOT
    // Create() it!
    XRC_MAKE_INSTANCE(control, kwxLCDDisplay)

    // this is the point where you'll typically need to do the most
    // important changes: here the control is created and initialized.
    // You'll want to use the wxXmlResourceHandler's getters to
    // do most of your work.
    // If e.g. the MyControl::Create function looks like:
    //
    // bool MyControl::Create(wxWindow *parent, int id,
    //                        const wxBitmap &first, const wxPoint &posFirst,
    //                        const wxBitmap &second, const wxPoint &posSecond,
    //                        const wxString &theTitle, const wxFont &titleFont,
    //                        const wxPoint &pos, const wxSize &size,
    //                        long style = MYCONTROL_DEFAULT_STYLE,
    //                        const wxString &name = wxT("MyControl"));
    //
    // Then the XRC for your component should look like:
    //
    //    <object class="MyControl" name="some_name">
    //      <first-bitmap>first.xpm</first-bitmap>
    //      <second-bitmap>text.xpm</second-bitmap>
    //      <first-pos>3,3</first-pos>
    //      <second-pos>4,4</second-pos>
    //      <the-title>a title</the-title>
    //      <title-font>
    //        <!-- Standard XRC tags for a font: <size>, <style>, <weight>, etc -->
    //      </title-font>
    //      <!-- XRC also accepts other usual tags for wxWindow-derived classes:
    //           like e.g. <name>, <style>, <size>, <position>, etc -->
    //    </object>
    //
    // And the code to read your custom tags from the XRC file is just:

    control->Create(m_parentAsWindow, GetPosition(), GetSize());
    control->SetNumberDigits(GetLong(wxT("num_digits")));
    control->SetValue(GetText(wxT("value")));
    control->SetLightColour(GetColour(wxT("active_colour"), wxColour(0, 255, 0)));
    control->SetGrayColour(GetColour(wxT("inactive_colour"), wxColour(0, 64, 0)));

    SetupWindow(control);

    return control;
}

bool kwxLCDDisplayHandler::CanHandle(wxXmlNode *node)
{
    // this function tells XRC system that this handler can parse
    // the <object class="MyControl"> tags
    return IsOfClass(node, wxT("kwxLCDDisplay"));
}
