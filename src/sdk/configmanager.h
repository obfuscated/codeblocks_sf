#ifndef X_CONFIGMANAGER_H
#define X_CONFIGMANAGER_H

#include <wx/wx.h>
#include <wx/hashmap.h>
#include <wx/hashset.h>

#include "settings.h"

#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "manager.h"
#include "messagemanager.h"
#include "base64.h"


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
    WX_DECLARE_STRING_HASH_MAP( wxString, StringToStringMap);
    WX_DECLARE_HASH_MAP(long int, wxString, wxIntegerHash, wxIntegerEqual, IntToStringMap);
    WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, StringSet);

    WX_DECLARE_STRING_HASH_MAP(ISerializable *, SerializableObjectMap);
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

    static wxString app_path;
    static wxString data_path;
    static wxString config_folder;
    static wxString home_folder;

    ConfigManager(TiXmlElement* r);
    TiXmlElement* AssertPath(wxString& path);
    TiXmlElement* GetUniqElement(TiXmlElement* p, const wxString& q);
    void SetNodeText(TiXmlElement *n, const TiXmlText& t);

public:

    /* -----------------------------------------------------------------------------------------------------
    *  Utility functions for accessing files/folders in a system-wide, consistent way
    */
    static wxString LocateDataFile(const wxString& filename);

    static wxString GetHomeFolder();
    static wxString GetConfigFolder();
    static wxString GetPluginsFolder();
    static wxString GetScriptsFolder();
    static wxString GetDataFolder();
    static wxString GetExecutableFolder();
    static wxString GetProxy();

    static inline wxString ReadDataPath(){return GetDataFolder();};      // use instead of cfg->Read("data_path");
    static inline wxString ReadAppPath(){return GetExecutableFolder();}; // use instead of cfg->Read("app_path");
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
    *  WX_DECLARE_STRING_HASH_MAP(MySerializableClass *, MyMap);
    *  MyMap objMap;
    *  cfg->Read<MySerializableClass>("name", (ConfigManagerContainer::SerializableObjectMap*) &objMap);
    *  map["somekey"]->DoSomething();
    */
    void ConfigManager::Write(const wxString& name, const ConfigManagerContainer::SerializableObjectMap* map);
    template <class T> void Read(const wxString& name, ConfigManagerContainer::SerializableObjectMap *map)
    {
        wxString key(name);
        TiXmlHandle ph(AssertPath(key));
        TiXmlElement* e = 0;
        if(TiXmlNode *n = ph.FirstChild(key).FirstChild("objmap").Node())
            while(e = n->IterateChildren(e)->ToElement())
            {
                T *obj = new T;
                obj->SerializeIn(wxBase64::Decode(_U(e->FirstChild()->ToText()->Value())));
                (*map)[_U(e->Value())] = obj;
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

class DLLIMPORT CfgMgrBldr
{
    friend class ConfigManager;
    NamespaceMap namespaces;
    TiXmlDocument *doc;
    TiXmlDocument *volatile_doc;
    wxCriticalSection cs;
    bool r;

    CfgMgrBldr();
    ~CfgMgrBldr();
    void Close();
    static inline CfgMgrBldr* Instance();
    ConfigManager* Instantiate(const wxString& name_space);
public:
    static ConfigManager* Get(const wxString& name_space);
    void SwitchTo(const wxString& absFN);
    void SwitchToR(const wxString& absFN);
};


#endif

