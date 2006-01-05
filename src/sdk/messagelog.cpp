/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date: 2005-10-19 16:50:49 +0300 (ΞΒΞ’ΒΞβ€™Ξ’Β¤ΞΒΞ’ΒΞβ€™Ξ’ΒµΞΒΞ’ΒΞΒ²Ξ²β€Β¬Ξ’Β, 19 ΞΒΞ’ΒΞβ€™Ξ’ΒΞΒΞ’ΒΞΒΞ’ΒΞΒΞ’ΒΞΒ²Ξ²β€Β¬Ξ’Β 2005) $
*/

#include "sdk_precomp.h"
#include "messagelog.h" // class's header file
#include "manager.h"

// class constructor
MessageLog::MessageLog()
    : wxPanel(Manager::Get()->GetAppWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    m_PageId = -1;
}

// class destructor
MessageLog::~MessageLog()
{
}
