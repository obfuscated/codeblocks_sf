#include "cbnetwork.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/app.h> // wxPostEvent
#include <wx/filename.h>

DECLARE_EVT_CBNET(cbEVT_CBNET_CONNECT);
DECLARE_EVT_CBNET(cbEVT_CBNET_DISCONNECT);
DECLARE_EVT_CBNET(cbEVT_CBNET_START_DOWNLOAD);
DECLARE_EVT_CBNET(cbEVT_CBNET_PROGRESS);
DECLARE_EVT_CBNET(cbEVT_CBNET_END_DOWNLOAD);
DECLARE_EVT_CBNET(cbEVT_CBNET_ABORTED);

cbNetwork::cbNetwork(wxEvtHandler* parent, int id, const wxString& serverUrl)
    : m_pParent(parent),
    m_ID(id),
    m_ServerURL(serverUrl),
    m_pStream(0),
    m_pURL(0),
    m_Abort(false)
{
}

cbNetwork::~cbNetwork()
{
    Disconnect();
}

void cbNetwork::Notify(cbNetEvent event, const wxString& msg, int integer)
{
    if (m_pParent)
    {
        wxCommandEvent evt(event, m_ID);
        evt.SetString(msg);
        evt.SetInt(integer);
        wxPostEvent(m_pParent, evt);
        wxYield();
    }
}

bool cbNetwork::IsConnected()
{
    return m_pStream != 0;
}

void cbNetwork::Abort()
{
    m_Abort = true;
}

void cbNetwork::SetServer(const wxString& serverUrl)
{
    m_ServerURL = serverUrl;
}

void cbNetwork::Disconnect()
{
    Notify(cbEVT_CBNET_DISCONNECT, "Disconnected");
    m_Abort = false;
    if (m_pStream)
        delete m_pStream;
    m_pStream = 0;
    
    if (m_pURL)
        delete m_pURL;
    m_pURL = 0;
}

bool cbNetwork::Connect(const wxString& remote)
{
    Disconnect();

    m_pURL = new wxURL(m_ServerURL + "/" + remote);
    if (m_pURL->GetError() != wxURL_NOERR)
        return false;
    
    m_pStream = m_pURL->GetInputStream();
    if (m_pStream && m_pStream->IsOk())
    {
        Notify(cbEVT_CBNET_CONNECT, "Connected");
        return true;
    }
    return false;
}

cbNetwork::FileInfo* cbNetwork::PrivateGetFileInfo(const wxString& remote) // no Connect() / Disconnect()
{
    static cbNetwork::FileInfo info;
    info.contentType = m_pURL->GetProtocol().GetContentType();
    info.size = m_pStream->GetSize();
    return &info;
}

cbNetwork::FileInfo* cbNetwork::GetFileInfo(const wxString& remote)
{
    if (!Connect(remote))
        return 0;
    FileInfo* info = PrivateGetFileInfo(remote);
    Disconnect();
    return info;
}

bool cbNetwork::ReadFileContents(const wxString& remote, wxString& buffer)
{
    if (!Connect(remote))
        return false;
    wxString name = wxFileName(remote).GetFullName();
    FileInfo* info = PrivateGetFileInfo(remote);
    Notify(cbEVT_CBNET_START_DOWNLOAD, name, info ? info->size : 0);

    buffer.Clear();
    wxTextInputStream tis(*m_pStream);
    while (!m_Abort && !m_pStream->Eof())
    {
        buffer += tis.ReadLine() + '\n';
        Notify(cbEVT_CBNET_PROGRESS, name, buffer.Length());
    }

    if (m_Abort)
    {
        Notify(cbEVT_CBNET_ABORTED, "Aborted");
        buffer.Clear();
    }
    Notify(cbEVT_CBNET_END_DOWNLOAD, name, m_Abort ? -1 : 0);

    Disconnect();
    return true;
}

bool cbNetwork::DownloadFile(const wxString& remote, const wxString& local)
{
    if (!Connect(remote))
        return false;
    wxString name = wxFileName(remote).GetFullName();
    
    // block to limit scope of "fos"
    {
        wxFileOutputStream fos(local);
        if (!fos.Ok())
        {
            Disconnect();
            return false;
        }
    
        FileInfo* info = PrivateGetFileInfo(remote);
        Notify(cbEVT_CBNET_START_DOWNLOAD, name, info ? info->size : 0);
    
        static char buffer[4096];
        memset(buffer, 0, 4096);
        int counter = 0;
        while (!m_Abort && !m_pStream->Eof() && m_pStream->CanRead())
        {
            m_pStream->Read(buffer, 4096);
            size_t size = m_pStream->LastRead();
            if (size == 0)
                break;
            fos.Write(buffer, size);
            counter += size;
            Notify(cbEVT_CBNET_PROGRESS, name, counter);
        }
    }

    if (m_Abort)
    {
        Notify(cbEVT_CBNET_ABORTED, name);
        wxRemoveFile(local);
    }
    Notify(cbEVT_CBNET_END_DOWNLOAD, name, m_Abort ? -1 : 0);

    Disconnect();
    return true;
}
