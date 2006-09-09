#ifndef WXSGUI_H
#define WXSGUI_H

#include <configurationpanel.h>
#include <tinyxml/tinyxml.h>

class wxsProject;

/** \brief This class is interface for GUI types.
 *
 * Each project has it's main GUI and each resource uses particular GUI.
 * GUI class is responsible for editing base GUI parameters and for
 * creating / setting up main application class. Each project may have
 * only one GUI managed.
 */
class wxsGUI
{
    public:

        /** \brief Ctor
         * \param GUIName name of GUI (f.ex. wxWidgets for wx, MFC, Qt etc)
         * \param Project project class using this GUI
         */
        wxsGUI(const wxString& GUIName,wxsProject* Project);

        /** \brief Dctor */
        virtual ~wxsGUI();

        /** \brief Getting project item */
        inline wxsProject* GetProject() const { return m_Project; }

        /** \brief Getting name of this GUI */
        inline const wxString& GetName() const { return m_Name; }

    protected:

        /** \brief Function called to build configuration panel */
        virtual cbConfigurationPanel* OnBuildConfigurationPanel(wxWindow* Parent) = 0;

        /** \brief Function called when there's need to rebuild application code */
        virtual void OnRebuildApplicationCode() = 0;

        /** \brief Function checking if project's main application code is managed using
         *         this GUI.
         */
        virtual bool OnCheckIfApplicationManaged() = 0;

        /** \brief Function trying to create bindings between this GUI class and
         *         application class.
         *
         * Creating application binding may require scanning of files / selecting
         * application file, creating new files or anything else needed.
         *
         * \return true if added bindings, false otherwise.
         */
        virtual bool OnCreateApplicationBinding() = 0;

        /** \brief Reading configuration from given GUI module */
        virtual void OnReadConfig(TiXmlElement* element) = 0;

        /** \brief Storing configuration of this GUI module */
        virtual void OnWriteConfig(TiXmlElement* element) = 0;

    private:

        const wxString m_Name;          ///< \brief name of this GUI module (f.ex wxWidgets)
        wxsProject* const m_Project;    ///< \brief wxsProject class using this GUI class

        friend class wxsProject;
};

#endif
