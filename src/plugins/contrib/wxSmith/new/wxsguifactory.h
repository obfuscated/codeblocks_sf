#ifndef WXSGUIFACTORY_H
#define WXSGUIFACTORY_H

class wxsGUI;
class wxsProject;

/** \brief Class representing GUI factory
 *
 * This class can be used to enumerate and produce any GUI item
 * managed by wxSmith. This is done using static members of this class.
 * To add more GUI, only instance of class derived from wxsGUIFactory need
 * to be created and it will automatically register new GUI modules.
 */
class wxsGUIFactory
{
    public:

        /** \brief Building GUI item from given name */
        static wxsGUI* Build(const wxString& Name,wxsProject* Project);

        /** \brief Creating GUI after selecting it from list */
        static wxsGUI* SelectNew(const wxString& Message,wxsProject* Project);

    protected:

        /** \brief Ctor */
        wxsGUIFactory();

        /** \brief Dctor */
        virtual ~wxsGUIFactory();


        /** \brief Getting number of managed GUI modules */
        virtual int OnGetCount() = 0;

        /** \brief Getting name of GUI module */
        virtual wxString OnGetName(int Number) = 0;

        /** \brief Creating GUI module */
        virtual wxsGUI* OnCreate(int Number,wxsProject* Project) = 0;

    private:

        struct GUIItem
        {
            wxsGUIFactory* m_Factory;
            int m_Number;
            GUIItem(): m_Factory(0), m_Number(0) {}
        };
        WX_DECLARE_STRING_HASH_MAP(GUIItem,GUIItemHashT);

        wxsGUIFactory* m_Next;                  ///< \brief Pointer to next factory in newly added queue
        static wxsGUIFactory* m_UpdateQueue;    ///< \brief Queue of QUI factories newly added but not yet initiated, it's required since can not call pure virtual functions from constructor
        static GUIItemHashT m_Hash;             ///< \brief Hash table for GUI objects

        /** \brief Initializing all factories waiting on queue */
        static inline void InitializeFromQueue();

        /** \brief Initializing instance of GUI factory */
        inline void Initialize();
};

#endif
