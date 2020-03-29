#include "AspellDictionaryDownloader.h"

#include "bzipstream/bzipstream.h"

#include <wx/protocol/ftp.h>
#include <wx/filename.h>
#include <wx/wfstream.h>

#include "wx/fs_zip.h"
#include "wx/zipstrm.h"

#include "wx/regex.h"

#include "AspellInterface.h"


bool AspellDictionaryDownloader::RetrieveDictionaryList(wxArrayString& DictionaryArray)
{
  if (UseWin32Dictionaries())
    return RetrieveDictionaryListFromWin32Aspell(DictionaryArray);
  else
    return RetrieveDictionaryListFromStandardAspell(DictionaryArray);
}

wxString AspellDictionaryDownloader::DownloadDictionary(wxString& strDictionary)
{
  if (UseWin32Dictionaries())
    return DownloadDictionaryFromWin32Aspell(strDictionary);
  else
    return DownloadDictionaryFromStandardAspell(strDictionary);
}

bool AspellDictionaryDownloader::InstallDictionary(wxString& strFileName, bool bDeleteFileAfterInstall /*= false*/)
{
  bool bReturn = false;
  if (UseWin32Dictionaries())
    bReturn = InstallDictionaryFromWin32Aspell(strFileName);
  else
    bReturn = InstallDictionaryFromStandardAspell(strFileName);

  if (bReturn)
  {
    if (bDeleteFileAfterInstall)
    {
      ::wxRemoveFile(strFileName);
    }
  }
  
  return true;
}

wxString AspellDictionaryDownloader::SelectDictionaryToDownload(wxArrayString& FileArray)
{
  // Loop through the array and return the most recent file
  // Eventually, we might want to check the Aspell version and make sure
  //  that we're getting a compatible version
  if (FileArray.GetCount() > 0)
  {
    return FileArray[FileArray.GetCount()-1]; // Just return the last file name in the array
  }
  else
  {
    return wxEmptyString;
  }
}

bool AspellDictionaryDownloader::RetrieveDictionaryListFromStandardAspell(wxArrayString& DictionaryArray)
{
  return RetrieveDictionaryList(_("/gnu/aspell/dict/"), _("*"), DictionaryArray);
}

wxString AspellDictionaryDownloader::DownloadDictionaryFromStandardAspell(wxString& strDictionary)
{
  return DownloadDictionary(_("/gnu/aspell/dict/") + strDictionary, _("*.bz2"));
}

bool AspellDictionaryDownloader::RetrieveDictionaryListFromWin32Aspell(wxArrayString& DictionaryArray)
{
  return RetrieveDictionaryList(_("/gnu/aspell/w32/"), _("Aspell-*-*.exe"), DictionaryArray);
}

wxString AspellDictionaryDownloader::DownloadDictionaryFromWin32Aspell(wxString& strDictionary)
{
  return DownloadDictionary(_("/gnu/aspell/w32/"), _("Aspell-") + strDictionary + _("-*.exe"));
}

bool AspellDictionaryDownloader::InstallDictionaryFromStandardAspell(wxString& strFileName)
{
  // The API to wxTarInputStream will hopefully be like
  //  wxZipInputStream so coding to the zip API will be sufficient
  wxFileInputStream FileIn(strFileName);
  wxBZipInputStream BZipIn(FileIn);

  wxString strTempFile = wxFileName::CreateTempFileName(_("tar"));
  wxFileOutputStream TempFileStream(strTempFile);
  wxBufferedOutputStream TempBufferedStream(TempFileStream);
  // It might be better to switch to reading into a buffer so that we can provide
  //  progress indicator feedback.  For now, just one time it though.
  BZipIn.Read(TempBufferedStream);
  TempBufferedStream.Sync();
  
  return true;
}

bool AspellDictionaryDownloader::InstallDictionaryFromWin32Aspell(wxString& strFileName)
{
  wxFileSystem::AddHandler(new wxZipFSHandler);
  wxFileSystem fs;

  AspellInterface* pAspellInterface = new AspellInterface();
  
  if (pAspellInterface)
  {
    wxString strDataDirectory = _T("");
    OptionsMap* pOptions = pAspellInterface->GetOptions();
    OptionsMap::iterator it = pOptions->find("data-dir");
    if (it != pOptions->end())
    {
      strDataDirectory = it->second.GetValueAsString();
    }
    wxString strDictDirectory = _T("");
    pOptions = pAspellInterface->GetOptions();
    it = pOptions->find("dict-dir");
    if (it != pOptions->end())
    {
      strDictDirectory = it->second.GetValueAsString();
    }
    
    wxPrintf(_T("Data directory = %s\n"), strDataDirectory.c_str());
    wxPrintf(_T("Dict directory = %s\n"), strDictDirectory.c_str());
    
    if ((strDataDirectory.IsEmpty() == false) && (strDictDirectory.IsEmpty() == false))
    {
      // Stuff for the data directory
      wxPrintf(_T("Stuff for the data directory\n"));
      wxString wildcard = wxString::Format(_("%s#zip:TmpInstall/data/*"), strFileName.c_str());
      wxPrintf(_T("%s\n"), wildcard.c_str());
      wxString filename = fs.FindFirst(wildcard, wxFILE);
      // Loop through the files in the directory
      while ( !filename.empty() )
      {
        wxPrintf(_T("%s\n"), filename.c_str());
        // Copy the file
        wxString strDestDir = strDataDirectory + wxFileName::GetPathSeparator();
        CopyFromZipFile(fs, filename, strDestDir);
        filename = fs.FindNext();
      }
    
      // Stuff for the dict directory
      wxPrintf(_T("Stuff for the dict directory\n"));
      wildcard = wxString::Format(_("%s#zip:TmpInstall/dict/*"), strFileName.c_str());
      wxPrintf(_T("%s\n"), wildcard.c_str());
      filename = fs.FindFirst(wildcard, wxFILE);
      // Loop through the files in the directory
      while ( !filename.empty() )
      {
        wxPrintf(_T("%s\n"), filename.c_str());
        wxString strDestDir = strDictDirectory + wxFileName::GetPathSeparator();
        CopyFromZipFile(fs, filename, strDestDir);
        filename = fs.FindNext();
      }
    }
  }
  
  return true;
}

bool AspellDictionaryDownloader::CopyFromZipFile(wxFileSystem& fs, wxString& strFileInZip, wxString& strDestDir)
{
  // Create the output file
  wxString strOutputFile = strDestDir + strFileInZip.AfterLast('/');
  wxFileOutputStream OutputFileStream(strOutputFile);
  wxBufferedOutputStream OutputBufferedStream(OutputFileStream);
  wxFSFile* pFile = fs.OpenFile(strFileInZip);
  if (pFile)
  {
    wxInputStream* pInputStream = pFile->GetStream();
    if (pInputStream)
    {
      // It might be better to switch to reading into a buffer so that we can provide
      //  progress indicator feedback.  For now, just one time it though.
      pInputStream->Read(OutputBufferedStream);
      OutputBufferedStream.Sync();
      int nServerSideSize = pInputStream->GetSize();
      int nDownloadedFileSize = wxFile(strOutputFile).Length();
      return (nServerSideSize == nDownloadedFileSize);
    }
  }
  return false;
}

bool AspellDictionaryDownloader::RetrieveDictionaryList(wxString strServerPath, wxString strFileMask, wxArrayString& DictionaryArray)
{
  wxFTP ftp;
  if ( !ftp.Connect(GetServer()) )
  {
      wxLogError("Couldn't connect");
      return false;
  }

  ftp.ChDir(strServerPath);
  wxArrayString FtpListing;
  if (ftp.GetFilesList(FtpListing, strFileMask))
  {
    int FileCount = FtpListing.Count();
    for (int i=0; i<FileCount; i++)
    {
      // Here we let the dictionary downloader interpret the directory name
      //  and (if desireable) give back more descriptive text.  Also, the
      //  downloader can return an empty string if we don't want this
      //  dictionary in the list.
      wxString DictionaryListEntry = GetDictionaryNameFromFileName(FtpListing[i]);
      if (DictionaryListEntry != wxEmptyString)
        DictionaryArray.Add((DictionaryListEntry));
    }
  }
  else
  {
    ::wxMessageBox("Unable to retrieve listing of available dictionaries");
    return false;
  }
  return true;
}

wxString AspellDictionaryDownloader::DownloadDictionary(wxString strServerPath, wxString strFileMask)
{
  wxFTP ftp;
  if ( !ftp.Connect(GetServer()) )
  {
      wxLogError("Couldn't connect");
      return wxEmptyString;
  }
  ftp.ChDir(strServerPath);
  wxArrayString DictionaryFileList;
  if (ftp.GetFilesList(DictionaryFileList, strFileMask))
  {
    wxString strDictionaryToDownload = SelectDictionaryToDownload(DictionaryFileList);
    wxInputStream *in = ftp.GetInputStream(strDictionaryToDownload);

    wxString strTempFile = wxFileName::CreateTempFileName(_("dict"));
    wxFileOutputStream TempFileStream(strTempFile);
    wxBufferedOutputStream TempBufferedStream(TempFileStream);
    // It might be better to switch to reading into a buffer so that we can provide
    //  progress indicator feedback.  For now, just one time it though.
    in->Read(TempBufferedStream);
    TempBufferedStream.Sync();
    int nServerSideSize = in->GetSize();
    int nDownloadedFileSize = wxFile(strTempFile).Length();
    if (nServerSideSize != nDownloadedFileSize)
    {
      ::wxMessageBox("Error downloading dictionary ");// + strDictionary);
      return wxEmptyString;
    }
    else
      return strTempFile;
  }
  else
  {
    ::wxMessageBox("Unable to retrieve listing of available dictionary files for ");// + strDictionary);
    return wxEmptyString;
  }
  return wxEmptyString;
}

wxString AspellDictionaryDownloader::GetDictionaryNameFromFileName(wxString& strFileName)
{
  if (UseWin32Dictionaries())
  {
    wxRegEx RegExpDict("Aspell-(..)-.*exe");
    if  (RegExpDict.Matches(strFileName))
    {
      wxString strMatch = RegExpDict.GetMatch(strFileName, 1);
      return strMatch;
    }
    else
    {
      return wxEmptyString;
    }
  }
  else
  {
    return strFileName;
  }
}

