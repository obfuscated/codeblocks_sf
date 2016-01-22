/** \file wxsimagelist.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* This code was taken from the wxSmithImage plug-in, copyright Ron Collins
* and released under the GPL.
*
*/

#include "wxsimagelist.h"
#include "../properties/wxsimagelisteditordlg.h"

namespace
{
    wxsRegisterItem<wxsImageList> Reg(_T("ImageList"), wxsTTool, _T("Tools"), 72);
}

wxsImageList::wxsImageList(wxsItemResData *Data):
    wxsTool(Data, &Reg.Info, 0, 0)
{
    int         n;
    wxString    ss, tt;
    wxFileName  fn;

    m_IsBuilt = false;
    m_ImageData.Clear();
    m_Width   = 16;
    m_Height  = 16;
    m_Count   = 0;
    m_Include = false;

    // make the absolute directory path where we store XPM image files
    // this directory is always a sub-dir where the source code file is stored
    fn = Data->GetSrcFileName();
    ss = fn.GetPath((wxPATH_GET_VOLUME + wxPATH_GET_SEPARATOR));
    n  = ss.Len();
    ss = ss + _T("wximages") + ss[n-1];

    m_IDir = ss;
    m_RDir = _T("./wximages/");
    m_Base = fn.GetName();
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsImageList::OnBuildCreatingCode()
{
    int         i;
    wxString    inc;
    wxString    vname;  // this variable name
    wxString    bname;  // name of the bitmap variable
    wxString    fbase;  // base name of XPM file without dirs or extension
    wxString    fabs;   // absolute name of XPM file
    wxString    frel;   // relative
    wxString    dname;  // name of XPM data array
    wxBitmap    bmp;    // preview bitmap saved as XPM
    wxString    ss, tt; // general use

    // have we already been here?
    if(m_IsBuilt){
        return;
    }
    m_IsBuilt = true;

    switch(GetLanguage())
    {
        case wxsCPP:
            {
                AddHeader(_("<wx/imaglist.h>"), GetInfo().ClassName, 0);

                // store the XPM data someplace
                StoreXpmData();

                vname = GetVarName();
                // if there is no data, then just make empty image and bitmap
                if(m_Count == 0){
                    Codef(_T("%s = new wxImageList(%d, %d, 1);\n"), vname.wx_str(), m_Width, m_Height);
                }
                // else fill it with XPM data
                else{
                    Codef(_T("%s = new wxImageList(%d, %d, %d);\n"),  vname.wx_str(), m_Width, m_Height, (m_Count + 1));
                    for(i = 0; i < m_Count; i++) {
                        ss.Printf(_("%s_%d_XPM"), vname.wx_str(), i);
                        Codef(_T("%s->Add(wxBitmap(%s));\n"), vname.wx_str(), ss.wx_str());
                    }
                }

                BuildSetupWindowCode();
                return;
            }

        case wxsUnknownLanguage: // fall through
        default:
            {
                wxsCodeMarks::Unknown(_T("wxsImageList::OnBuildCreatingCode"), GetLanguage());
            }
    }
}

/*! \brief Enumerate the tool's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsImageList::OnEnumToolProperties(cb_unused long Flags)
{
    // starting a new build cycle
    m_IsBuilt = false;
    m_Context = GetCoderContext();

    // details for the image list
    WXS_IMAGELIST(wxsImageList, m_ImageData, _T("Image List"), _T("image_list"));
    WXS_ARRAYSTRING(wxsImageList, m_ImageData, _("Images as Text"), _T("image_text"), _T("item2"));
    WXS_LONG(wxsImageList, m_Width,   _("Image Width"),      _T("image_width"),   16);
    WXS_LONG(wxsImageList, m_Height,  _("Image Height"),     _T("image_height"),  16);
    WXS_LONG(wxsImageList, m_Count,   _("Image Count"),      _T("image_count"),   0);
    WXS_BOOL(wxsImageList, m_Include, _("Use Include File"), _T("use_include"), false);
};

/*! \brief Save XPM data either in the code or in a separate header file.
 *
 * \param void
 * \return void
 *
 */
void wxsImageList::StoreXpmData(void)
{
    int         i, n;
    long        ll;
    wxString    vname;
    wxString    xname;
    wxString    ss, tt, vv;
    wxFile      ff;

    // important names
    vname = GetVarName();

    // if no XPM images, then just store the image size
    n = m_ImageData.GetCount();
    if(n <= 2){
        if(m_Width <= 0) m_Width = 16;
        if(m_Height <= 0) m_Height = 16;
        m_Count = 0;

        m_ImageData.Clear();
        ss.Printf(_T("%ld"), m_Width);
        m_ImageData.Add(ss);

        ss.Printf(_T("%ld"), m_Height);
        m_ImageData.Add(ss);
    }
    // else reset the displayed size
    else{
        ss = m_ImageData.Item(0);
        if(ss.ToLong(&ll)) m_Width = ll;

        ss = m_ImageData.Item(1);
        if(ss.ToLong(&ll)) m_Height = ll;

        m_Count = 0;
        for(i = 0;i < (int)m_ImageData.GetCount();i++){
            ss = m_ImageData.Item(i);
            if(ss.Find(_T("xpm_data")) >= 0){
                m_Count += 1;
            }
        };
    };

    // are we finished?
    if(m_Count == 0){
        return;
    }

    // need to create a #include file?
    if(m_Include){
        if(! wxFileName::DirExists(m_IDir)) wxFileName::Mkdir(m_IDir);
        ss  = m_IDir;
        ss += m_Base;
        ss += _T("_");
        ss += vname;
        ss += _T(".xpm");
        ff.Open(ss, wxFile::write);
    }

    // go thru entire array, pulling out one XPM at a time into a single string
    n = 0;
    i = 2;
    tt = _("");
    while(i < (int)m_ImageData.GetCount()){
        ss = m_ImageData.Item(i);
        i += 1;

        // the beginning of a new XPM image means the end of the previous image
        if(ss.Find(_T("xpm_data")) >= 0){
            vv.Printf(_T("%s_%d_XPM"), vname.wx_str(), n);
            ss.Replace(_T("xpm_data"), vv);
            n += 1;

            if(tt.Length() > 0){
                if(m_Include){
                    ff.Write(tt);
                }
                else{
                    Codef(tt);
                }
            }

            tt  = ss;
            tt += _T("\n");
        }
        // else just another data line
        else {
            tt += ss;
            tt += _T("\n");
        }
    }

    // the left-overs
    if(tt.Length() > 0){
        if(m_Include){
            ff.Write(tt);
        }
        else{
            Codef(tt);
        }
    }

    // include the #include file
    if(m_Include){
        ff.Close();

        ss = _T("\"");
        ss += m_RDir;
        ss += m_Base;
        ss += _T("_");
        ss += vname;
        ss += _T(".xpm");
        ss += _T("\"");

        AddHeader(ss, GetInfo().ClassName, 0);
    }
}

int  wxsImageList::GetCount(void)
{
    return m_Count;
}

/*! \brief Get an image preview by index.
 *
 * \param inIndex int
 * \return wxBitmap
 *
 */
wxBitmap wxsImageList::GetPreview(int inIndex)
{
    int             i, j, n;
    wxString        ss, tt;
    wxArrayString   aa;
    wxBitmap        bmp;

    // no such image?
    if((inIndex < 0) || (inIndex >= m_Count)){
        return wxNullBitmap;
    }
    if(m_ImageData.GetCount() == 0){
        return wxNullBitmap;
    }

    // count down to the start of that image data
    n = -1;             // found index at start of data
    j = 0;              // counter of data blocks
    i = 0;              // index into m_ImageData

    while((i < (int)m_ImageData.GetCount()) && (n < 0)){
        ss = m_ImageData.Item(i);
        i += 1;

        if(ss.Find(_T("xpm_data")) >= 0){
            if(j == inIndex){
                n = i;
            }
            j += 1;
        }
    }

    // still no data block?
    if(n < 0){
        return wxNullBitmap;\
    }

    // save that first line
    aa.Clear();
    aa.Add(ss);

    // copy out the data block (until the next "xpm_data")
    i = n;
    n = -1;
    while((i < (int)m_ImageData.GetCount()) && (n < 0)){
        ss = m_ImageData.Item(i);
        i += 1;

        if(ss.Find(_T("xpm_data")) >= 0){
            n = i;
        }
        else{
            aa.Add(ss);
        }
    }

    // turn that data block into a bitmap
    wxsImageListEditorDlg::ArrayToBitmap(aa, bmp);

    // done
    return bmp;
}

/*! \brief Get the image list.
 *
 * \param aImageList wxImageList&
 * \return void
 *
 */
void wxsImageList::GetImageList(wxImageList &aImageList)
{
    aImageList.RemoveAll();
    wxsImageListEditorDlg::ArrayToImageList(m_ImageData, aImageList);
}

/*! \brief This just exposes the "OnBuildCreatingCode()" method.
 *
 * \param void
 * \return void
 *
 */
void wxsImageList::DoBuild(void)
{
    BuildCode(m_Context);
}

