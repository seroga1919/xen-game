///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 29 2019)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/statline.h>
#include <wx/stc/stc.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/aui/auibook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class XenGUI_template
///////////////////////////////////////////////////////////////////////////////
class XenGUI_template : public wxFrame
{
	private:

	protected:
		wxAuiNotebook* m_auinotebook3;
		wxPanel* panel_activity;
		wxCheckBox* checkbox_walkPath;
		wxCheckBox* checkbox_attackEnemies;
		wxCheckBox* checkbox_collectItems;
		wxCheckBox* checkbox_sellToMerchant;
		wxStaticText* label_pickPathFile;
		wxChoice* dropdown_pathChoice;
		wxCheckBox* checkbox_hideGameWindow;
		wxPanel* m_panel51;
		wxToggleButton* button_activateBot;
		wxStaticLine* m_staticline2;
		wxStyledTextCtrl* text_playerStatus;
		wxStyledTextCtrl* text_console;
		wxPanel* panel_settings;
		wxStaticText* m_staticText1;
		wxTextCtrl* txt_enemyNumberTreshold;
		wxStaticText* m_staticText2;
		wxTextCtrl* txt_aggroDistance;
		wxStaticText* m_staticText3;
		wxChoice* m_choice1;
		wxStaticText* m_staticText6;
		wxTextCtrl* txt_healUponHP;
		wxStaticText* m_staticText7;
		wxTextCtrl* txt_manaPotUponMP;
		wxStaticText* m_staticText4;
		wxChoice* choice_sellItemAfterSlot;
		wxStaticLine* m_staticline1;
		wxStaticText* label_moveMode;
		wxChoice* choice_moveMode;
		wxStaticText* label_targeting_mode;
		wxChoice* choice_targetingMode;
		wxStaticText* m_staticText5;
		wxTextCtrl* txt_delayBetweenAttacks;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText16;
		wxChoice* choice_attackKey;
		wxStaticText* m_staticText241;
		wxChoice* choice_healKey;
		wxStaticText* m_staticText19;
		wxChoice* choice_usePotionKey;
		wxStaticText* m_staticText18;
		wxChoice* choice_usePortalKey;
		wxStaticText* m_staticText20;
		wxChoice* choice_buff1;
		wxStaticText* m_staticText21;
		wxChoice* choice_buff2;
		wxStaticText* m_staticText22;
		wxChoice* choice_buff3;
		wxStaticText* m_staticText23;
		wxChoice* choice_buff4;
		wxStaticText* m_staticText24;
		wxChoice* choice_buff5;
		wxStaticText* m_staticText26;
		wxChoice* choice_stopActionKey;
		wxStaticLine* m_staticline3;
		wxPanel* panel_pathEdit;
		wxToggleButton* button_startRecording;
		wxButton* button_moveToLast;
		wxButton* button_undoCheckPoint;
		wxCheckListBox* list_pathPoints;
		wxStaticText* label_merchantId;
		wxTextCtrl* text_merchantNPCid;
		wxStaticText* label_homePortal;
		wxChoice* choice_homePortal;
		wxStaticText* label_huntingGroundPortal;
		wxChoice* choice_huntingGroundPortal;
		wxStaticText* m_staticText25;
		wxTextCtrl* txt_followUpPathName;
		wxStaticText* label_pathName;
		wxTextCtrl* text_pathName;
		wxButton* button_savePath;
		wxButton* button_loadPath;
		wxPanel* panel_radar;
		wxStyledTextCtrl* text_radar;
		wxStaticText* label_radarRange;
		wxTextCtrl* text_radarRange;
		wxPanel* m_panel6;
		wxCheckBox* checkbox_filterOutIdleUnits;
		wxPanel* panel_misc;
		wxCheckBox* checkbox_keepConsole;
		wxCheckBox* checkbox_autoLogin;
		wxCheckBox* checkbox_resizeWindow;
		wxCheckBox* checkbox_keepAlive;
		wxStaticText* m_staticText27;
		wxChoice* choice_uiToForeground;
		wxStaticText* m_staticText29;
		wxChoice* choice_stopBotGameHotkey;
		wxStatusBar* m_statusBar;

	public:

		XenGUI_template( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Xenimus Grinder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,700 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~XenGUI_template();

};

