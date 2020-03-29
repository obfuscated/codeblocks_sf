/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef MAINFRAME_H
#define MAINFRAME_H


//(*Headers(MainFrame)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checklst.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)

#include <wx/arrstr.h>
#include <vector>

#include <tinyxml.h>

class MainFrame: public wxFrame
{
//***********************************************************************
	public:
//***********************************************************************

		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();


		//(*Identifiers(MainFrame)
		static const long ID_LBL_STEPS;
		static const long ID_LBL_FILE_SRC;
		static const long ID_LBL_FILE_DST;
		static const long ID_TXT_FILE_SRC;
		static const long ID_BTN_FILE_SRC;
		static const long ID_TXT_FILE_DST;
		static const long ID_BTN_FILE_DST;
		static const long ID_CFG_SRC;
		static const long ID_LST_CFG;
		static const long ID_BTN_TRANSFER;
		static const long ID_BTN_UNCHECK;
		static const long ID_BTN_EXPORT_ALL;
		static const long ID_BTN_EXPORT;
		static const long ID_BTN_SAVE;
		static const long ID_BTN_CLOSE;
		//*)

//***********************************************************************
	protected:
//***********************************************************************

		//(*Handlers(MainFrame)
		void OnBtnFileSrcClick(wxCommandEvent& event);
		void OnBtnFileDstClick(wxCommandEvent& event);
		void OnBtnTransferClick(wxCommandEvent& event);
		void OnBtnUncheckClick(wxCommandEvent& event);
		void OnBtnExportAllClick(wxCommandEvent& event);
		void OnBtnExportClick(wxCommandEvent& event);
		void OnBtnSaveClick(wxCommandEvent& event);
		void OnBtnCloseClick(wxCommandEvent& event);
		//*)


		//(*Declarations(MainFrame)
		wxBoxSizer* bszSteps;
		wxGridSizer* grsAction;
		wxTextCtrl* txtFileDst;
		wxStaticBoxSizer* sbsSteps;
		wxBoxSizer* bszMain;
		wxStaticText* lblFileSrc;
		wxCheckListBox* clbCfgSrc;
		wxButton* btnFileSrc;
		wxFlexGridSizer* flsFileDst;
		wxFlexGridSizer* flsFileSrc;
		wxTextCtrl* txtFileSrc;
		wxGridSizer* grsFile;
		wxGridSizer* grsCfg;
		wxButton* btnFileDst;
		wxListBox* lstCfgDst;
		wxStaticText* lblFileDst;
		wxGridSizer* grsFileLabel;
		wxStaticText* lblSteps;
		//*)

//***********************************************************************
	private:
//***********************************************************************

		wxString      FileSelector();
    bool          LoadConfig  (const wxString& filename, TiXmlDocument** doc);
    bool          SameConfig  (const wxString& filename, wxTextCtrl* txt);
    void          OfferConfig (TiXmlDocument* config, wxListBox* listbox,
                               std::vector<TiXmlNode*> *nodes);
    void          OfferNode   (TiXmlNode** node, wxListBox* listbox,
                               std::vector<TiXmlNode*> *nodes,
                               const wxString& prefix = wxT(""));
    bool          TransferNode(TiXmlNode** node, const wxArrayString& path);
    void          AttachNode(size_t idx, TiXmlElement* root);
    wxArrayString PathToArray (const wxString& path);

    // The following methods to load/save a TinyXML document are taken and
    // modified from C::B. Changes done there may have to be applied here, too.
    // Modifications: Remove the dependency to C::B SDK (C::B Manager classes).
    bool          TiXmlLoadDocument(const wxString& filename, TiXmlDocument* doc);
    bool          TiXmlSaveDocument(const wxString& filename, TiXmlDocument* doc);
    bool          TiXmlSuccess(TiXmlDocument* doc);

    wxString                mFileSrc;
    TiXmlDocument*          mCfgSrc;
    bool                    mCfgSrcValid;
    std::vector<TiXmlNode*> mNodesSrc;

    wxString                mFileDst;
    TiXmlDocument*          mCfgDst;
    bool                    mCfgDstValid;
    std::vector<TiXmlNode*> mNodesDst;

		DECLARE_EVENT_TABLE()
};

#endif
