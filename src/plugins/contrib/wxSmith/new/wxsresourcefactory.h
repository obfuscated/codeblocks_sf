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
    
    private:
    
        /** \brief Ctor is private - class can not be created outside, 
         *         only singleton taken 
         */
        wxsResourceFactory() {}
        
        /** \brief Pointer to singleton */
        static wxsResourceFactory Singleton;
};

#endif
