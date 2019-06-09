#include "sdk.h"

#include "cbart_provider.h"

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "logmanager.h"
#endif // CB_PRECOMP

cbArtProvider::cbArtProvider(const wxString &prefix, int size)
{
    m_prefix = wxString::Format(wxT("%s/%s/%dx%d/"), prefix.wx_str(),
                                wxT("resources.zip#zip:/images"), size, size);

    m_idToPath[wxT("core/file_open")] = wxT("fileopen.png");
    m_idToPath[wxT("core/history_clear")] = wxT("history_clear.png");
    m_idToPath[wxT("core/file_save")] = wxT("filesave.png");
    m_idToPath[wxT("core/file_save_as")] = wxT("filesaveas.png");
    m_idToPath[wxT("core/file_save_all")] = wxT("filesaveall.png");
    m_idToPath[wxT("core/file_close")] = wxT("fileclose.png");
    m_idToPath[wxT("core/file_print")] = wxT("fileprint.png");
    m_idToPath[wxT("core/exit")] = wxT("exit.png");
    m_idToPath[wxT("core/undo")] = wxT("undo.png");
    m_idToPath[wxT("core/redo")] = wxT("redo.png");
    m_idToPath[wxT("core/edit_cut")] = wxT("editcut.png");
    m_idToPath[wxT("core/edit_copy")] = wxT("editcopy.png");
    m_idToPath[wxT("core/edit_paste")] = wxT("editpaste.png");
    m_idToPath[wxT("core/bookmark_add")] = wxT("bookmark_add.png");
    m_idToPath[wxT("core/find")] = wxT("filefind.png");
    m_idToPath[wxT("core/find_in_files")] = wxT("findf.png");
    m_idToPath[wxT("core/find_next")] = wxT("filefindnext.png");
    m_idToPath[wxT("core/find_prev")] = wxT("filefindprev.png");
    m_idToPath[wxT("core/search_replace")] = wxT("searchreplace.png");
    m_idToPath[wxT("core/search_replace_in_files")] = wxT("searchreplacef.png");
    m_idToPath[wxT("core/goto")] = wxT("goto.png");
    m_idToPath[wxT("core/manage_plugins")] = wxT("plug.png");
    m_idToPath[wxT("core/help_info")] = wxT("info.png");
    m_idToPath[wxT("core/help_idea")] = wxT("idea.png");

    m_idToPath[wxT("core/dbg/run")] = wxT("dbgrun.png");
    m_idToPath[wxT("core/dbg/pause")] = wxT("dbgpause.png");
    m_idToPath[wxT("core/dbg/stop")] = wxT("dbgstop.png");
    m_idToPath[wxT("core/dbg/run_to")] = wxT("dbgrunto.png");
    m_idToPath[wxT("core/dbg/next")] = wxT("dbgnext.png");
    m_idToPath[wxT("core/dbg/step")] = wxT("dbgstep.png");
    m_idToPath[wxT("core/dbg/step_out")] = wxT("dbgstepout.png");
    m_idToPath[wxT("core/dbg/next_inst")] = wxT("dbgnexti.png");
    m_idToPath[wxT("core/dbg/step_inst")] = wxT("dbgstepi.png");
}

wxBitmap cbArtProvider::CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                     const wxSize &size)
{
    if (client != wxT("wxART_MENU_C"))
        return wxNullBitmap;

    MapStockIdToPath::const_iterator it = m_idToPath.find(id);
    if (it == m_idToPath.end())
    {
        wxString msg = wxString::Format(wxT("cbArtProvider: Unknown id '%s'"), id.wx_str());
        Manager::Get()->GetLogManager()->LogError(msg);
        return wxNullBitmap;
    }

    const wxString filepath = m_prefix + it->second;
    wxBitmap result = cbLoadBitmap(filepath);
    if (!result.IsOk())
    {
        wxString msg = wxString::Format(wxT("cbArtProvider: Cannot load image '%s'"),
                                        filepath.wx_str());
        Manager::Get()->GetLogManager()->LogError(msg);
    }

    return result;
}
