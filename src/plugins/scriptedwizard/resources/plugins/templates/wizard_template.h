/***************************************************************
 * Name:      [PROJECT_NAME]
 * Purpose:   Code::Blocks plugin
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME]
 * License:   GPL
 **************************************************************/

#ifndef [GUARD_WORD]
#define [GUARD_WORD]

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h> // for "class cbWizardPlugin"

class [PLUGIN_NAME] : public cbWizardPlugin
{
    public:
        /** Constructor. */
        [PLUGIN_NAME]();
        /** Destructor. */
        virtual ~[PLUGIN_NAME]();
[IF HAS_CONFIGURE]
        /** Return the plugin's configuration priority.
          * This is a number (default is 50) that is used to sort plugins
          * in configuration dialogs. Lower numbers mean the plugin's
          * configuration is put higher in the list.
          */
        virtual int GetConfigurationPriority() const { return 50; }

        /** Return the configuration group for this plugin. Default is cgUnknown.
          * Notice that you can logically OR more than one configuration groups,
          * so you could set it, for example, as "cgCompiler | cgContribPlugin".
          */
        virtual int GetConfigurationGroup() const { return cgUnknown; }

        /** Return plugin's configuration panel.
          * @param parent The parent window.
          * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
          */
        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent){ return 0; }

        /** Return plugin's configuration panel for projects.
          * The panel returned from this function will be added in the project's
          * configuration dialog.
          * @param parent The parent window.
          * @param project The project that is being edited.
          * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
          */
        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0; }
[ENDIF HAS_CONFIGURE]
        /** @return the number of template wizards this plugin contains */
        virtual int GetCount() const;

        /** @param index the wizard index.
          * @return the output type of the specified wizard at @c index */
        virtual TemplateOutputType GetOutputType(int index) const;

        /** @param index the wizard index.
          * @return the template's title */
        virtual wxString GetTitle(int index) const;

        /** @param index the wizard index.
          * @return the template's description */
        virtual wxString GetDescription(int index) const;

        /** @param index the wizard index.
          * @return the template's category (GUI, Console, etc; free-form text). Try to adhere to standard category names... */
        virtual wxString GetCategory(int index) const;

        /** @param index the wizard index.
          * @return the template's bitmap */
        virtual const wxBitmap& GetBitmap(int index) const;

        /** @param index the wizard index.
          * @return this wizard's script filename (if this wizard is scripted). */
        virtual wxString GetScriptFilename(int index) const;

        /** When this is called, the wizard must get to work ;).
          * @param index the wizard index.
          * @param createdFilename if provided, on return it should contain the main filename
          *                         this wizard created. If the user created a project, that
          *                         would be the project's filename.
          *                         If the wizard created a build target, that would be an empty string.
          *                         If the wizard created a file, that would be the file's name.
          * @return a pointer to the generated cbProject or ProjectBuildTarget. NULL for everything else (failure too).
          * You should dynamic-cast this to the correct type based on GetOutputType() 's value. */
        virtual CompileTargetBase* Launch(int index, wxString* createdFilename = 0);
    protected:
        /** Any descendent plugin should override this virtual method and
          * perform any necessary initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded and should attach in Code::Blocks. When Code::Blocks
          * starts up, it finds and <em>loads</em> all plugins but <em>does
          * not</em> activate (attaches) them. It then activates all plugins
          * that the user has selected to be activated on start-up.\n
          * This means that a plugin might be loaded but <b>not</b> activated...\n
          * Think of this method as the actual constructor...
          */
        virtual void OnAttach();

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnRelease(bool appShutDown);

        wxBitmap m_Bitmap;
};

#endif // [GUARD_WORD]

