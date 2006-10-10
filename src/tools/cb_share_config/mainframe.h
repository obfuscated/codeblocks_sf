#ifndef MAINFRAME_H
#define MAINFRAME_H


//(*Headers(MainFrame)
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/arrstr.h>
#include <vector>

#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

class MainFrame: public wxFrame
{
//***********************************************************************
	public:
//***********************************************************************

		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();


		//(*Identifiers(MainFrame)
		enum Identifiers
		{
		  ID_TXT_STEPS = 0x1000,
		  ID_LBL_FILE_SRC,
		  ID_LBL_FILE_DST,
		  ID_TXT_FILE_SRC,
		  ID_BTN_FILE_SRC,
		  ID_TXT_FILE_DST,
		  ID_BTN_FILE_DST,
		  ID_CFG_SRC,
		  ID_LST_CFG,
		  ID_BTN_TRANSFER,
		  ID_BTN_SAVE,
		  ID_BTN_CLOSE
		};
		//*)

//***********************************************************************
	protected:
//***********************************************************************

		//(*Handlers(MainFrame)
		void OnBtnFileSrcClick(wxCommandEvent& event);
		void OnBtnFileDstClick(wxCommandEvent& event);
		void OnBtnCloseClick(wxCommandEvent& event);
		void OnBtnTransferClick(wxCommandEvent& event);
		void OnBtnSaveClick(wxCommandEvent& event);
		//*)


		//(*Declarations(MainFrame)
		wxBoxSizer* bszMain;
		wxBoxSizer* bszSteps;
		wxStaticText* txtSteps;
		wxGridSizer* grsFileLabel;
		wxStaticText* lblFileSrc;
		wxStaticText* lblFileDst;
		wxGridSizer* grsFile;
		wxFlexGridSizer* flsFileSrc;
		wxTextCtrl* txtFileSrc;
		wxButton* btnFileSrc;
		wxFlexGridSizer* flsFileDst;
		wxTextCtrl* txtFileDst;
		wxButton* btnFileDst;
		wxGridSizer* grsCfg;
		wxCheckListBox* clbCfgSrc;
		wxListBox* lstCfgDst;
		wxGridSizer* grsAction;
		wxButton* btnTransfer;
		wxButton* btnSave;
		wxButton* btnClose;
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
    wxArrayString PathToArray (const wxString& path);

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
