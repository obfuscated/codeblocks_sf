/** \file wxsimage.cpp
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


#include "wxsimage.h"
#include "../properties/wxsimagelisteditordlg.h"

namespace
{
    wxsRegisterItem<wxsImage> Reg(_T("Image"), wxsTTool, _T("Tools"), 73);
}

wxsImage::wxsImage(wxsItemResData *Data) :
    wxsTool(Data, &Reg.Info, 0, 0)
{
    int         n;
    wxString    ss, tt;
    wxFileName  fn;

    // nothing selected yet
    m_IsBuilt = false;
    m_ImageData.Clear();
    m_Include = false;

    // make the absolute directory path where we store XPM image files
    // this directory is always a sub-dir where the source code file is stored
    fn = Data->GetSrcFileName();
    ss = fn.GetPath((wxPATH_GET_VOLUME + wxPATH_GET_SEPARATOR));
    n  = ss.Len();
    ss = ss + _("wximages") + ss[n-1];

    m_IDir = ss;
    m_RDir = _("./wximages/");
    m_Base = fn.GetName();
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsImage::OnBuildCreatingCode()
{
    wxString    inc;
    wxString    vname;                  // this variable name
    wxString    bname;                  // name of the bitmap variable
    wxString    xname;                  // XPM data block name
    wxString    ss, tt;                 // general use

    // have we already been here?
    if(m_IsBuilt) {
        return;
    }
    m_IsBuilt = true;

    switch(GetLanguage())
    {
        case wxsCPP:
            {
                vname = GetVarName();
                bname = vname + _("_BMP");
                xname = vname + _("_XPM");
                AddHeader(_("<wx/image.h>"), GetInfo().ClassName, 0);
                AddHeader(_("<wx/bitmap.h>"), GetInfo().ClassName, 0);

                // store the XPM data someplace
                StoreXpmData();

                // if there is no data, then just make empty image and bitmap
                if(m_ImageData.Count() == 0) {
                    Codef(_T("%s = new wxImage();\n"), vname.c_str());
                    Codef(_T("%s = new wxBitmap();\n"), bname.c_str());
                }
                // else fill it with XPM data
                else {
                    Codef(_T("%s = new wxImage(%s);\n"),  vname.c_str(), xname.c_str());
                    Codef(_T("%s = new wxBitmap(%s);\n"), bname.c_str(), xname.c_str());
                }

                BuildSetupWindowCode();
                return;
            }

        default:
            {
                wxsCodeMarks::Unknown(_T("wxsImage::OnBuildCreatingCode"), GetLanguage());
            }
    }
}

/*! \brief Enumerate the tool's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsImage::OnEnumToolProperties(long Flags)
{
    // starting a new build cycle
    m_IsBuilt = false;
    m_Context = GetCoderContext();

    // details
    WXS_IMAGE(wxsImage, m_ImageData, _("Image"), _T("image"));
    WXS_ARRAYSTRING(wxsImage, m_ImageData, _("Image as Text"), _T("image_text"), _("item2"));
    WXS_BOOL(wxsImage, m_Include, _("Use Include File"), _T("use_include"), false);
};

/*! \brief Declare the var with a bitmap.
 *
 * \return void
 *
 */
void wxsImage::OnBuildDeclarationsCode()
{
    wxString    vname;
    wxString    bname;

    vname = GetVarName();
    bname = vname + _T("_BMP");

    // put in all the vars, although some might be ignored later
    AddDeclaration(_T("wxImage               *") + vname + _T(";"));
    AddDeclaration(_T("wxBitmap              *") + bname + _T(";"));
}

/*! \brief Save the XPM data block; may be stored as a #include file.
 *
 * \param void
 * \return void
 *
 */
void wxsImage::StoreXpmData(void) {
    int                 i, n;
    wxString    vname;
    wxString    xname;
    wxString    ss, tt;
    wxFile          ff;

    // nothing to store?
    if(m_ImageData.Count() == 0) return;

    // important names
    vname = GetVarName();
    xname = vname + _T("_XPM");

    // make a single string with the proper name
    tt = _T("");
    n = m_ImageData.GetCount();
    if(n > 5){
        n = (n * m_ImageData.Item(n - 2).Length()) + 100;
        tt.Alloc(n);
    }

    for(i = 0;i < (int)m_ImageData.GetCount();i++){
        ss = m_ImageData.Item(i);
        if(ss.Find(_T("xpm_data")) >= 0){
            ss.Replace(_T("xpm_data"), xname);
        }

        tt += ss;
        tt += _T("\n");
    }

    // store as an include file
    if(m_Include) {
        if(! wxFileName::DirExists(m_IDir)){
            wxFileName::Mkdir(m_IDir);
        }
        ss  = m_IDir;
        ss += m_Base;
        ss += _T("_");
        ss += xname;
        ss += _T(".xpm");
        ff.Open(ss, wxFile::write);
        ff.Write(tt);
        ff.Close();

        ss = _T("\"");
        ss += m_RDir;
        ss += m_Base;
        ss += _T("_");
        ss += xname;
        ss += _T(".xpm");
        ss += _T("\"");

        AddHeader(ss, GetInfo().ClassName, 0);
    }
    // store in-line in the main header file
    else {
        Codef(tt);
    }
}

/*! \brief Get an image preview.
 *
 * \param void
 * \return wxBitmap
 *
 */
wxBitmap wxsImage::GetPreview(void)
{
    wxBitmap    bmp;

    if(m_ImageData.GetCount() == 0){
        return wxNullBitmap;
    }

    wxsImageListEditorDlg::ArrayToBitmap(m_ImageData, bmp);
    return bmp;
}

/*! \brief This just exposes the "OnBuildCreatingCode()" method.
 *
 * \param void
 * \return void
 *
 */
void wxsImage::DoBuild(void)
{
    BuildCode(m_Context);
}








