#ifndef PREDEFINEDIDS_H
#define PREDEFINEDIDS_H

#include <wx/string.h>

/** \brief Class managing predefined identifiers
 *
 * There are two types of predefined ids - first is set of identifiers
 * used internally inside wxWidgets, second is when number is given instead
 * of string for identifier.
 *
 * \note This class contains static functions only - there's no instance of it.
 */
class wxsPredefinedIDs
{
    public:

        /** \brief Function checking if identifier is predefined
         * \return true Id is predefined, false otherwise
         */
        static bool Check(const wxString& Name);

        /** \brief Getting value of predefined id
         * \return id value or wxID_ANY if it's not predefined id
         */
        static wxWindowID Value(const wxString& Name);

};

#endif
