#include <memory>
#include "webresourcesmanager.h"

#include <globals.h>
#include <configmanager.h>
#include <wx/url.h>

WebResourcesManager::WebResourcesManager()
{
}

WebResourcesManager::~WebResourcesManager()
{
    ClearEntries();
}

bool WebResourcesManager::LoadDetectionConfigurations(const wxArrayString& baseUrls, ProgressHandler* handler )
{
    ClearEntries();

    bool AnyValid = false;

    for ( size_t i=0; i<baseUrls.Count(); i++ )
    {

        wxString listUrl = baseUrls[i];
        wxString baseUrl;
        if ( listUrl.Last() == _T('/') )
        {
            baseUrl = listUrl;
        }
        else
        {
            baseUrl = listUrl.BeforeLast(_T('/'));
            if ( baseUrl.IsEmpty() )
            {
                baseUrl = listUrl;
            }
            baseUrl += _T('/');
        }

        std::vector< char > arr;
        if ( !DoDownload( listUrl, handler, arr ) ) continue;

        TiXmlDocument doc;
        if ( doc.Parse( &arr[0], 0, TIXML_DEFAULT_ENCODING ) )
        {
            if ( doc.RootElement() )
            {
                if ( !strcmp(doc.RootElement()->Value(),"libfinderlist") )
                {
                    TiXmlElement* root = doc.RootElement();
                    for ( TiXmlElement* lib = root->FirstChildElement("library");
                          lib;
                          lib = lib->NextSiblingElement("library") )
                    {
                        wxString ShortCode = wxString( lib->Attribute("short_code"), wxConvUTF8 );
                        wxString FileName  = wxString( lib->Attribute("file_name"),  wxConvUTF8 );
                        wxString DigiSign  = wxString( lib->Attribute("sign"),       wxConvUTF8 );
                        if ( !ShortCode.IsEmpty() && !FileName.IsEmpty() )
                        {
                            DetectConfigurationEntry* entry = new DetectConfigurationEntry;
                            entry->m_Url = baseUrl + FileName;
                            entry->m_Sign = DigiSign;
                            entry->m_Next = m_Entries[ ShortCode ];
                            m_Entries[ ShortCode ] = entry;
                            AnyValid = true;
                            continue;
                        }
                    }

                    continue;
                }
            }
        }
        if ( handler ) handler->Error( _("Invalid data in libraries list in: ") + listUrl, handler->idDownloadList );
    }

    if ( handler ) handler->JobFinished( handler->idDownloadList );
    return AnyValid;
}

bool WebResourcesManager::DoDownload( const wxString& urlName, ProgressHandler* handler, std::vector< char >& arr )
{
    int id = handler ? handler->StartDownloading( urlName ) : 0;

    if ( handler ) handler->SetProgress( 0.f, id );

    std::unique_ptr< wxURL > url ( new wxURL( urlName ) );
    url->SetProxy( ConfigManager::GetProxy() );

    if ( url->GetError() != wxURL_NOERR )
    {
        if ( handler ) handler->Error( _("Couldn't open url: ") + urlName, id );
        return false;
    }

    std::unique_ptr< wxInputStream > stream ( url->GetInputStream() );
    if ( !stream.get() || !stream->IsOk() )
    {
        if ( handler ) handler->Error( _("Couldn't open url: ") + urlName, id );
        return false;
    }

    wxFileOffset length = stream->GetLength();
    if ( !length )
    {
        return true;
    }

    static const int BlockSize = 0x1000;

    if ( length != wxInvalidOffset )
    {
        arr.resize( length+1 );
        arr[length] = 0;

        wxFileOffset left = length;
        wxFileOffset read = 0;
        if ( handler ) handler->SetProgress( 0.f, id  );
        while ( left )
        {
            size_t nowRead = stream->Read( &arr[read], ( left > BlockSize ) ? BlockSize : left ).LastRead();
            if ( !nowRead )
            {
                if ( handler ) handler->Error( _("Read error from url: ") + urlName, id );
                return false;
            }

            left -= nowRead;
            read += nowRead;

            if ( handler ) handler->SetProgress( 100.f * read / length, id );
        }
    }
    else
    {
        if ( handler ) handler->SetProgress( -1.f, id );
        size_t read = 0;
        do
        {
            arr.resize( read + BlockSize + 1 );
            size_t nowRead = stream->Read( &arr[read], BlockSize ).LastRead();
            if ( !nowRead ) break;
            read += nowRead;
            if ( handler ) handler->SetProgress( -1.f, id );
        }
        while ( !stream->Eof() );

        arr.resize( read+1 );
        arr[read] = 0;
    }
    if ( handler ) handler->JobFinished( id );
    return true;
}

void WebResourcesManager::ClearEntries()
{
    for ( EntriesT::iterator i = m_Entries.begin(); i!=m_Entries.end(); ++i )
    {
        for ( DetectConfigurationEntry * entry = i->second, *next; entry; entry = next )
        {
            next = entry->m_Next;
            delete entry;
        }
    }
    m_Entries.clear();
}

bool WebResourcesManager::LoadDetectionConfig( const wxString& shortcut, std::vector< char >& content, ProgressHandler* handler )
{
    DetectConfigurationEntry* entry = m_Entries[ shortcut ];
    for ( ; entry ; entry = entry->m_Next )
    {
        if ( DoDownload( entry->m_Url, handler, content ) )
        {
            if ( handler ) handler->JobFinished( handler->idDownloadConfig );
            return true;
        }
    }
    if ( handler ) handler->Error( _("Couldn't download any configuration"), handler->idDownloadConfig );
    return false;
}
