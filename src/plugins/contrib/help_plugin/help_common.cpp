#include "help_common.h"
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/dynarray.h>

using std::make_pair;

int HelpCommon::m_DefaultHelpIndex = -1;

void HelpCommon::LoadHelpFilesVector(HelpCommon::HelpFilesVector &vect)
{
  vect.clear();
  ConfigManager* conf = Manager::Get()->GetConfigManager(_T("help_plugin"));
  m_DefaultHelpIndex = conf->ReadInt(_T("/default"), -1);
  wxArrayString list = conf->EnumerateSubPaths(_T("/"));
  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
      wxString name = conf->Read(list[i] + _T("/name"), wxEmptyString);
      wxString file = conf->Read(list[i] + _T("/file"), wxEmptyString);

      if (!name.IsEmpty() && !file.IsEmpty())
      {
        vect.push_back(make_pair(name, file));
      }
  }
}

void HelpCommon::SaveHelpFilesVector(HelpCommon::HelpFilesVector &vect)
{
  ConfigManager* conf = Manager::Get()->GetConfigManager(_T("help_plugin"));
  wxArrayString list = conf->EnumerateSubPaths(_T("/"));

  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
    conf->DeleteSubPath(list[i]);
  }

  HelpFilesVector::iterator it;

  int count = 0;

  for (it = vect.begin(); it != vect.end(); ++it)
  {
    wxString name = it->first;
    wxString file = it->second;

    if (!name.IsEmpty() && !file.IsEmpty())
    {
      wxString key = wxString::Format(_T("/help%d/"), count++);
      conf->Write(key + _T("name"), name);
      conf->Write(key + _T("file"), file);
    }
  }

  conf->Write(_T("/default"), m_DefaultHelpIndex);
}
