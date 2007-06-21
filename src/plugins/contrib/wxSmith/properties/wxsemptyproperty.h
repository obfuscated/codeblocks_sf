#ifndef WXSEMPTYPROPERTY_H
#define WXSEMPTYPROPERTY_H

#include "wxsproperty.h"

/** \brief Empty property with no meaning
 *
 * This property is used to avoid failures when 0 pointers are supplied.
 * It should only avoid seg faults and currently there's no other purpose.
 * It's given as singleton class, can not be created.
 *
 * To get an instance, use Get() function
 */
class wxsEmptyProperty: public wxsProperty
{
    public:

        /** \brief Getting instance of empty property */
        static inline wxsEmptyProperty& Get() { return Singleton; }

    private:
        wxsEmptyProperty();
        virtual const wxString GetTypeName() { return _T(""); }
        static wxsEmptyProperty Singleton;
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring empty property (totally useless,
 *         currently no purpose)
 */
#define WXS_VOID() \
    Property(wxsEmptyProperty::Get(),0);

/** \} */


#endif
