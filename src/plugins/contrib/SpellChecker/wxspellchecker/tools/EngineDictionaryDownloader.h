#ifndef __ENGINE_DICTIONARY_DOWNLOADER__
#define __ENGINE_DICTIONARY_DOWNLOADER__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


// Abstract base class comparable to wxSpellCheckEngineInterface with derived classes
//  handling the details needed for downloading and installing the dictionaries

class EngineDictionaryDownloader
{
public:  
  
  // Populate DictionaryArray with a list of the available dictionaries
  virtual bool RetrieveDictionaryList(wxArrayString& DictionaryArray) = 0;

  // Download the given dictionary and return the local file name
  virtual wxString DownloadDictionary(wxString& strDictionary) = 0;
  
  // Given the filename (downloaded under the hard drive), install the dictionary
  virtual bool InstallDictionary(wxString& strFileName, bool bDeleteFileAfterInstall = false) = 0;
  
  // Return the FTP server to connect to for the dictionaries
  virtual wxString GetServer() = 0;
  
  // Return the FTP server path were to find the dictionaries
  virtual wxString GetServerDirectory() = 0;
};

#endif  // __ENGINE_DICTIONARY_DOWNLOADER__
