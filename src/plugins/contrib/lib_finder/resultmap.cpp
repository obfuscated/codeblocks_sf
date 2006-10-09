#include <wx/arrstr.h>

#include "resultmap.h"

ResultMap::ResultMap()
{
}

ResultMap::~ResultMap()
{
    Clear();
}

void ResultMap::Clear()
{
    for ( ResultHashMap::iterator it = Map.begin(); it != Map.end(); ++it )
    {
        ResultArray& RA = it->second;
        for ( size_t i = 0; i<RA.Count(); ++i )
        {
            delete RA[i];
        }
    }
    Map.clear();
}

void ResultMap::GetAllResults(ResultArray& Array)
{
    for ( ResultHashMap::iterator it = Map.begin(); it != Map.end(); ++it )
    {
        ResultArray& RA = it->second;
        for ( size_t i = 0; i<RA.Count(); ++i )
        {
            Array.Add(RA[i]);
        }
    }
}

void ResultMap::GetGlobalVarNames(wxArrayString& Array)
{
    for ( ResultHashMap::iterator it = Map.begin(); it != Map.end(); ++it )
    {
        Array.Add(it->first);
    }
}

ResultMap ResultMap::Singleton;
