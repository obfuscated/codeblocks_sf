#ifndef RESULTMAP_H
#define RESULTMAP_H

#include <wx/dynarray.h>
#include <wx/hashmap.h>

#include "libraryresult.h"

class wxArrayString;
class wxString;

WX_DEFINE_ARRAY(LibraryResult*,ResultArray);

class ResultMap
{
    public:
        ResultMap();
        virtual ~ResultMap();

        /** @brief Clearing all results */
        void Clear();

        /** @brief Getting array associated with specified variable name */
        ResultArray& GetGlobalVar(const wxString& Name) { return Map[Name]; }

        /** @brief Getting all results */
        void GetAllResults(ResultArray& Array);

        /** @brief Getting array of used variable names */
        void GetGlobalVarNames(wxArrayString& Names);

        /** @brief getting singleton object */
        static ResultMap* Get() { return &Singleton; }

    private:

        WX_DECLARE_STRING_HASH_MAP(ResultArray,ResultHashMap);
        ResultHashMap Map;
        static ResultMap Singleton;
};

#define RM() ResultMap::Get()

#endif // RESULTMAP_H
