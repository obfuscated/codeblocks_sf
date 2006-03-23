#include "wxsemptyproperty.h"

wxsEmptyProperty::wxsEmptyProperty():
    wxsProperty(_T("Empty"),_T("empty"))
{
}

wxsEmptyProperty wxsEmptyProperty::Singleton;
