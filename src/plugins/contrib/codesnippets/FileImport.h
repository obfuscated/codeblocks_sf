#ifndef FILEIMPORT_H_INCLUDED
#define FILEIMPORT_H_INCLUDED

#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/dir.h"

// ----------------------------------------------------------------------------
class FileImportTraverser : public wxDirTraverser
// ----------------------------------------------------------------------------
{
public:
    FileImportTraverser (const wxString& rootSrcDir,
                         const wxString& destDir)
    {
        m_rootSrcDir = rootSrcDir;
        m_destDir = destDir;
        wxFileName destdirs(destDir);
        wxString volume = destdirs.GetVolume();
        if (not volume.IsEmpty())
            volume += wxFileName::GetVolumeSeparator()+wxFileName::GetPathSeparator();
        wxArrayString arydirs =  destdirs.GetDirs();
        wxString newDirectory(volume);
        for (size_t i=0; i<arydirs.Count(); ++i)
        {
            if ( i>0 )
                newDirectory += wxFileName::GetPathSeparator();
            newDirectory += arydirs[i];
            if (::wxDirExists(newDirectory) == false)
            {
                //wxPrintf(_T("ctor Dir create[%s]\n"), newDirectory.c_str());
                int rc = ::wxMkdir (newDirectory);
                if (not rc)
                {   //wxPrintf(_T("Mkdir failed for[%s]"), newDirectory.c_str());
                    break;
                }
            }
        }//for
    }//ctor

    virtual wxDirTraverseResult OnFile (const wxString& filename)
    {
        // ::wxCopyFile (filename, (destDir + dirname - rootSrcDir))
        //wxPrintf(_T("OnFile[%s] to [%s]\n"), filename.c_str(), ConvertToDestinationPath(filename).c_str());
        int rc = ::wxCopyFile (filename, ConvertToDestinationPath (filename));
        if (not rc) ; //wxPrintf(_T("Copy failed for[%s]"), filename.c_str());
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir (const wxString& dirname)
    {
        // ::wxMkDir (destDir + (dirname - rootSrcDir))
        wxString newDirectory = ConvertToDestinationPath (dirname);
        if (::wxDirExists(newDirectory) == false)
        {
            //wxPrintf(_T("OnDir create[%s]\n"), newDirectory.c_str());
            int rc = ::wxMkdir (newDirectory);
            if (not rc)
            {   //wxPrintf(_T("Mkdir failed for[%s]"), newDirectory.c_str());
                return wxDIR_STOP;
            }
        }
        return wxDIR_CONTINUE;
    }

private:

    wxString ConvertToDestinationPath (const wxString& fullPath)
    {
        wxFileName filename(fullPath);
        wxString destName = wxFileName(m_destDir + wxFileName::GetPathSeparator() +
                                       fullPath.Mid (m_rootSrcDir.Len())).GetFullPath ();
        return destName;
    }

    wxString m_rootSrcDir;
    wxString m_destDir;
};


#endif // FILEIMPORT_H_INCLUDED
