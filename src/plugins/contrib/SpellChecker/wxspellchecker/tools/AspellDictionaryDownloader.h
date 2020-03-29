#ifndef __ASPELL_DICTIONARY_DOWNLOADER__
#define __ASPELL_DICTIONARY_DOWNLOADER__

#include "EngineDictionaryDownloader.h"

#include "wx/filesys.h"

class AspellDictionaryDownloader : public EngineDictionaryDownloader
{
public:
  
  // Populate DictionaryArray with a list of the available dictionaries
  virtual bool RetrieveDictionaryList(wxArrayString& DictionaryArray);

  // Download the given dictionary and return the local file name
  virtual wxString DownloadDictionary(wxString& strDictionary);
  
  // Given the filename (downloaded under the hard drive), install the dictionary
  virtual bool InstallDictionary(wxString& strFileName, bool bDeleteFileAfterInstall = false);

  // Return the FTP server to connect to for the dictionaries
  virtual wxString GetServer() { return _("ftp.gnu.org"); }
  
  // Return the FTP server path were to find the dictionaries
  virtual wxString GetServerDirectory() { return _("/gnu/aspell/dict"); }

  // Determine which file to download from an array of filenames
  wxString SelectDictionaryToDownload(wxArrayString& FileArray);
  
  // The Win32 dictionaries should work on any x86 byte order system
  inline bool UseWin32Dictionaries() { return (wxINT32_SWAP_ALWAYS(128) == wxINT32_SWAP_ON_LE(128)); }
  
  wxString GetDictionaryNameFromFileName(wxString& strFileName);
  
  // I'm not sure which dictionaries to download (Standard or Win32 Aspell).  The differences are as follows:
  // Standard Aspell:
  //    Packaged in tar.bz2 packages
  //    May need to be "compiled" for their platform
  //    More dictionaries available in this format than Win32
  // Win32 Aspell:
  //    Packaged in zip-compatible executables
  //    Pre-compiled for the "x86 byte order"
  // For now, the following functions will provide the ability to download from both locations
  bool RetrieveDictionaryListFromStandardAspell(wxArrayString& DictionaryArray);
  wxString DownloadDictionaryFromStandardAspell(wxString& strDictionary);
  bool RetrieveDictionaryListFromWin32Aspell(wxArrayString& DictionaryArray);
  wxString DownloadDictionaryFromWin32Aspell(wxString& strDictionary);
  bool RetrieveDictionaryList(wxString strServerPath, wxString strFileMask, wxArrayString& DictionaryArray);
  wxString DownloadDictionary(wxString strServerPath, wxString strFileMask);

  bool InstallDictionaryFromStandardAspell(wxString& strFileName);
  bool InstallDictionaryFromWin32Aspell(wxString& strFileName);
  bool CopyFromZipFile(wxFileSystem& fs, wxString& strFileInZip, wxString& strDestDir);
};

#endif  // __ASPELL_DICTIONARY_DOWNLOADER__
