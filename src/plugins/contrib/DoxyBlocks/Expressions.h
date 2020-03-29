/**************************************************************************//**
 * \file      Expressions.h
 * \author    Gary Harris
 * \date      02-03-2010
 *
 * DoxyBlocks - doxygen integration for Code::Blocks. \n
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
 *----------------------------------------------------------------------------------------------- \n
 * This file was pilfered from doxbar and modified to suit DoxyBlocks. \n
 * The original file credits follow.
 *
 * author  Olivier Sannier                                             \n
 * date    13-08-2003
 *
 * This file contains all the regular expressions used in the program. \n
 * They have been grouped here to allow easy correction
 *
 *****************************************************************************/
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <wx/regex.h>

/** \brief The regex for a class declaration.
*/
wxRegEx reClass(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "class[[:space:]]+"             // The word class, followed by some space (at least one).
    "([^[:space:]\\:]+)"            // The class name (made of no space and no colon).
    ));

/** \brief The regex for a struct declaration.
*/
wxRegEx reStruct(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "(typedef"                      // Maybe a typedef declaration.
    "[[:space:]]+)?"                // Followed by at least one space.
    "struct[[:space:]]+"            // The word struct, followed by some space (at least one).
    "([^[:space:]\\:\\{\\;]+)"      // The struct name (made of no space, no colon, no bracket, no semicolon).
    ));


/** \brief The regex for a typedef declaration.
*/
wxRegEx reTypedef(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "typedef[[:space:]]+"           // The typedef word followed by at least one space.
    "(.+)[[:space:]]+"              // The original type, followed by at least one space.
    "([^\\{[:space:];]+)"           // The new type, not made of any space nor braces, nor semicolon.
    ));

/** \brief The regex for an enum declaration.
*/
wxRegEx reEnum(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "enum[[:space:]]+"              // The word class, followed by some space (at least one).
    "([^[:space:]\\:]+)"            // The class name (made of no space and no colon).
    ));

/** \brief The regex for a function declaration.
 *
 * Matches: return_type function(type param, type param, ...)
 * \note This declaration generates a false warning in doxygen regarding undocumented return type.
 */
wxRegEx reFunction(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "((.+)[[:space:]])"             // The return type of the function.
    "([[:space:]]*)"                // Possibly some space after the name.
    "([^[:space:]]+)\\("            // The name of the function, followed by a "(".
    "([^)]*)?"                      // The function's parameters.
    "\\)"                           // The closing parenthesis.
    ),
#ifndef __WXMAC__
    wxRE_ADVANCED);
#else
    wxRE_EXTENDED);
#endif

/** \brief The regex for a class function declaration.
 *
 * Matches: return_type class::function(type param, type param, ...)
 * \note This declaration generates a false warning in doxygen regarding undocumented return type.
 */
wxRegEx reClassFunction(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "((.+)[[:space:]])"             // The return type of the function.
    "([[:space:]]*)"                // Some space after the return type.
    "([^[:space:]]+)\\::"           // The class name followed by "::".
    "([^[:space:]]+)\\("            // The name of the function, followed by a "(".
    "([^)]*)?"                      // The function's parameters.
    "\\)"                           // The closing parenthesis.
    ),
#ifndef __WXMAC__
    wxRE_ADVANCED);
#else
    wxRE_EXTENDED);
#endif

/** \brief The regex for a class function declaration without a return value, such as a constructor.
 *
 * Matches: class::function(type param, type param, ...)
 * \note This declaration generates a false warning in doxygen regarding undocumented return type.
 */
wxRegEx reClassFunctionNoRet(wxT(
    "^[[:space:]]*"                 // Space at the beginning of the line.
    "([^[:space:]]+)\\::"           // The class name followed by "::".
    "([^[:space:]]+)\\("            // The name of the function, followed by a "(".
    "([^)]*)?"                      // The function's parameters.
    "\\)"                           // The closing parenthesis.
    ));

#endif
