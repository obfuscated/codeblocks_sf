/** \file wxsimagelisteditordlg.h
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

#ifndef WXSIMAGELISTEDITORDLG_H
#define WXSIMAGELISTEDITORDLG_H


//(*Headers(wxsImageListEditorDlg)
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/colordlg.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/panel.h>
#include "scrollingdialog.h"
#include <wx/stattext.h>
//*)

#include "wxsimagelistproperty.h"
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include "wxsbitmapiconeditordlg.h"

class wxsItem;

/*! \brief Class for wxsImageListEditorDlg. */
class wxsImageListEditorDlg: public wxScrollingDialog
{
    public:

        wxsImageListEditorDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~wxsImageListEditorDlg();
        bool                        Execute(wxString &inName, wxArrayString &aImageData);
        static  void            ImageToArray(wxImage &inImage, wxArrayString &outArray);
        static  void            ArrayToImage(wxArrayString &inArray, wxImage &outImage);
        static  void            BitmapToArray(wxBitmap &inBitmap, wxArrayString &outArray);
        static  void            ArrayToBitmap(wxArrayString &inArray, wxBitmap &outBitmap);
        static  void            ImageListToArray(wxImageList &inList, wxArrayString &outArray);
        static  void            ArrayToImageList(wxArrayString &inArray, wxImageList &outList);
        static  void            CopyImageList(wxImageList &inList, wxImageList &outList);
        static  wxsItem     *FindTool(wxsItem *inItem, wxString inName);
        static  int               CalcArraySize(wxArrayString &inArray);
        static  bool            SaveXPM(wxImage * image, wxOutputStream& stream);

        //(*Declarations(wxsImageListEditorDlg)
        wxStaticText* StaticText24;
        wxStaticText* StaticText22;
        wxBitmapButton* bSaveList;
        wxBitmapButton* bDel;
        wxPanel* Panel1;
        wxPanel* Panel11;
        wxPanel* Panel6;
        wxStaticText* StaticText21;
        wxStaticText* StaticText13;
        wxStaticText* StaticText14;
        wxPanel* Panel7;
        wxStaticText* StaticText15;
        wxStaticText* StaticText17;
        wxStaticText* StaticText28;
        wxButton* bCancel;
        wxPanel* Panel12;
        wxPanel* Panel9;
        wxPanel* Panel8;
        wxColourDialog* ColourDialog1;
        wxStaticText* StaticText20;
        wxStaticText* StaticText18;
        wxStaticText* StaticText1;
        wxStaticText* StaticText10;
        wxPanel* Panel10;
        wxStaticText* StaticText16;
        wxBitmapButton* bLeft;
        wxPanel* Panel2;
        wxStaticText* StaticText3;
        wxPanel* Panel4;
        wxStaticText* StaticText23;
        wxPanel* Panel5;
        wxBitmapButton* bAdd;
        wxStaticText* StaticText8;
        wxStaticText* StaticText12;
        wxFileDialog* FileDialog1;
        wxBitmapButton* bRight;
        wxButton* bColor;
        wxPanel* Panel3;
        wxBitmapButton* bSave;
        wxStaticText* StaticText7;
        wxBitmapButton* bClear;
        wxBitmapButton* bRead;
        wxCheckBox* cxTransparent;
        wxStaticText* StaticText4;
        wxStaticText* StaticText5;
        wxStaticText* StaticText2;
        wxStaticText* StaticText27;
        wxStaticText* StaticText26;
        wxStaticText* StaticText6;
        wxButton* bOK;
        wxStaticText* StaticText19;
        wxStaticText* StaticText29;
        wxStaticText* StaticText9;
        wxStaticText* StaticText11;
        wxStaticText* StaticText25;
        //*)

    protected:

        //(*Identifiers(wxsImageListEditorDlg)
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT15;
        static const long ID_STATICTEXT16;
        static const long ID_STATICTEXT17;
        static const long ID_STATICTEXT18;
        static const long ID_STATICTEXT26;
        static const long ID_STATICTEXT19;
        static const long ID_STATICTEXT27;
        static const long ID_BITMAPBUTTON1;
        static const long ID_PANEL2;
        static const long ID_STATICTEXT2;
        static const long ID_PANEL3;
        static const long ID_STATICTEXT3;
        static const long ID_PANEL4;
        static const long ID_STATICTEXT4;
        static const long ID_PANEL5;
        static const long ID_STATICTEXT5;
        static const long ID_PANEL8;
        static const long ID_STATICTEXT6;
        static const long ID_PANEL6;
        static const long ID_STATICTEXT7;
        static const long ID_PANEL7;
        static const long ID_STATICTEXT8;
        static const long ID_PANEL9;
        static const long ID_STATICTEXT9;
        static const long ID_PANEL10;
        static const long ID_STATICTEXT10;
        static const long ID_PANEL11;
        static const long ID_STATICTEXT12;
        static const long ID_BITMAPBUTTON2;
        static const long ID_STATICTEXT11;
        static const long ID_PANEL1;
        static const long ID_STATICTEXT23;
        static const long ID_BITMAPBUTTON5;
        static const long ID_STATICTEXT20;
        static const long ID_CHECKBOX1;
        static const long ID_BUTTON2;
        static const long ID_STATICTEXT21;
        static const long ID_BITMAPBUTTON3;
        static const long ID_STATICTEXT13;
        static const long ID_STATICTEXT22;
        static const long ID_PANEL12;
        static const long ID_STATICTEXT28;
        static const long ID_BITMAPBUTTON4;
        static const long ID_STATICTEXT14;
        static const long ID_BITMAPBUTTON6;
        static const long ID_STATICTEXT24;
        static const long ID_BITMAPBUTTON7;
        static const long ID_STATICTEXT25;
        static const long ID_BITMAPBUTTON8;
        static const long ID_STATICTEXT29;
        static const long ID_BUTTON1;
        static const long ID_BUTTON4;
        //*)

    private:

        //(*Handlers(wxsImageListEditorDlg)
        void OnbAddClick(wxCommandEvent& event);
        void OnbReadClick(wxCommandEvent& event);
        void OnPanel1Paint(wxPaintEvent& event);
        void OnbColorClick(wxCommandEvent& event);
        void OnPanel1LeftUp(wxMouseEvent& event);
        void OncxTransparentClick(wxCommandEvent& event);
        void OnbOKClick(wxCommandEvent& event);
        void OnbCancelClick(wxCommandEvent& event);
        void OnbLeftClick(wxCommandEvent& event);
        void OnbRightClick(wxCommandEvent& event);
        void OnPanel2LeftUp(wxMouseEvent& event);
        void OnPanel3LeftUp(wxMouseEvent& event);
        void OnPanel4LeftUp(wxMouseEvent& event);
        void OnPanel5LeftUp(wxMouseEvent& event);
        void OnPanel8LeftUp(wxMouseEvent& event);
        void OnPanel6LeftUp(wxMouseEvent& event);
        void OnPanel7LeftUp(wxMouseEvent& event);
        void OnPanel9LeftUp(wxMouseEvent& event);
        void OnPanel10LeftUp(wxMouseEvent& event);
        void OnPanel11LeftUp(wxMouseEvent& event);
        void OnPanel12Paint(wxPaintEvent& event);
        void OnbDelClick(wxCommandEvent& event);
        void OnbClearClick(wxCommandEvent& event);
        void OnbSaveClick(wxCommandEvent& event);
        void OnbSaveListClick(wxCommandEvent& event);
        void PaintPreviewPanel(wxPaintEvent& event);
        //*)

        void                PreviewImport(void);
        void                PreviewList(void);
        void                PreviewSelected(void);
        void                UpdateEnabled(void);
        void                SelectPreviewPanel(int aIndex);
        void                PaintPanel(wxPaintDC &aDC, wxPanel *aPanel, wxBitmap &aBitmap, bool aHot = false);

        wxImageList                         m_ImageList;                     //!< Working copy of image list.
        wxImage                                 m_ImportImage;                //!< The thing that gets displayed.
        wxColour                                m_ImportMask;                  //!< Mask color.
        wxsBitmapIconEditorDlg     *m_ImageDialog;            //!< Dialog to import external images.
        wxsBitmapIconData           m_ImageData;                   //!< Image data for the dialog.
        int                                             m_FirstImage;                    //!< Left-most image displayed in preview.
        wxPanel                                *m_PreviewPanel[10];      //!< The list of preview images.
        wxStaticText                           *m_PreviewLabel[10];      //!< The preview list label.
        int                                             m_PreviewSelect;             //!< The selected preview image.

        DECLARE_EVENT_TABLE()
};

#endif
