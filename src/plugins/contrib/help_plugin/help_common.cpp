#include "help_common.h"
#include <configmanager.h>
#include <wx/intl.h>

using std::make_pair;

int HelpCommon::m_DefaultHelpIndex = -1;

void HelpCommon::LoadHelpFilesVector(HelpCommon::HelpFilesVector &vect)
{
	vect.clear();
  long cookie;
  wxString entry;
  wxConfigBase *conf = ConfigManager::Get();
  wxString oldPath = conf->GetPath();
  conf->SetPath(_("/help_plugin"));
  bool cont = conf->GetFirstEntry(entry, cookie);
  
  while (cont)
  {
    if (entry == _("default"))
    {
      m_DefaultHelpIndex = conf->Read(entry, -1);
    }
    else
    {
      wxString file = conf->Read(entry, wxEmptyString);
      
      if (!file.IsEmpty())
      {
        vect.push_back(make_pair(entry, file));
      }
    }
    
    cont = conf->GetNextEntry(entry, cookie);
  }
  
  conf->SetPath(oldPath);
}

void HelpCommon::SaveHelpFilesVector(HelpCommon::HelpFilesVector &vect)
{
  wxConfigBase *conf = ConfigManager::Get();
  conf->DeleteGroup(_("/help_plugin"));
  wxString oldPath = conf->GetPath();
  conf->SetPath(_("/help_plugin"));
  HelpFilesVector::iterator it;
  
  for (it = vect.begin(); it != vect.end(); ++it)
  {
    wxString file = it->second;
    
    if (!file.IsEmpty())
    {
      conf->Write(it->first, file);
    }
  }
  
  conf->Write(_("default"), m_DefaultHelpIndex);
  conf->SetPath(oldPath);
}
