/**************************************************************************//**
 * \file		xh_kwxangularmeterhandler.h
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
#ifndef XH_KWXANGULARMETERHANDLER_H_INCLUDED
#define XH_KWXANGULARMETERHANDLER_H_INCLUDED

#include <wx/xrc/xmlres.h>

class kwxAngularMeterHandler : public wxXmlResourceHandler
{
public:
    // Constructor.
    kwxAngularMeterHandler();

    // Creates the control and returns a pointer to it.
    virtual wxObject *DoCreateResource();

    // Returns true if we know how to create a control for the given node.
    virtual bool CanHandle(wxXmlNode *node);

    // Register with wxWidgets' dynamic class subsystem.
    DECLARE_DYNAMIC_CLASS(kwxAngularMeterHandler)
};


#endif // XH_KWXANGULARMETERHANDLER_H_INCLUDED
