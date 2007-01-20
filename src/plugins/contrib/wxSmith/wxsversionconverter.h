#ifndef WXSVERSIONCONVERTER_H
#define WXSVERSIONCONVERTER_H

#include <tinyxml/tinyxml.h>
#include <wx/string.h>

class wxsProject;

/** \brief This class is responsible for converting data between different
 *         wxSmith versions
 *
 * This class may convert data from Old-wxSmith style and should help with
 * converting in never configuration versions
 *
 * \warning This class is stub only, no implementation
 */
class wxsVersionConverter
{
    public:

        /** \brief Checking if given configuratino node represents Old-wxSmith configuration */
        bool DetectOldConfig(TiXmlElement* ConfigNode,wxsProject* Project) const;

        /** \brief Converting data from Old-wxSmith configuration style
         * \param ConfigNode nde of Old-wxSmith style configuration (should be validated using DetectOldConfig first)
         * \param Doc Temporary document where valid wxSmith configuration node will be created
         * \return converted wxSmith configuration (inside Doc, so deleting Doc will delete it too)
         */
        TiXmlElement* ConvertFromOldConfig(TiXmlElement* ConfigNode,TiXmlDocument* Doc,wxsProject* Project) const;

        /** \brief Converting from older version
         * \param ConfigNode nde of older configuration
         * \param Doc Temporary document where valid wxSmith configuration node will be created
         * \return converted wxSmith configuration (inside Doc, so deleting Doc will delete it too)
         */
        TiXmlElement* Convert(TiXmlElement* ConfigNode,TiXmlDocument* Doc,wxsProject* Project) const;

        /** \brief Getting singleton object */
        static const wxsVersionConverter& Get();

    private:

        void ConvertOldWxsFile(const wxString& FileName,bool UsingXRC) const;
        void GatherExtraFromOldResourceReq(TiXmlElement* Object,TiXmlElement* Extra,bool RootNode) const;
        void AdoptOldSourceFile(const wxString& FileName,const wxString& ClassName) const;
        bool LineContainDirectivesOnly(const wxString& Code,int& BeginPos) const;

        wxsVersionConverter() {}
        ~wxsVersionConverter() {}
};

#endif
