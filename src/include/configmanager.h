#ifndef X_CONFIGMANAGER_H
#define X_CONFIGMANAGER_H

#include <wx/hashmap.h>
#include <wx/hashset.h>

#include "settings.h"
#include "globals.h"

#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "manager.h"
#include "messagemanager.h"
#include "base64.h"

#undef new
#include <map>
#include <set>


/* ------------------------------------------------------------------------------------------------------------------
*  Interface Serializable
*  ConfigManager can save arbitrary objects and even sets/maps of objects, provided they implement Serializable.
*
*  Usage:
*  ------
*   class MySerializableLongIntClass : public ISerializable
*   {
*   //...
*   wxString SerializeOut() const {wxString tmp; tmp << m_int; return tmp;};
*   void SerializeIn(const wxString& s){s.ToLong(&m_int);};
*   //...
*   long int m_int;
*   };
*/
class ISerializable
{
public:
    ISerializable();
    virtual ~ISerializable();
    virtual wxString SerializeOut() const = 0;
    virtual void SerializeIn(const wxString& s) = 0;
};


/* ------------------------------------------------------------------------------------------------------------------
*  Containers supported by ConfigManager
*/
namespace ConfigManagerContainer
{
    typedef std::map<wxString, wxString> StringToStringMap;
    typedef std::map<int, wxString> IntToStringMap;
    typedef std::set<wxString> StringSet;;

    typedef std::map<wxString, ISerializable*> SerializableObjectMap;
};


/* ------------------------------------------------------------------------------------------------------------------*/
/** Search dirs values. Used as a bitmask in ConfigManager::LocateDataFile() and friends.*/
enum SearchDirs
{
    sdHome            = 0x0001, ///< User's home directory
    sdBase            = 0x0002, ///< Code::Blocks' installation base
    sdTemp            = 0x0004, ///< System-wide temp folder
    sdPath            = 0x0008, ///< All dirs in the PATH environment variable
    sdConfig          = 0x0010, ///< Config folder
    sdCurrent         = 0x0020, ///< Current working folder

    sdPluginsUser     = 0x0100, ///< Plugins folder in user's dir
    sdScriptsUser     = 0x0200, ///< Scripts folder in user's dir
    sdDataUser        = 0x0400, ///< Data folder in user's dir

    sdAllUser         = 0x0f00, ///< Convenience value meaning "all sd*User values"

    sdPluginsGlobal   = 0x1000, ///< Plugins folder in base dir
    sdScriptsGlobal   = 0x2000, ///< Scripts folder in base dir
    sdDataGlobal      = 0x4000, ///< Data folder in base dir

    sdAllGlobal       = 0xf000, ///< Convenience value meaning "all sd*Global values"

    sdAllKnown        = 0xffff, ///< All known dirs (i.e. all of the above)
};



/* ------------------------------------------------------------------------------------------------------------------
*  ConfigManager class
*/
class DLLIMPORT ConfigManager
{
    friend class CfgMgrBldr;

    TiXmlDocument *doc;
    TiXmlElement* root;
    TiXmlElement* pathNode;

    ConfigManager(TiXmlElement* r);
    TiXmlElement* AssertPath(wxString& path);
    TiXmlElement* GetUniqElement(TiXmlElement* p, const wxString& q);
    void SetNodeText(TiXmlElement *n, const TiXmlText& t);
    inline void Collapse(wxString& str) const;
    wxString InvalidNameMessage(const wxString& what, const wxString& sub, TiXmlElement *localPath) const;
    static void InitPaths();

    static wxString config_folder;
    static wxString home_folder;
    static wxString data_path_user;
    static wxString data_path_global;
    static wxString app_path;
    static wxString temp_folder;
    static bool relo;

public:

    /* -----------------------------------------------------------------------------------------------------
    *  Utility functions for accessing files/folders in a system-wide, consistent way
    * -----------------------------------------------------------------------------------------------------*/


    /** @brief Locate a file in an installation- and platform-independent way.
    *
    * You should always use this function if you are looking for "some arbitrary file that belongs to Code::Blocks",
    * as it works across platforms without any additional effort from your side, and it has some builtin redundancy.
    * @par
    * So, code that looked like this in the old days:
    * @code
    * wxString some_file = ConfigManager::GetScriptsFolder() + wxFILE_SEP_PATH + _T("startup.script");
    * @endcode
    * should be converted to this:
    * @code
    * wxString some_file = ConfigManager::LocateDataFile(_T("startup.script"), sdScriptsUser | sdScriptsGlobal);
    * @endcode
    * This would try to locate the file named "startup.script" in the global and also in the user's scripts folders.
    * @note User's dirs @b always have precedence over global dirs.
    *
    * @param filename name of the file to search for
    * @param search_dirs A bit-mask of the folders to include in the search.
    */
    static wxString LocateDataFile(const wxString& filename, int search_dirs = sdAllKnown);

    /** @brief Access one of Code::Blocks' folders.
      * @param dir The directory to return.
      */
    static wxString GetFolder(SearchDirs dir);

    /* Backwards compatible functions. For new code, please use GetFolder() instead.
    *
    * Query "standard" paths that work across platforms.
    * NEVER harcode a path like "C:\CodeBlocks\share\data". Always use one of the following functions to compose a path.
    */
    static wxString GetHomeFolder() { return home_folder; }
    static wxString GetConfigFolder(){ return config_folder; }
    static wxString GetPluginsFolder(bool global = true){ return GetFolder(global ? sdPluginsGlobal : sdPluginsUser); }
    static wxString GetScriptsFolder(bool global = true){ return GetFolder(global ? sdScriptsGlobal : sdScriptsUser); }
    static wxString GetDataFolder(bool global = true){ return global ? data_path_global : data_path_user; }
    static wxString GetExecutableFolder(){ return app_path; }
    static wxString GetTempFolder(){ return GetFolder(sdTemp); }

    /*
    *  Network proxy for HTTP/FTP transfers
    */
    static wxString GetProxy();

    /*
    *  Builtin revision information
    */
    static wxString GetRevisionString();
    static unsigned int GetRevisionNumber();
    static wxString GetSvnDate();

    static inline wxString ReadDataPath(){return GetDataFolder();}      // use instead of cfg->Read("data_path");
    static inline wxString ReadAppPath(){return GetExecutableFolder();} // use instead of cfg->Read("app_path");



    /* -----------------------------------------------------------------------------------------------------
    *  Path functions for navigation within your configuration namespace
    */
    wxString GetPath() const;
    void SetPath(const wxString& strPath);
    wxArrayString EnumerateSubPaths(const wxString& path);
    wxArrayString EnumerateKeys(const wxString& path);
    void DeleteSubPath(const wxString& strPath);

    /* -----------------------------------------------------------------------------------------------------
    *  Clear all nodes from your namespace or delete the namespace (removing it from the config file).
    *  WARNING: After Delete() returns, the pointer to your instance is invalid. Before you can call ANY member
    *  function of this class, you have to call Manager::Get()->GetConfigManager() for a valid reference again.
    *  Note that Delete() is inherently thread-unsafe. You delete an entire namespace of data as well as the object
    *  responsible of handling that data! Make sure you know what you do.
    *  This is even more true for DeleteAll() which you should really NEVER use.
    */
    void Clear();
    void Delete();
    void DeleteAll();

    /* -----------------------------------------------------------------------------------------------------
    *  Standard primitives
    */
    void Write(const wxString& name,  const wxString& value, bool ignoreEmpty = false);
    wxString Read(const wxString& key, const wxString& defaultVal = wxEmptyString);
    bool Read(const wxString& key, wxString* str);
    void Write(const wxString& key, const char* str);

    void Write(const wxString& name,  int value);
    bool Read(const wxString& name,  int* value);
    int  ReadInt(const wxString& name,  int defaultVal = 0);

    void Write(const wxString& name,  bool value);
    bool Read(const wxString& name,  bool* value);
    bool ReadBool(const wxString& name,  bool defaultVal = false);

    void Write(const wxString& name,  double value);
    bool Read(const wxString& name,  double* value);
    double ReadDouble(const wxString& name,  double defaultVal = 0.0f);

    /* -----------------------------------------------------------------------------------------------------
    *  Set and unset keys, or test for existence. Note that these functions cannot be used to remove paths
    *  or test existence of paths (it may be used to implicitely create paths, though).
    */
    bool Exists(const wxString& name);
    void Set(const wxString& name);
    void UnSet(const wxString& name);

    /* -----------------------------------------------------------------------------------------------------
    *  Compound objects
    */
    void Write(const wxString& name,  const wxArrayString& as);
    void Read(const wxString& name,  wxArrayString* as);
    wxArrayString ReadArrayString(const wxString& name);

    void WriteBinary(const wxString& name,  const wxString& source);
    void WriteBinary(const wxString& name,  void* ptr, size_t len);
    wxString ReadBinary(const wxString& name);

    void Write(const wxString& name,  const wxColour& c);
    bool Read(const wxString& name, wxColour* value);
    wxColour ReadColour(const wxString& name, const wxColour& defaultVal = *wxBLACK);


    /* -----------------------------------------------------------------------------------------------------
    *  Single serializable objects
    */
    void Write(const wxString& name, const ISerializable& object);
    bool Read(const wxString& name, ISerializable* object);


    /* -----------------------------------------------------------------------------------------------------
    *  Maps and sets of primitive types
    */
    void Write(const wxString& name, const ConfigManagerContainer::StringToStringMap& map);
    void Read(const wxString& name, ConfigManagerContainer::StringToStringMap* map);
    ConfigManagerContainer::StringToStringMap ReadSSMap(const wxString& name);

    void Write(const wxString& name, const ConfigManagerContainer::IntToStringMap& map);
    void Read(const wxString& name, ConfigManagerContainer::IntToStringMap* map);
    ConfigManagerContainer::IntToStringMap ReadISMap(const wxString& name);

    void Write(const wxString& name, const ConfigManagerContainer::StringSet& set);
    void Read(const wxString& name, ConfigManagerContainer::StringSet* map);
    ConfigManagerContainer::StringSet ReadSSet(const wxString& name);


    /* -----------------------------------------------------------------------------------------------------
    *  Maps of serialized objects. You are responsible for deleting the objects in the map/set.
    *
    *
    *  Usage:
    *  ------
    *  typedef std::map<wxString, MySerializableClass *> MyMap;
    *  MyMap objMap;
    *  cfg->Read("name", &objMap);
    *  map["somekey"]->DoSomething();
    */
    void Write(const wxString& name, const ConfigManagerContainer::SerializableObjectMap* map);

    template <typename T> void Read(const wxString& name, std::map<wxString, T*> *map)
    {
        wxString key(name);
        TiXmlHandle ph(AssertPath(key));
        TiXmlElement* e = 0;
        if(TiXmlNode *n = ph.FirstChild(key.mb_str(wxConvUTF8)).FirstChild("objmap").Node())
            while(n->IterateChildren(e) && (e = n->IterateChildren(e)->ToElement()))
            {
                T *obj = new T;
                obj->SerializeIn(wxBase64::Decode(cbC2U(e->FirstChild()->ToText()->Value())));
                (*map)[cbC2U(e->Value())] = obj;
            }
    };
};








/* ------------------------------------------------------------------------------------------------------------------
*  "Builder pattern" class for ConfigManager
*
*  ################################################################
*  ###### Do not use this class. Do not even think about it. ######
*  ################################################################
*
*  --->  use Manager::Get()->GetConfigManager("yournamespace") instead.
*
*  Manager::Get()->GetConfigManager("yournamespace") is guaranteed to always work while
*  the builder class and its interfaces may be changed any time without prior notice.
*/

WX_DECLARE_STRING_HASH_MAP(ConfigManager*, NamespaceMap);

class DLLIMPORT CfgMgrBldr : public Mgr<CfgMgrBldr>
{
    friend class ConfigManager;
    friend class Mgr<CfgMgrBldr>;
    NamespaceMap namespaces;
    TiXmlDocument *doc;
    TiXmlDocument *volatile_doc;
    wxCriticalSection cs;
    bool r;
    wxString cfg;

    void Close();
    void SwitchTo(const wxString& absFN);
    void SwitchToR(const wxString& absFN);
    ConfigManager* Build(const wxString& name_space);
    wxString FindConfigFile(const wxString& filename);
    wxString DetermineExecutablePath();
protected:
    CfgMgrBldr();
    ~CfgMgrBldr();

public:
    static ConfigManager* GetConfigManager(const wxString& name_space);
};

#endif

