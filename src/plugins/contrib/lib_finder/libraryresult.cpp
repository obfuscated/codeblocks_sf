#include "libraryresult.h"

#include <manager.h>
#include <configmanager.h>
#include <logmanager.h>

bool LibraryResult::operator == ( const LibraryResult& compareWith ) const
{
    if ( ShortCode   != compareWith.ShortCode   ) return false;
    if ( LibraryName != compareWith.LibraryName ) return false;
    if ( BasePath    != compareWith.BasePath    ) return false;
    if ( Description != compareWith.Description ) return false;
    return true;
}

void LibraryResult::SetGlobalVar() const
{
    ConfigManager * cfg = Manager::Get()->GetConfigManager(_T("gcv"));
    wxString activeSet = cfg->Read(_T("/active"));
    wxString curr = _T("/sets/") + activeSet + _T("/") + ShortCode;

    wxString _IncludePath = IncludePath.IsEmpty() ? _T("") : IncludePath[0];
    wxString _LibPath     = LibPath.IsEmpty()     ? _T("") : LibPath[0];
    wxString _ObjPath     = ObjPath.IsEmpty()     ? _T("") : ObjPath[0];

    wxString _CFlags;
    if ( !PkgConfigVar.IsEmpty() )
    {
        _CFlags.Append(_T(" `pkg-config "));
        _CFlags.Append(PkgConfigVar);
        _CFlags.Append(_T(" --cflags`"));
    }
    for ( size_t i=0; i<CFlags.Count(); i++ )
    {
        _CFlags.Append(_T(" "));
        _CFlags.Append(CFlags[i]);
    }
    for ( size_t i=1; i<IncludePath.Count(); i++ )
    {
        _CFlags.Append(_T(" -I"));
        _CFlags.Append(IncludePath[i]);
    }
    for ( size_t i=0; i<Defines.Count(); i++ )
    {
        _CFlags.Append(_T(" -D"));
        _CFlags.Append(Defines[i]);
    }
    _CFlags.Remove(0,1);

    wxString _LFlags;
    if ( !PkgConfigVar.IsEmpty() )
    {
        _LFlags.Append(_T(" `pkg-config "));
        _LFlags.Append(PkgConfigVar);
        _LFlags.Append(_T(" --libs`"));
    }
    for ( size_t i=0; i<LFlags.Count(); i++ )
    {
        _LFlags.Append(_T(" "));
        _LFlags.Append(LFlags[i]);
    }
    for ( size_t i=1; i<LibPath.Count(); i++ )
    {
        _LFlags.Append(_T(" -L"));
        _LFlags.Append(LibPath[i]);
    }
    for ( size_t i=1; i<ObjPath.Count(); i++ )
    {
        _LFlags.Append(_T(" -L"));
        _LFlags.Append(ObjPath[i]);
    }
    for ( size_t i=0; i<Libs.Count(); i++ )
    {
        _LFlags.Append(_T(" -l"));
        _LFlags.Append(Libs[i]);
    }
    _LFlags.Remove(0,1);

    wxString _BasePath = BasePath;
    if ( _BasePath.IsEmpty() )
    {
        // BasePath is mandatory so let's set it anyway
        if ( !PkgConfigVar.IsEmpty() )
            _BasePath = _T("`pkg-config ") + PkgConfigVar + _T(" --variable=prefix`");
        else
            _BasePath = _T("---");
    }

    cfg->Write(curr + _T("/base"),    _BasePath);
    cfg->Write(curr + _T("/include"), _IncludePath);
    cfg->Write(curr + _T("/lib"),     _LibPath);
    cfg->Write(curr + _T("/obj"),     _ObjPath);
    cfg->Write(curr + _T("/cflags"),  _CFlags);
    cfg->Write(curr + _T("/lflags"),  _LFlags);
}


void LibraryResult::DebugDump(const wxString& Prefix)
{
    LogManager::Get()->DebugLog( Prefix + _T(" --- ") + ShortCode + _T(" ---") );
    LogManager::Get()->DebugLog( Prefix + _T(" Name: ") + LibraryName );
    LogManager::Get()->DebugLog( Prefix + _T(" Description: ") + Description );
    LogManager::Get()->DebugLog( Prefix + _T(" BasePath: ") + BasePath );
    LogManager::Get()->DebugLog( Prefix + _T(" Pkg-Config: ") + PkgConfigVar );
}
