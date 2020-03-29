/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#ifndef MACROS_H
#define MACROS_H
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#if defined(_APPLE_) || defined(_MACH_)
#define OS_MAC
#endif

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#define OS_WIN
#endif

#ifndef OS_MAC
#ifndef OS_WIN
#define OS_UNIX
#endif
#endif

#define SHOW_MODE_ONELINE
//#define TARGET_WDIR_ENABLED

#ifndef NOMINMAX
#define NOMINMAX
namespace std {

template <typename T1, typename T2>
inline T1 const& min (T1 const& a, T2 const& b)
{
 return (((a)<(b))?(a):(b));
}

template <typename T1, typename T2>
inline T1 const& max (T1 const& a, T2 const& b)
{
 return (((a)>(b))?(a):(b));
}

}
#endif

#endif
//------------------------------------------------------------------------------
