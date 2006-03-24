#ifndef WXSRESOURCEFACTORY_H
#define WXSRESOURCEFACTORY_H

#include <wx/string.h>

class wxsResource;
class wxsProject;

/** \brief This class is responsible for creating resource classes
 *
 * This class is provided as singleton
 */
class wxsResourceFactory
{
    public:

        /** \brief Getting singleton instance */
        static wxsResourceFactory* Get() { return &Singleton; }

        /** \brief Function generating resource class */
        wxsResource* Build(const wxString& ResourceType,wxsProject* Project);

        /** \brief Getting number of supported resources */
        int GetResTypesCnt();

        /** \brief Getting name of given resource
         *
         * \note previous wxSmith version used other names for resources than this,
         *       only new ones are enumerated
         */
        wxString GetResType(int No);

        /** \brief Getting name of resource node in resource browser */
        wxString GetResBrowserName(int No);

    private:

        /** \brief Ctor is private - class can not be created outside,
         *         only singleton taken
         */
        wxsResourceFactory() {}

        /** \brief Pointer to singleton */
        static wxsResourceFactory Singleton;
};

#endif
