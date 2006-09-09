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

        /** \brief Function generating resource class */
        static wxsResource* Build(const wxString& ResourceType,wxsProject* Project);

        /** \brief Checking if given resource type can be main resource of application */
        static bool CanBeMain(const wxString& ResourceType);

        /** \brief Checking if factory can handle given file as external resource */
        static bool CanHandleExternal(const wxString& FileName);

        /** \brief Building external resource object */
        static wxsResource* BuildExternal(const wxString& FileName);

        /** \brief Building wxSmith menu enteries */
        static void BuildSmithMenu(wxMenu* menu);

        /** \brief Building module menu enteries */
        static void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data);

        /** \brief Building toolbar */
        static void BuildToolBar(wxToolBar* toolBar);

    protected:

        /** \brief Ctor */
        wxsResourceFactory();

        /** \brief Dctor */
        virtual ~wxsResourceFactory();

        /** \brief Getting number of resouce types inside this factory */
        virtual int OnGetCount() = 0;

        /** \brief Getting name of resource */
        virtual wxString OnGetName(int Number) = 0;

        /** \brief Checking if given resource can be main in application */
        virtual void OnGetInfo(int Number,wxString& Name,wxString& GUI,bool& CanBeMain) = 0;

        /** \brief creating resource */
        virtual wxsResource* OnCreate(int Number,wxsProject* Project) = 0;

        /** \brief Checking if factory can handle given file as external resource */
        virtual bool OnCanHandleExternal(const wxString& FileName) { return false; }

        /** \brief Building external resource object */
        virtual wxsResource* OnBuildExternal(const wxString& FileName) { return NULL; }

        /** \brief Function allowing factory to create extra menu enteries in wxSmith menu */
        virtual void OnBuildSmithMenu(wxMenu* Menu) {}

        /** \brief Function allowing factory to create extra module menu enteries */
        virtual void OnBuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data) {}

        /** \brief Function allowing factory to create etra toolbar items */
        virtual void OnBuildToolBar(wxToolBar* toolBar) {}

    private:

        struct ResourceInfo
        {
            wxsResourceFactory* m_Factory;
            int m_Number;
            bool m_CanBeMain;
            wxString m_GUI;
            ResourceInfo(): m_Factory(NULL), m_Number(0), m_CanBeMain(false) {}
        };

        WX_DECLARE_STRING_HASH_MAP(ResourceInfo,HashT);

        wxsResourceFactory* m_Next;
        static wxsResourceFactory* m_UpdateQueue;
        static wxsResourceFactory* m_Initialized;
        static HashT m_Hash;
        static wxString m_LastExternalName;
        static wxsResourceFactory* m_LastExternalFactory;

        static void InitializeFromQueue();
        inline void Initialize();

};

#endif
