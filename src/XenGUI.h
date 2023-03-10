#pragma once
#include "XenGUI_template.h"
#include "Bot.h"

// XenGUI_template.h/cpp are the files generated by the design tool
// wxFormBuilder. They shouldn't be edited manually, as they'll be overwritten
// by any UI modification. Instead, write custom behaviour in XenGUI.h/cpp

class XenGUI : public XenGUI_template
{

public:
	XenGUI(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Xenimus Grinder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 700), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	void FinishComponentSetup();
	void BindEvents();
	
private:
	wxTimer * m_timer = new wxTimer(this);
	Bot * bot;

	static std::string cleanNumericInput(std::string input, bool isFloat=false);
	static wxString cleanNumericInput(wxString input, bool isFloat = false);
	void refreshBotStatus();
	void refreshPlayerStatus();
	void refreshRadar();
	void refreshPathEdit();
	
	void onClose(wxCloseEvent& event);
	void onTimer(wxTimerEvent& event);
	void onChanged_txt_enemyNumberTreshold(wxCommandEvent & ev);
	void onChanged_txt_aggroDistance(wxCommandEvent & ev);
	void onChanged_txt_healUponHP(wxCommandEvent & ev);
	void onChanged_txt_manaPotUponMP(wxCommandEvent & ev);
	void onChanged_txt_delayBetweenAttacks(wxCommandEvent & ev);
	void onChanged_sellItemAfterSlot(wxCommandEvent & ev);
	void onClicked_checkbox_keepConsole(wxCommandEvent & ev);
	void onClicked_checkbox_resizeWindow(wxCommandEvent & ev);
	
	void onToggled_startRecording(wxCommandEvent & ev);
	void onToggled_activate(wxCommandEvent & ev);
	void onClicked_checkbox_walkPath(wxCommandEvent & ev);
	void onClicked_checkbox_attackEnemies(wxCommandEvent & ev);
	void onClicked_checkbox_collectItems(wxCommandEvent & ev);
	void onClicked_checkbox_sellToMerchant(wxCommandEvent & ev);
	void onClicked_checkbox_hideGameWindow(wxCommandEvent & ev);


	void onClicked_undo(wxCommandEvent & ev);
	void onClicked_moveBack(wxCommandEvent & ev);
	void onChanged_text_merchantNPCid(wxCommandEvent & ev);
	void onChanged_choice_homePortal(wxCommandEvent & ev);
	void onChanged_choice_huntingGroundPortal(wxCommandEvent & ev);
	void onClicked_savePath(wxCommandEvent & ev);
	void onClicked_loadPath(wxCommandEvent & ev);
	void onSelected_pathChoice(wxCommandEvent & ev);

	void onChanged_choice_moveMode(wxCommandEvent & ev);
	void onChanged_choice_choice_healKey(wxCommandEvent & ev);
	void onChanged_choice_buff1(wxCommandEvent & ev);
	void onChanged_choice_buff2(wxCommandEvent & ev);
	void onChanged_choice_buff3(wxCommandEvent & ev);
	void onChanged_choice_buff4(wxCommandEvent & ev);
	void onChanged_choice_buff5(wxCommandEvent & ev);
	void onChanged_choice_stopAction(wxCommandEvent & ev);
	
	void onChanged_choice_uiToForeground(wxCommandEvent & ev);
	void onChanged_choice_stopBotGameHotkey(wxCommandEvent & ev);


	void onChanged_txt_radarRange(wxCommandEvent & ev);
	void onClicked_checkbox_filterOutIdleUnits(wxCommandEvent & ev);
	void onClicked_checkbox_autoLogin(wxCommandEvent & ev);
	void onClicked_checkbox_keepAlive(wxCommandEvent & ev);

	void do_toggleRecording(bool toggled);

	wxDECLARE_EVENT_TABLE();
};
