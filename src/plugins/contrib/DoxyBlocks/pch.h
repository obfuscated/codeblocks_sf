/**************************************************************************//**
 * \file		pch.h
 * \author	Gary Harris
 * \date		01-02-2010
 *
 * DoxyBlocks - doxygen integration for Code::Blocks.					\n
 * Copyright (C) 2010 Gary Harris.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef DOXYBLOCKS_PCH_H
#define DOXYBLOCKS_PCH_H

#if ( !defined(WX_PRECOMP) )
    #define WX_PRECOMP
#endif

// basic wxWidgets headers
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// #include other rarely changing headers here

#endif

