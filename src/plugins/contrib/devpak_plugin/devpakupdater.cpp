/***************************************************************
 * Name:      devpakupdater.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos<mandrav@codeblocks.org>
 * Created:   05/12/05 15:44:32
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "devpakupdater.h"
#endif

#include "devpakupdater.h"
#include "updatedlg.h"
#include "conf.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <manager.h>
#include <configmanager.h>
#include <compilerfactory.h>
#include <compiler.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>

cbPlugin* GetPlugin()
{
	return new DevPakUpdater;
}

DevPakUpdater::DevPakUpdater()
{
	//ctor
	m_PluginInfo.name = "DevPakUpdater";
	m_PluginInfo.title = "Dev-C++ DevPak updater/installer";
	m_PluginInfo.version = "0.1";
	m_PluginInfo.description = "Installs selected DevPaks from the Internet";
	m_PluginInfo.author = "Yiannis Mandravellos";
	m_PluginInfo.authorEmail = "mandrav@codeblocks.org";
	m_PluginInfo.authorWebsite = "http://www.codeblocks.org";
	m_PluginInfo.thanksTo = "Dev-C++ community.\n"
                            "Julian R Seward for libbzip2.\n"
                            "\tlibbzip2 copyright notice:\n"
                            "\t\"bzip2\" and associated library \"libbzip2\", are\n"
                            "\tcopyright (C) 1996-2000 Julian R Seward.\n"
                            "\tAll rights reserved.";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;

    Manager::Get()->Loadxrc("/devpakupdater.zip#zip:*.xrc");
    ConfigManager::AddConfiguration(m_PluginInfo.title, "/devpak_plugin"); 
    g_MasterPath = ConfigManager::Get()->Read("/devpak_plugin/master_path");
}

DevPakUpdater::~DevPakUpdater()
{
	//dtor
}

void DevPakUpdater::OnAttach()
{
}

void DevPakUpdater::OnRelease(bool appShutDown)
{
}

bool DevPakUpdater::ConfigurationValid()
{
    // let's make sure we have a valid configuration
    if (g_MasterPath.IsEmpty() || !wxDirExists(g_MasterPath))
    {
        if (wxMessageBox("The Dev-C++ DevPak Plugin is not configured yet.\nDo you want to configure it now?", "Question", wxICON_QUESTION | wxYES_NO) == wxNO)
            return false;
        if (Configure() != 0)
            return false;
        // ask to add in compiler paths
        if (wxMessageBox("Do you want to add this path to the compiler's search dirs?\n"
                        "(needed to be able to actually compile anything)",
                        "Question", wxICON_QUESTION | wxYES_NO) == wxYES)
        {
            Compiler* compiler = CompilerFactory::Compilers[0]; // GCC is always first compiler
            if (!compiler)
            {
                wxMessageBox("Invalid compiler!?!", "Error", wxICON_ERROR);
                return true;
            }
            compiler->AddIncludeDir(g_MasterPath + wxFILE_SEP_PATH + "include");
            compiler->AddLibDir(g_MasterPath + wxFILE_SEP_PATH + "lib");
        }
    }
    return true;
}

int DevPakUpdater::Configure()
{
    if (g_MasterPath.IsEmpty())
        g_MasterPath = ConfigManager::Get()->Read("/app_path") + wxFILE_SEP_PATH + "DevPaks";
	wxString dir = wxDirSelector("Please select the path where DevPaks will be downloaded and installed:",
                                g_MasterPath);
    if (!dir.IsEmpty())
    {
        g_MasterPath = dir;
        if (!ConfigurationValid())
        {
            g_MasterPath.Clear();
            return -1;
        }
        ConfigManager::Get()->Write("/devpak_plugin/master_path", g_MasterPath);
        return 0;
    }
    return -1;
}

int DevPakUpdater::Execute()
{
    if (!ConfigurationValid())
        return -1;
    UpdateDlg dlg(Manager::Get()->GetAppWindow());
    dlg.ShowModal();
	return 0;
}
