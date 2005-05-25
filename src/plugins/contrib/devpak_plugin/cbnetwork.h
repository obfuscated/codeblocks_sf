#ifndef CBNETWORK_H
#define CBNETWORK_H

#include <wx/event.h>
#include <wx/string.h>
#include <wx/stream.h>
#include <wx/url.h>

typedef const int cbNetEvent;

/**
  * @brief Utility class to retrieve remote files from URLs
  *
  * This class is designed to simplify the retrieval of remote files
  * located, for example, on web/ftp servers.
  *
  * The usage is ridiculously simple:
  * <code>
  * cbNetwork net("http://www.example.com");
  * net.DownloadFile("files/remote_file_name.txt", "local_file.txt");
  * </code>
  */
class cbNetwork : public wxEvtHandler
{
    public:
        /**
          * @brief Structure that holds info on a remote file
          *
          * Used by the GetFileInfo() function.
          */
        struct FileInfo
        {
            wxString contentType; ///< The content type, e.g. "text/plain"
            size_t size; ///< The file's size in bytes
        };

        /** Constructor
          * @param serverUrl The remote server's top-level URL.
          */
        cbNetwork(wxEvtHandler* parent, int id, const wxString& serverUrl = "");
        ~cbNetwork();
        
        /** Set the server to connect to.
          * @param serverUrl The remote server's top-level URL.
          */
        void SetServer(const wxString& serverUrl);
        /** Retrieve file info on a remote file.
          * @param remote The remote file (relative to server's URL passed to the constructor)
          */
        /** Abort an active connection */
        void Abort();
        FileInfo* GetFileInfo(const wxString& remote);
        /** Retrieve the contents of a remote file.
          * @param remote The remote file (relative to server's URL passed to the constructor)
          * @param buffer The string buffer to put the file's contents in
          * @note Use this function only for ASCII files.
          */
        bool ReadFileContents(const wxString& remote, wxString& buffer);
        /** Retrieve a remote file.
          * @param remote The remote file (relative to server's URL passed to the constructor)
          * @param local The local filename to save to
          */
        bool DownloadFile(const wxString& remote, const wxString& local);
        /** Is it connected? */
        bool IsConnected();
    protected:
        void Disconnect();
        bool Connect(const wxString& remote);
        void Notify(cbNetEvent event, const wxString& msg, int integer = 0);
        FileInfo* PrivateGetFileInfo(const wxString& remote); // no Connect() / Disconnect()

        wxEvtHandler* m_pParent;
        int m_ID;
        wxString m_ServerURL;
        wxInputStream* m_pStream;
        wxURL* m_pURL;
        bool m_Abort;
};

#define DEFINE_EVT_CBNET(e) extern cbNetEvent e;
#define DECLARE_EVT_CBNET(e) cbNetEvent e = wxNewId();

DEFINE_EVT_CBNET(cbEVT_CBNET_CONNECT);
DEFINE_EVT_CBNET(cbEVT_CBNET_DISCONNECT);
DEFINE_EVT_CBNET(cbEVT_CBNET_START_DOWNLOAD);
DEFINE_EVT_CBNET(cbEVT_CBNET_PROGRESS);
DEFINE_EVT_CBNET(cbEVT_CBNET_END_DOWNLOAD);
DEFINE_EVT_CBNET(cbEVT_CBNET_ABORTED);

#define EVT_CBNET_CONNECT( id, fn ) \
    DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBNET_CONNECT, id, id, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, (wxObject*) NULL ),
#define EVT_CBNET_DISCONNECT( id, fn ) \
    DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBNET_DISCONNECT, id, id, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, (wxObject*) NULL ),
#define EVT_CBNET_START_DOWNLOAD( id, fn ) \
    DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBNET_START_DOWNLOAD, id, id, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, (wxObject*) NULL ),
#define EVT_CBNET_PROGRESS( id, fn ) \
    DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBNET_PROGRESS, id, id, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, (wxObject*) NULL ),
#define EVT_CBNET_END_DOWNLOAD( id, fn ) \
    DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBNET_END_DOWNLOAD, id, id, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, (wxObject*) NULL ),
#define EVT_CBNET_ABORTED( id, fn ) \
    DECLARE_EVENT_TABLE_ENTRY( cbEVT_CBNET_ABORTED, id, id, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, (wxObject*) NULL ),

#endif // CBNETWORK_H
