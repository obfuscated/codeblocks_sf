#include <wx/utils.h> // wxLaunchDefaultBrowser

#include <sdk.h> // Code::Blocks SDK
#include "cb_koders.h"

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(CB_Koders, "Code::Blocks Koders connectivity");

// constructor
CB_Koders::CB_Koders() :
  TheDialog(0)
{
	m_PluginInfo.name = _T("CB_Koders");
	m_PluginInfo.title = _("Koders query");
	m_PluginInfo.version = _T("0.1");
	m_PluginInfo.description = _("This plugin queries the Koders webpage for keywords");
	m_PluginInfo.author = _T("MortenMacFly");
	m_PluginInfo.authorEmail = _T("mac-fly@gmx.net");
	m_PluginInfo.authorWebsite = _T("http://www.codeblocks.org");
	m_PluginInfo.thanksTo = _("Yiannis Mandravellos, Thomas Denk and all the other C::B developers and contributers for a great piece of software.");
	m_PluginInfo.license = LICENSE_GPL;
}

// destructor
CB_Koders::~CB_Koders()
{
	if (TheDialog)
    TheDialog->Destroy();
}

void CB_Koders::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void CB_Koders::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int CB_Koders::Execute()
{
  if (!IsAttached())
    return -1;

  if (!TheDialog)
    TheDialog = new KodersDialog(Manager::Get()->GetAppWindow());

  if(TheDialog && TheDialog->ShowModal()==wxID_OK)
  {
    wxString search = TheDialog->GetSearch();
    if (search.IsEmpty())
    {
      cbMessageBox(_("Cannot search for an empty expession."), _("Error"), wxICON_ERROR);
    }
    else
    {
      wxString language = TheDialog->GetLanguage();
      wxString license  = TheDialog->GetLicense();

      wxString query;
      query.Printf(_("http://www.koders.com/?S=%s&btnSearch=Search&la=%s&li=%s"),
                   search.c_str(), language.c_str(), license.c_str());
      if (!wxLaunchDefaultBrowser(query))
        cbMessageBox(_("Could not launch the default browser of your system."), _("Error"), wxICON_ERROR);
    }
  }

	return 0;
}
