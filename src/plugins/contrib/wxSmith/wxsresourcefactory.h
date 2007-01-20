#ifndef WXSRESOURCEFACTORY_H
#define WXSRESOURCEFACTORY_H

#include <wx/string.h>
#include <wx/menu.h>

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

        /** \brief Checking if factory can handle given file as external resource */
        static bool CanHandleExternal(const wxString& FileName);

        /** \brief Building external resource object */
        static wxsResource* BuildExternal(const wxString& FileName);

        /** \brief Building wxSmith menu enteries */
        static void BuildSmithMenu(wxMenu* menu);

        /** \brief Processing menu event for creating new resources
         *
         * This function is triggered when there's a chance that "Add ..." menu from
         * wxSmith has been clicked. In that case, wxsResourceFactory will process it
         * and return true. If it's not "Add ..." menu entry, it will return false
         */
        static bool NewResourceMenu(int Id,wxsProject* Project);

    protected:

        /** \brief Ctor */
        wxsResourceFactory();

        /** \brief Dctor */
        virtual ~wxsResourceFactory();

        /** \brief Getting number of resouce types inside this factory */
        virtual int OnGetCount() = 0;

        /** \brief Checking if given resource can be main in application */
        virtual void OnGetInfo(int Number,wxString& Name,wxString& GUI) = 0;

        /** \brief creating resource */
        virtual wxsResource* OnCreate(int Number,wxsProject* Project) = 0;

        /** \brief Checking if factory can handle given file as external resource */
        virtual bool OnCanHandleExternal(const wxString& FileName) { return false; }

        /** \brief Building external resource object */
        virtual wxsResource* OnBuildExternal(const wxString& FileName) { return NULL; }

        /** \brief Function creating new resource object
         *
         * This function is responsible for creating new resource, adding it into project,
         * additionally adding new files to project and initialize resource with proper
         * data. Usually it will require some dialog box before creating resource
         * to get needed params.
         */
        virtual bool OnNewWizard(int Number,wxsProject* Project) = 0;

    private:

        struct ResourceInfo
        {
            wxsResourceFactory* m_Factory;
            int m_Number;
            wxString m_GUI;
            int m_MenuId;
            ResourceInfo(): m_Factory(NULL), m_Number(0), m_MenuId(-1) {}
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
