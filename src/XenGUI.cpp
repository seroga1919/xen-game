#include "XenGUI.h"
#include "Bot.h"
#include "Logger.h"
#include "IDInputDevice8_Keyboard.h"

wxBEGIN_EVENT_TABLE(XenGUI, wxFrame)
	EVT_TIMER(-1, XenGUI::onTimer)
	EVT_CLOSE(XenGUI::onClose)
wxEND_EVENT_TABLE()


XenGUI::XenGUI(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	:XenGUI_template(parent, id, title, pos, size, style)
{
	bot = Bot::getInstance();
	SETTINGS * bot_settings = bot->settings;
	Logger::setGUIOutPut(text_console);
	
	FinishComponentSetup();
	BindEvents();
	m_timer->Start(300);	// UI refresh loop, with 300ms frequency
}

void XenGUI::FinishComponentSetup() {

	//That button is actually sending packet,
	// so potentially dangerous ban-wise
	button_moveToLast->Hide();

	// Tweaking some appearance settings manually
	text_playerStatus->SetUseHorizontalScrollBar(false);
	text_console->SetUseHorizontalScrollBar(false);
	text_radar->SetUseHorizontalScrollBar(false);
	
	// Loading config in UI
	bot = Bot::getInstance();
	SETTINGS * bot_settings = bot->settings;

	checkbox_walkPath->SetValue(bot_settings->walk_path);
	checkbox_attackEnemies->SetValue(bot_settings->attack_enemies);
	checkbox_collectItems->SetValue(bot_settings->collect_items);
	checkbox_sellToMerchant->SetValue(bot_settings->sell_to_merchant);
	//checkbox_keepConsole->SetValue(bot->settings->keep_console);
	checkbox_keepConsole->SetValue(false);
	checkbox_keepConsole->Enable(false);
	checkbox_filterOutIdleUnits->SetValue(bot->settings->filterOutIdleUnits);
	checkbox_autoLogin->SetValue(bot->settings->auto_log);
	checkbox_resizeWindow->SetValue(bot->settings->resize_window);

	txt_enemyNumberTreshold->ChangeValue(std::to_string(bot_settings->enemiesBeforeAttack));
	txt_aggroDistance->ChangeValue(std::to_string(bot_settings->aggroDistance));
	txt_healUponHP->ChangeValue(std::to_string(bot_settings->healOnHPbellow));
	txt_manaPotUponMP->ChangeValue(std::to_string(bot_settings->usePotOnMPbellow));
	txt_delayBetweenAttacks->ChangeValue(std::to_string(bot_settings->delayBetweenAttacks));

	choice_sellItemAfterSlot->SetSelection(bot_settings->startSellOffAfterSlot);
	
	checkbox_keepAlive->SetValue(bot->settings->keep_alive);

	int choice;
	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->moveSkillKey) == 0 ?
		0 : choice_moveMode->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->moveSkillKey)
		);
	choice_moveMode->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->selfHealKey) == 0 ?
		0 : choice_healKey->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->selfHealKey)
		);
	choice_healKey->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff1) == 0 ?
		0 : choice_buff1->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff1)
		);
	choice_buff1->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff2) == 0 ?
		0 : choice_buff2->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff2)
		);
	choice_buff2->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff3) == 0 ?
		0 : choice_buff3->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff3)
		);
	choice_buff3->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff4) == 0 ?
		0 : choice_buff4->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff4)
		);
	choice_buff4->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff5) == 0 ?
		0 : choice_buff5->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->buff5)
		);
	choice_buff5->SetSelection(choice);

	choice =
		hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->stopActionKey) == 0 ?
		0 : choice_stopActionKey->FindString(
			hkIDirectInput8Device_Keyboard::GetCharFromDIcode(bot_settings->stopActionKey)
		);
	choice_stopActionKey->SetSelection(choice);

	std::string tmp;
	int i = 1;
	for (std::string name : bot->getPathsNames()) {
		tmp = name.substr(0, name.size() - 5); // removes ".path"
		dropdown_pathChoice->Append(tmp);
		if (0 == strcmp(tmp.c_str(),bot->settings->currentPathFile)) {
			dropdown_pathChoice->SetSelection(i);
			
		}
		i++;
	}

	//TODO F1-F12 hotkeys, currently only one choice
}

void XenGUI::BindEvents() {

	// Activity panel
	checkbox_walkPath->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_walkPath, this);
	checkbox_attackEnemies->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_attackEnemies, this);
	checkbox_collectItems->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_collectItems, this);
	checkbox_sellToMerchant->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_sellToMerchant, this);
	checkbox_hideGameWindow->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_hideGameWindow, this);
	button_activateBot->Bind(wxEVT_TOGGLEBUTTON, &XenGUI::onToggled_activate, this);
	dropdown_pathChoice->Bind(wxEVT_CHOICE, &XenGUI::onSelected_pathChoice, this);

	// Settings panel
	txt_enemyNumberTreshold->Bind(wxEVT_TEXT, &XenGUI::onChanged_txt_enemyNumberTreshold, this);
	txt_aggroDistance->Bind(wxEVT_TEXT, &XenGUI::onChanged_txt_aggroDistance, this);
	txt_healUponHP->Bind(wxEVT_TEXT, &XenGUI::onChanged_txt_healUponHP, this);
	txt_manaPotUponMP->Bind(wxEVT_TEXT, &XenGUI::onChanged_txt_manaPotUponMP, this);
	txt_delayBetweenAttacks->Bind(wxEVT_TEXT, &XenGUI::onChanged_txt_delayBetweenAttacks, this);
	choice_sellItemAfterSlot->Bind(wxEVT_CHOICE, &XenGUI::onChanged_sellItemAfterSlot, this);;
	checkbox_keepConsole->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_keepConsole, this);
	checkbox_autoLogin->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_autoLogin, this);
	checkbox_resizeWindow->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_resizeWindow, this);

	choice_moveMode->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_moveMode, this);
	choice_healKey->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_choice_healKey, this);
	choice_buff1->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_buff1, this);
	choice_buff2->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_buff2, this);
	choice_buff3->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_buff3, this);
	choice_buff4->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_buff4, this);
	choice_buff5->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_buff5, this);
	choice_stopActionKey->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_stopAction, this);

	// Path edit panel
	button_startRecording->Bind(wxEVT_TOGGLEBUTTON, &XenGUI::onToggled_startRecording, this);
	button_undoCheckPoint->Bind(wxEVT_BUTTON, &XenGUI::onClicked_undo, this);
	button_moveToLast->Bind(wxEVT_BUTTON, &XenGUI::onClicked_moveBack, this);
	text_merchantNPCid->Bind(wxEVT_TEXT, &XenGUI::onChanged_text_merchantNPCid, this);
	choice_homePortal->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_homePortal, this);;
	choice_huntingGroundPortal->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_huntingGroundPortal, this);
	button_savePath->Bind(wxEVT_BUTTON, &XenGUI::onClicked_savePath, this);
	button_loadPath->Bind(wxEVT_BUTTON, &XenGUI::onClicked_loadPath, this);

	// Radar
	text_radarRange->Bind(wxEVT_TEXT, &XenGUI::onChanged_txt_radarRange, this);
	checkbox_filterOutIdleUnits->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_filterOutIdleUnits, this);

	// Misc
	checkbox_keepAlive->Bind(wxEVT_CHECKBOX, &XenGUI::onClicked_checkbox_keepAlive, this);
	choice_uiToForeground->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_uiToForeground, this);
	choice_stopBotGameHotkey->Bind(wxEVT_CHOICE, &XenGUI::onChanged_choice_stopBotGameHotkey, this);
}

void XenGUI::refreshBotStatus() {
	button_activateBot->SetValue(bot->isRunning());
	m_statusBar->SetLabel(
		(bot->isRunning() ? "Running | " : "Disabled | ")
		+ bot->gamestatus->status
	);
}

void XenGUI::refreshPlayerStatus() {
	std::string status = "";
	status += "==== Character's status ====\n\n";
	status += "Position : <" + std::to_string(bot->getX())
		+ " ; " + std::to_string(bot->getY()) + ">" + "\n";
	status += "Life stats : ";
	status += "\n\t HP : " + std::to_string(bot->getHPpercent());
	status += "\n\t MP : " + std::to_string(bot->getMPpercent());
	status += "\n\t XP : " + std::to_string(bot->getXPpercent());
	text_playerStatus->SetEditable(true);
	text_playerStatus->SetValue(status);
	text_playerStatus->SetEditable(false);
}

void XenGUI::refreshRadar() {
	wxString str = "Units in " + std::to_string(bot->settings->radar_range) + " range :\n";
	for (std::string s : GameWorld::getInstance()
		->units_toStrings(bot->settings->radar_range, bot->settings->filterOutIdleUnits)) {
		str += s;
	}
	text_radar->SetText(str);
}

void XenGUI::refreshPathEdit() {
	list_pathPoints->Set(bot->getCurrentRecordedPath()->to_wxStrings());

	if (!bot->isRecording()) {
		button_startRecording->SetValue(false);
		do_toggleRecording(false);
	}
}

void XenGUI::onClose(wxCloseEvent& event) {
	m_timer->Stop();
	Logger::setGUIOutPut(NULL);
	OnCloseWindow(event);
}

int tempo = 0;
void XenGUI::onTimer(wxTimerEvent& event) {

	if (bot->flags->need_ui_on_front) {
		log(I, "UI requested to pop to the foreground");
		Restore();
		SetForegroundWindow(GetHandle());
		bot->flags->need_ui_on_front = false;
	}

	tempo++;
	refreshBotStatus();
	refreshPlayerStatus();
	refreshRadar();

	if (bot->isRecording())
		refreshPathEdit();


	text_console->SetEditable(true);
	for (std::string s : Logger::getOutputBuff()) {
		text_console->AppendText(s);
	}
	text_console->ScrollToLine(text_console->GetLineCount() - 1);
	text_console->SetEditable(false);

	if (tempo >= 30) {
		tempo = 0;
		bot->saveSettings(true);
	}
}


///////////////////////////////////////////////////////////////////////////
//					Actions on components								 //
///////////////////////////////////////////////////////////////////////////

void XenGUI::onClicked_checkbox_keepAlive(wxCommandEvent & ev) {
	bot->settings->keep_alive = checkbox_keepAlive->GetValue();
}

void XenGUI::onChanged_choice_uiToForeground(wxCommandEvent & ev) {
	wxString s = choice_uiToForeground->GetString(choice_uiToForeground->GetSelection());
	if (0 == s.compare("F5")) {
		bot->settings->toForegroundKey = DIK_F5;
	}
}

void XenGUI::onChanged_choice_stopBotGameHotkey(wxCommandEvent & ev) {
	wxString s = choice_stopBotGameHotkey->GetString(choice_stopBotGameHotkey->GetSelection());
	if (0 == s.compare("F6")) {
		bot->settings->stopBotKey = DIK_F6;
	}
}

void XenGUI::onChanged_choice_stopAction(wxCommandEvent & ev) {
	wxString s = choice_stopActionKey->GetString(choice_stopActionKey->GetSelection());
	if (!s.empty()) {
		bot->settings->stopActionKey = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_moveMode(wxCommandEvent & ev) {
	wxString s = choice_moveMode->GetString(choice_moveMode->GetSelection());
	if (!s.empty()) {
		bot->settings->moveSkillKey = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_choice_healKey(wxCommandEvent & ev) {
	wxString s = choice_healKey->GetString(choice_healKey->GetSelection());
	if (!s.empty()) {
		bot->settings->selfHealKey = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_buff1(wxCommandEvent & ev) {
	wxString s = choice_buff1->GetString(choice_buff1->GetSelection());
	if (!s.empty()) {
		bot->settings->buff1 = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_buff2(wxCommandEvent & ev) {
	wxString s = choice_buff2->GetString(choice_buff2->GetSelection());
	if (!s.empty()) {
		bot->settings->buff2 = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_buff3(wxCommandEvent & ev) {
	wxString s = choice_buff3->GetString(choice_buff3->GetSelection());
	if (!s.empty()) {
		bot->settings->buff3 = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_buff4(wxCommandEvent & ev) {
	wxString s = choice_buff4->GetString(choice_buff4->GetSelection());
	if (!s.empty()) {
		bot->settings->buff4 = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onChanged_choice_buff5(wxCommandEvent & ev) {
	wxString s = choice_buff5->GetString(choice_buff5->GetSelection());
	if (!s.empty()) {
		bot->settings->buff5 = hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(s[0]);
	}
}

void XenGUI::onClicked_checkbox_autoLogin(wxCommandEvent & ev) {
	bot->settings->auto_log  = checkbox_autoLogin->GetValue();
}

void XenGUI::onClicked_checkbox_filterOutIdleUnits(wxCommandEvent & ev) {
	bot->settings->filterOutIdleUnits = checkbox_filterOutIdleUnits->GetValue();
}

void XenGUI::onClicked_checkbox_keepConsole(wxCommandEvent & ev) {
	bot->settings->keep_console = checkbox_keepConsole->GetValue();
}

void XenGUI::onClicked_checkbox_resizeWindow(wxCommandEvent & ev) {
	bot->settings->resize_window = checkbox_resizeWindow->GetValue();
}

void XenGUI::onChanged_txt_radarRange(wxCommandEvent & ev) {
	wxString str = text_radarRange->GetValue();
	int pos = text_radarRange->GetInsertionPoint();
	long val = 1;

	if (!str.IsEmpty() && !str.ToLong(&val)) { // error
		text_radarRange->SetValue(cleanNumericInput(str.ToStdString()));
		text_radarRange->SetInsertionPoint(pos - 1);
	}
	else if (val < 1 || 0xFFFF < val) {
		text_radarRange->SetValue(std::to_string(1));
	}
	else {
		bot->settings->radar_range = val;
	}
}

void XenGUI::onSelected_pathChoice(wxCommandEvent & ev) {
	int index = dropdown_pathChoice->GetSelection();
	bot->stop();
	if (index != 0) {
		wxString file = dropdown_pathChoice->GetString(dropdown_pathChoice->GetSelection());
		strcpy(bot->settings->currentPathFile, file.mb_str().data());
		file = "XenGrinder\\paths\\" + file + ".path";
		bot->getCurrentPath()->loadFromFile((char*)file.mb_str().data());
		std::string msg = "Loaded new path with ";
		msg += std::to_string(bot->getCurrentPath()->size()) + " check points";
		log(I, msg);
	}
}

void XenGUI::onClicked_checkbox_walkPath(wxCommandEvent & ev) {
	bot->settings->walk_path = checkbox_walkPath->GetValue();
	if (bot->settings->walk_path)
		log(I, "Walk path activated");
	else
		log(I, "Walk path deactivated");

	bot->settings->attack_enemies = checkbox_walkPath->GetValue();
	checkbox_attackEnemies->SetValue(bot->settings->attack_enemies);
}

void XenGUI::onClicked_checkbox_attackEnemies(wxCommandEvent & ev) {
	bot->settings->attack_enemies = checkbox_attackEnemies->GetValue();
	if (bot->settings->attack_enemies)
		log(I, "Attack activated");
	else
		log(I, "Attack deactivated");
}

void XenGUI::onClicked_checkbox_collectItems(wxCommandEvent & ev) {
	bot->settings->collect_items = checkbox_collectItems->GetValue();
	if (bot->settings->collect_items)
		log(I, "Item loot activated");
	else
		log(I, "Item loot deactivated");
}

void XenGUI::onClicked_checkbox_sellToMerchant(wxCommandEvent & ev) {
	bot->settings->sell_to_merchant = checkbox_sellToMerchant->GetValue();
	if (bot->settings->sell_to_merchant)
		log(I, "Item sell off activated");
	else
		log(I, "Item sell off deactivated");
}

void XenGUI::onClicked_checkbox_hideGameWindow(wxCommandEvent & ev) {
	bot->hideGameWindow(checkbox_hideGameWindow->GetValue());
}

void XenGUI::onToggled_activate(wxCommandEvent & ev) {

	if (button_activateBot->GetValue()) {
		bot->start();
		m_statusBar->SetLabel("Running");
	}
	else {
		bot->stop();
		m_statusBar->SetLabel("Disabled");
	}	
}

void XenGUI::onClicked_savePath(wxCommandEvent & ev) {
	wxString file = text_pathName->GetValue();
	if (!file.IsEmpty()) {
		file = "XenGrinder\\paths\\" + file + ".path";
		bot->getCurrentRecordedPath()->setFollowUpPathName(txt_followUpPathName->GetValue().mb_str().data());
		bot->getCurrentRecordedPath()->saveToFile((char*)file.mb_str().data());
		dropdown_pathChoice->Append(text_pathName->GetValue());
	}
}

void XenGUI::onClicked_loadPath(wxCommandEvent & ev)
{
	wxString file = text_pathName->GetValue();
	if (!file.IsEmpty()) {
		file = "XenGrinder\\paths\\" + file + ".path";
		bot->getCurrentRecordedPath()->loadFromFile((char*)file.mb_str().data());

		text_merchantNPCid->SetValue(std::to_string(bot->getCurrentRecordedPath()->getMerchantModelId()));
		choice_homePortal->SetSelection(bot->getCurrentRecordedPath()->getHomePortal());
		choice_huntingGroundPortal->SetSelection(bot->getCurrentRecordedPath()->getHuntingGroundPortal());
		txt_followUpPathName->SetValue(bot->getCurrentRecordedPath()->getFollowUpPathName().c_str());
		refreshPathEdit();
	}
}

void XenGUI::onChanged_choice_homePortal(wxCommandEvent & ev) {
	bot->getCurrentRecordedPath()
		->setHomePortal(choice_homePortal->GetSelection());
}

void XenGUI::onChanged_choice_huntingGroundPortal(wxCommandEvent & ev) {
	bot->getCurrentRecordedPath()
		->setHuntingGroundPortal(choice_huntingGroundPortal->GetSelection());
}

void XenGUI::onClicked_undo(wxCommandEvent & ev) {
	bot->getCurrentRecordedPath()->pop_back();
	refreshPathEdit();
}

void XenGUI::onClicked_moveBack(wxCommandEvent & ev) {
	if (!bot->getCurrentRecordedPath()->empty()) {
		CPacket_Move p = bot->getCurrentRecordedPath()->back();
		//bot->sendMoveOrder(p.x, p.y);		// Disabled
	}
}

void XenGUI::onChanged_text_merchantNPCid(wxCommandEvent & ev) {

	wxString str = wxString();
	str += cleanNumericInput(text_merchantNPCid->GetValue());
	
	text_merchantNPCid->ChangeValue(str);
	bot->getCurrentRecordedPath()->setMerchantModelId(std::atoi(str.c_str()));
}

void XenGUI::onToggled_startRecording(wxCommandEvent & ev) {
	do_toggleRecording(button_startRecording->GetValue());
}

void XenGUI::do_toggleRecording(bool toggled) {
	if (toggled) {
		m_statusBar->SetStatusText("Recording ...");
		button_startRecording->SetLabelText("Stop recording");
		if (!bot->stop()) {
			return;
		}
		button_activateBot->SetValue(false);
	}
	else {
		m_statusBar->SetStatusText("");
		button_startRecording->SetLabelText("Start recording");
	}
	button_moveToLast->Enable(!toggled);
	button_undoCheckPoint->Enable(!toggled);
	text_merchantNPCid->Enable(!toggled);
	choice_homePortal->Enable(!toggled);
	choice_huntingGroundPortal->Enable(!toggled);
	text_pathName->Enable(!toggled);
	button_savePath->Enable(!toggled);
	button_loadPath->Enable(!toggled);

	bot->record(toggled);
}

void XenGUI::onChanged_txt_enemyNumberTreshold(wxCommandEvent & ev) {
	wxString str = txt_enemyNumberTreshold->GetValue();
	int pos = txt_enemyNumberTreshold->GetInsertionPoint();
	long val = 1;

	if (!str.IsEmpty() && !str.ToLong(&val)) { // error
		txt_enemyNumberTreshold->SetValue(cleanNumericInput(str.ToStdString()));
		txt_enemyNumberTreshold->SetInsertionPoint(pos - 1);
	}
	else if (val < 1 || 0xFFFF < val) {
		txt_enemyNumberTreshold->SetValue(std::to_string(1));
	}
	else {
		bot->settings->enemiesBeforeAttack = val;
	}
}

void XenGUI::onChanged_txt_aggroDistance(wxCommandEvent & ev) {
	wxString str = txt_aggroDistance->GetValue();
	int pos = txt_aggroDistance->GetInsertionPoint();
	long val = 1;

	if (!str.IsEmpty() && !str.ToLong(&val)) { // error
		txt_aggroDistance->SetValue(cleanNumericInput(str.ToStdString()));
		txt_aggroDistance->SetInsertionPoint(pos - 1);
	}
	else if (val < 1 || 0xFFFF < val) {
		txt_aggroDistance->SetValue(std::to_string(1));
	}
	else {
		bot->settings->aggroDistance = val;
	}
}

void XenGUI::onChanged_txt_healUponHP(wxCommandEvent & ev) {
	wxString str = txt_healUponHP->GetValue();
	int pos = txt_healUponHP->GetInsertionPoint();
	long val = 1;

	if (!str.IsEmpty() && !str.ToLong(&val)) { // error
		txt_healUponHP->SetValue(cleanNumericInput(str.ToStdString()));
		txt_healUponHP->SetInsertionPoint(pos - 1);
	}
	else if (val < 0 || 100 < val) {
		txt_healUponHP->SetValue(std::to_string(0));
	}
	else {
		bot->settings->healOnHPbellow = val;
	}
}

void XenGUI::onChanged_txt_manaPotUponMP(wxCommandEvent & ev) {
	wxString str = txt_manaPotUponMP->GetValue();
	int pos = txt_manaPotUponMP->GetInsertionPoint();
	long val = 1;

	if (!str.IsEmpty() && !str.ToLong(&val)) { // error
		txt_manaPotUponMP->SetValue(cleanNumericInput(str.ToStdString()));
		txt_manaPotUponMP->SetInsertionPoint(pos - 1);
	}
	else if (val < 0 || 100 < val) {
		txt_manaPotUponMP->SetValue(std::to_string(0));
	}
	else {
		bot->settings->usePotOnMPbellow = val;
	}
}

void XenGUI::onChanged_txt_delayBetweenAttacks(wxCommandEvent & ev) {
	wxString str = txt_delayBetweenAttacks->GetValue();
	int pos = txt_delayBetweenAttacks->GetInsertionPoint();
	long val = 1;

	if (!str.IsEmpty() && !str.ToLong(&val)) { // error
		txt_delayBetweenAttacks->SetValue(cleanNumericInput(str.ToStdString()));
		txt_delayBetweenAttacks->SetInsertionPoint(pos - 1);
	}
	else if (val < 1 || 10000 < val) {
		txt_delayBetweenAttacks->SetValue(std::to_string(100));
	}
	else {
		bot->settings->delayBetweenAttacks = val;
	}
}

void XenGUI::onChanged_sellItemAfterSlot(wxCommandEvent & ev) {
	bot->settings->startSellOffAfterSlot =
		choice_sellItemAfterSlot->GetSelection();
}

///////////////////////////////////////////////////////////////////////////////
std::string XenGUI::cleanNumericInput(std::string input, bool isFloat) {

	std::string res;

	for (int i = 0; i < (int)input.size(); i++) {
		if (('0' <= input[i] && input[i] <= '9') || (isFloat && input[i] == '.')) {
			res.push_back(input[i]);
		}
	}
	return res;

}

wxString XenGUI::cleanNumericInput(wxString input, bool isFloat) {

	wxString res;

	for (int i = 0; i < (int)input.size(); i++) {
		if (('0' <= input[i] && input[i] <= '9') || (isFloat && input[i] == '.')) {
			res+=(input[i]);
		}
	}
	return res;

}