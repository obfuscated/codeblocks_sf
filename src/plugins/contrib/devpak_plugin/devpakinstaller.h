#ifndef DEVPAKINSTALLER_H
#define DEVPAKINSTALLER_H

#include <wx/string.h>
#include <wx/progdlg.h>
#include <wx/dynarray.h>

class DevPakInstaller
{
	public:
		DevPakInstaller();
		~DevPakInstaller();
		
		bool Install(const wxString& name, const wxString& filename, const wxString& dir, wxArrayString* files = 0);
		bool Uninstall(const wxString& entry);
		const wxString& GetStatus() const { return m_Status; }
	protected:
	private:
        bool Decompress(const wxString& filename, const wxString& tmpfile);
        bool Untar(const wxString& controlFile, const wxString& filename, const wxString& dirname, wxArrayString* files =  0);
        void CreateProgressDialog(int max = 100);
        void EndProgressDialog();
        void UpdateProgress(int val, const wxString& newtext = wxEmptyString);
        void RemoveControlFile(const wxString& filename); // removes dir also, if empty

        wxString m_Status;
        wxProgressDialog* m_pDlg;
};

#endif // DEVPAKINSTALLER_H
