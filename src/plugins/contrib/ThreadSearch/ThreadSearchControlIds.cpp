#include "ThreadSearchControlIds.h"

#include <wx/utils.h>

long ControlIDs::Get(IDs id)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        for (int ii = 0; ii < lastValue; ++ii)
            ids[ii] = wxNewId();
    }
    return ids[id];
}

ControlIDs controlIDs;
