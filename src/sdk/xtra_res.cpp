/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "xtra_res.h"
    #include "logmanager.h"
    #include "scrollingdialog.h"
    #include <wx/wx.h>
#endif

#include <wx/xml/xml.h>

/////////////////////////////////////////////////////////////////////////////
// Name:        xh_toolb.cpp
// Purpose:     XRC resource for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/08/11
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
// Modified by Ricardo Garcia for Code::Blocks
// Comment: Things would've been much easier if field m_isInside had been
//          protected instead of private! >:(
/////////////////////////////////////////////////////////////////////////////

wxToolBarAddOnXmlHandler::wxToolBarAddOnXmlHandler() :
    m_isInside(FALSE), m_isAddon(false), m_toolbar(NULL), m_ImageSize(0)
{
    XRC_ADD_STYLE(wxTB_FLAT);
    XRC_ADD_STYLE(wxTB_DOCKABLE);
    XRC_ADD_STYLE(wxTB_VERTICAL);
    XRC_ADD_STYLE(wxTB_HORIZONTAL);
    XRC_ADD_STYLE(wxTB_TEXT);
    XRC_ADD_STYLE(wxTB_NOICONS);
    XRC_ADD_STYLE(wxTB_NODIVIDER);
    XRC_ADD_STYLE(wxTB_NOALIGN);
}

void wxToolBarAddOnXmlHandler::SetToolbarImageSize(int size)
{
    m_ImageSize = size;
    m_PathReplaceString = wxString::Format(wxT("%dx%d"), size, size);
}

/// Copied this from wx's GetBitmap implementation.
/// We need to do this because we want to replace 22x22 in the path with a string matching our
/// resolution.
/// @note This version doesn't support loading stock art images only files!
wxBitmap wxToolBarAddOnXmlHandler::LoadBitmap(const wxString& name, double scaleFactor)
{
#if wxUSE_FILESYSTEM
    wxFSFile *fsfile = GetCurFileSystem().OpenFile(name, wxFS_READ | wxFS_SEEKABLE);
    if (fsfile == NULL)
    {
        LogManager *logger = Manager::Get()->GetLogManager();
        logger->LogError(wxString::Format(wxT("Cannot open bitmap resource \"%s\""), name));
        return wxNullBitmap;
    }
    wxImage img(*(fsfile->GetStream()));
    delete fsfile;
#else
    wxImage img(name);
#endif

    if (!img.IsOk())
    {
        LogManager *logger = Manager::Get()->GetLogManager();
        logger->LogError(wxString::Format(wxT("Cannot create bitmap from \"%s\""), name));
        return wxNullBitmap;
    }

#if defined(__WXOSX__) || (defined(__WXGTK3__) && wxCHECK_VERSION(3, 1, 2))
    return wxBitmap(img, -1, scaleFactor);
#else
    (void)scaleFactor;
    return wxBitmap(img);
#endif // defined(__WXOSX__) || (defined(__WXGTK3__) && wxCHECK_VERSION(3, 1, 2))
}


wxBitmap wxToolBarAddOnXmlHandler::GetCenteredBitmap(const wxString& param, wxSize size,
                                                     double scaleFactor)
{
    wxString name = GetParamValue(param);
    if (name.empty())
        return wxNullBitmap;

    wxString finalName = name;
    finalName.Replace(wxT("22x22"), m_PathReplaceString);

    wxBitmap bitmap = LoadBitmap(finalName, scaleFactor);
    if (!bitmap.Ok())
        return bitmap;

    int bw = bitmap.GetWidth();
    int bh = bitmap.GetHeight();
    if (size * scaleFactor == wxSize(bw, bh))
        return bitmap;

    LogManager *logger = Manager::Get()->GetLogManager();
    logger->LogWarning(wxString::Format(wxT("Image \"%s\" with size [%dx%d] doesn't match ")
                                        wxT("requested size [%dx%d] resizing (scale factor %.3f)!"),
                                        finalName.wx_str(), bw, bh,
                                        int(size.x * scaleFactor), int(size.y * scaleFactor),
                                        scaleFactor));

    wxImage image = bitmap.ConvertToImage();

    int w = size.GetWidth();
    int h = size.GetHeight();
    int x = (w - bw) / 2;
    int y = (h - bh) / 2;

    // If the image is bigger than the current size our code for resizing would do overflow the
    // buffers. Until the code is made to handle such cases just rescale the image.
    if (size.x < bw || size.y < bh)
        image.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
    else if (image.HasAlpha()) // Resize doesn't handle Alpha... :-(
    {
        const unsigned char *data = image.GetData();
        const unsigned char *alpha = image.GetAlpha();
        unsigned char *rgb = (unsigned char *) calloc(w * h, 3);
        unsigned char *a = (unsigned char *) calloc(w * h, 1);

        // copy Data/Alpha from smaller bitmap to larger bitmap
        for (int row = 0; row < bh; row++)
        {
            memcpy(rgb + ((row + y) * w + x) * 3, data + (row * bw) * 3, bw * 3);
            memcpy(a + ((row + y) * w + x), alpha + (row * bw), bw);
        }

        image = wxImage(w, h, rgb, a);
    }
    else
        image.Resize(size, wxPoint(x,y));

    return wxBitmap(image);
}

wxObject *wxToolBarAddOnXmlHandler::DoCreateResource()
{
    wxToolBar* toolbar=NULL;
    if (m_class == _T("tool"))
    {
        wxCHECK_MSG(m_toolbar, NULL, _("Incorrect syntax of XRC resource: tool not within a toolbar!"));

        const wxSize bitmapSize = m_toolbar->GetToolBitmapSize();
        const double scaleFactor = cbGetContentScaleFactor(*m_toolbar);

        if (GetPosition() != wxDefaultPosition)
        {
            m_toolbar->AddTool(GetID(),
            #if wxCHECK_VERSION(3, 0, 0)
                               wxEmptyString,
            #endif
                               GetCenteredBitmap(_T("bitmap"), bitmapSize, scaleFactor),
                               GetCenteredBitmap(_T("bitmap2"), bitmapSize, scaleFactor),
            #if !wxCHECK_VERSION(3, 0, 0)
                               GetBool(_T("toggle")),
                               GetPosition().x,
                               GetPosition().y,
                               NULL,
            #else
                               wxITEM_NORMAL,
            #endif
                               GetText(_T("tooltip")),
                               GetText(_T("longhelp")));
        }
        else
        {
            wxItemKind kind = wxITEM_NORMAL;
            if (GetBool(_T("radio")))
                kind = wxITEM_RADIO;
            if (GetBool(_T("toggle")))
            {
                wxASSERT_MSG( kind == wxITEM_NORMAL,
                              _("can't have both toggleable and radion button at once") );
                kind = wxITEM_CHECK;
            }
            m_toolbar->AddTool(GetID(),
                               GetText(_T("label")),
                               GetCenteredBitmap(_T("bitmap"), bitmapSize, scaleFactor),
                               GetCenteredBitmap(_T("bitmap2"), bitmapSize, scaleFactor),
                               kind,
                               GetText(_T("tooltip")),
                               GetText(_T("longhelp")));
        }

        if (GetBool(_T("disabled")))
            m_toolbar->EnableTool(GetID(),false);

        return m_toolbar; // must return non-NULL
    }

    else if (m_class == _T("separator"))
    {
        wxCHECK_MSG(m_toolbar, NULL, _("Incorrect syntax of XRC resource: separator not within a toolbar!"));
        m_toolbar->AddSeparator();
        return m_toolbar; // must return non-NULL
    }
    else /*<object class="wxToolBar">*/
    {
        m_isAddon=(m_class == _T("wxToolBarAddOn"));
        if(m_isAddon)
        { // special case: Only add items to toolbar
          toolbar=(wxToolBar*)m_instance;
          // XRC_MAKE_INSTANCE(toolbar, wxToolBar);
        }
        else
        {
            int style = GetStyle(_T("style"), wxNO_BORDER | wxTB_HORIZONTAL);
            #ifdef __WXMSW__
            if (!(style & wxNO_BORDER)) style |= wxNO_BORDER;
            #endif

            // XRC_MAKE_INSTANCE(toolbar, wxToolBar)
            if (m_instance)
                toolbar = wxStaticCast(m_instance, wxToolBar);
            if (!toolbar)
                toolbar = new wxToolBar;

            toolbar->Create(m_parentAsWindow,
                             GetID(),
                             GetPosition(),
                             GetSize(),
                             style,
                             GetName());
            wxSize margins = GetSize(_T("margins"));
            if (!(margins == wxDefaultSize))
                toolbar->SetMargins(margins.x, margins.y);
            long packing = GetLong(_T("packing"), -1);
            if (packing != -1)
                toolbar->SetToolPacking(packing);
            long separation = GetLong(_T("separation"), -1);
            if (separation != -1)
                toolbar->SetToolSeparation(separation);
        }

        wxXmlNode *children_node = GetParamNode(_T("object"));
        if (!children_node)
           children_node = GetParamNode(_T("object_ref"));

        if (children_node == NULL) return toolbar;

        m_isInside = TRUE;
        m_toolbar = toolbar;

        wxXmlNode *n = children_node;

        while (n)
        {
            if ((n->GetType() == wxXML_ELEMENT_NODE) &&
                (n->GetName() == _T("object") || n->GetName() == _T("object_ref")))
            {
                wxObject *created = CreateResFromNode(n, toolbar, NULL);
                wxControl *control = wxDynamicCast(created, wxControl);
                if (!IsOfClass(n, _T("tool")) &&
                    !IsOfClass(n, _T("separator")) &&
                    control != NULL &&
                    control != toolbar)
                {
                    //Manager::Get()->GetLogManager()->DebugLog(F(_T("control=%p, parent=%p, toolbar=%p"), control, control->GetParent(), toolbar));
                    toolbar->AddControl(control);
                }
            }
            n = n->GetNext();
        }

        toolbar->Realize();

        m_isInside = FALSE;
        m_toolbar = NULL;

        if(!m_isAddon)
        {
            if (m_parentAsWindow && !GetBool(_T("dontattachtoframe")))
            {
                wxFrame *parentFrame = wxDynamicCast(m_parent, wxFrame);
                if (parentFrame)
                    parentFrame->SetToolBar(toolbar);
            }
        }
        m_isAddon=false;
        return toolbar;
    }
}

bool wxToolBarAddOnXmlHandler::CanHandle(wxXmlNode *node)
{
// NOTE (mandrav#1#): wxXmlResourceHandler::IsOfClass() doesn't work in unicode (2.6.2)
// Don't ask why. It does this and doesn't work for our custom handler:
//    return node->GetPropVal(wxT("class"), wxEmptyString) == classname;
//
// This works though:
//    return node->GetPropVal(wxT("class"), wxEmptyString).Matches(classname);
//
// Don't ask me why... >:-|

    #if wxCHECK_VERSION(3, 0, 0)
    bool istbar = node->GetAttribute(wxT("class"), wxEmptyString).Matches(_T("wxToolBarAddOn"));
    bool istool = node->GetAttribute(wxT("class"), wxEmptyString).Matches(_T("tool"));
    bool issep = node->GetAttribute(wxT("class"), wxEmptyString).Matches(_T("separator"));
    #else
    bool istbar = node->GetPropVal(wxT("class"), wxEmptyString).Matches(_T("wxToolBarAddOn"));
    bool istool = node->GetPropVal(wxT("class"), wxEmptyString).Matches(_T("tool"));
    bool issep = node->GetPropVal(wxT("class"), wxEmptyString).Matches(_T("separator"));
    #endif

    return ((!m_isInside && istbar) ||
            (m_isInside && istool) ||
            (m_isInside && issep));
}



IMPLEMENT_DYNAMIC_CLASS(wxScrollingDialogXmlHandler, wxDialogXmlHandler)

wxScrollingDialogXmlHandler::wxScrollingDialogXmlHandler() : wxDialogXmlHandler()
{
}

wxObject *wxScrollingDialogXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(dlg, wxScrollingDialog);

    dlg->Create(m_parentAsWindow,
                GetID(),
                GetText(wxT("title")),
                wxDefaultPosition, wxDefaultSize,
                GetStyle(wxT("style"), wxDEFAULT_DIALOG_STYLE),
                GetName());

    if (HasParam(wxT("size")))
        dlg->SetClientSize(GetSize(wxT("size"), dlg));
    if (HasParam(wxT("pos")))
        dlg->Move(GetPosition());
    if (HasParam(wxT("icon")))
        dlg->SetIcon(GetIcon(wxT("icon"), wxART_FRAME_ICON));

    SetupWindow(dlg);

    CreateChildren(dlg);

    if (GetBool(wxT("centered"), false))
        dlg->Centre();

    return dlg;
}

bool wxScrollingDialogXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxScrollingDialog"));
}
