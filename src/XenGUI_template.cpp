///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 29 2019)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "XenGUI_template.h"

///////////////////////////////////////////////////////////////////////////

XenGUI_template::XenGUI_template( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_auinotebook3 = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	panel_activity = new wxPanel( m_auinotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("Activity log") );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 0, 2, 0, 0 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );

	checkbox_walkPath = new wxCheckBox( panel_activity, wxID_ANY, wxT("Walk path"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( checkbox_walkPath, 0, wxALL, 5 );

	checkbox_attackEnemies = new wxCheckBox( panel_activity, wxID_ANY, wxT("Attack enemies"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( checkbox_attackEnemies, 0, wxALL, 5 );

	checkbox_collectItems = new wxCheckBox( panel_activity, wxID_ANY, wxT("Collect items"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( checkbox_collectItems, 0, wxALL, 5 );

	checkbox_sellToMerchant = new wxCheckBox( panel_activity, wxID_ANY, wxT("Sell to merchant"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( checkbox_sellToMerchant, 0, wxALL, 5 );


	gSizer2->Add( bSizer9, 1, wxEXPAND, 5 );

	wxGridSizer* gSizer7;
	gSizer7 = new wxGridSizer( 0, 2, 0, 0 );

	label_pickPathFile = new wxStaticText( panel_activity, wxID_ANY, wxT("Select a path preset:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_pickPathFile->Wrap( -1 );
	gSizer7->Add( label_pickPathFile, 0, wxALL, 5 );

	wxString dropdown_pathChoiceChoices[] = { wxT("--- Select file ---") };
	int dropdown_pathChoiceNChoices = sizeof( dropdown_pathChoiceChoices ) / sizeof( wxString );
	dropdown_pathChoice = new wxChoice( panel_activity, wxID_ANY, wxDefaultPosition, wxDefaultSize, dropdown_pathChoiceNChoices, dropdown_pathChoiceChoices, 0 );
	dropdown_pathChoice->SetSelection( 0 );
	gSizer7->Add( dropdown_pathChoice, 0, wxALL, 5 );

	checkbox_hideGameWindow = new wxCheckBox( panel_activity, wxID_ANY, wxT("Hide game window"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer7->Add( checkbox_hideGameWindow, 0, wxALL, 5 );

	m_panel51 = new wxPanel( panel_activity, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	gSizer7->Add( m_panel51, 1, wxEXPAND | wxALL, 5 );

	button_activateBot = new wxToggleButton( panel_activity, wxID_ANY, wxT("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	button_activateBot->SetValue( true );
	gSizer7->Add( button_activateBot, 0, wxALL, 5 );


	gSizer2->Add( gSizer7, 1, wxEXPAND, 5 );


	bSizer6->Add( gSizer2, 0, wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( panel_activity, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer6->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	text_playerStatus = new wxStyledTextCtrl( panel_activity, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	text_playerStatus->SetUseTabs( true );
	text_playerStatus->SetTabWidth( 4 );
	text_playerStatus->SetIndent( 4 );
	text_playerStatus->SetTabIndents( true );
	text_playerStatus->SetBackSpaceUnIndents( true );
	text_playerStatus->SetViewEOL( false );
	text_playerStatus->SetViewWhiteSpace( false );
	text_playerStatus->SetMarginWidth( 2, 0 );
	text_playerStatus->SetIndentationGuides( true );
	text_playerStatus->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	text_playerStatus->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	text_playerStatus->SetMarginWidth( 1, 16);
	text_playerStatus->SetMarginSensitive( 1, true );
	text_playerStatus->SetProperty( wxT("fold"), wxT("1") );
	text_playerStatus->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	text_playerStatus->SetMarginWidth( 0, 0 );
	{
		wxFont font = wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas") );
		text_playerStatus->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
	}
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	text_playerStatus->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	text_playerStatus->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	text_playerStatus->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	text_playerStatus->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	text_playerStatus->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	text_playerStatus->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	text_playerStatus->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	text_playerStatus->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	text_playerStatus->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	text_playerStatus->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	text_playerStatus->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	text_playerStatus->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas") ) );

	bSizer8->Add( text_playerStatus, 1, wxALL|wxEXPAND, 5 );


	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );

	text_console = new wxStyledTextCtrl( panel_activity, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	text_console->SetUseTabs( true );
	text_console->SetTabWidth( 4 );
	text_console->SetIndent( 4 );
	text_console->SetTabIndents( true );
	text_console->SetBackSpaceUnIndents( true );
	text_console->SetViewEOL( false );
	text_console->SetViewWhiteSpace( false );
	text_console->SetMarginWidth( 2, 0 );
	text_console->SetIndentationGuides( true );
	text_console->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	text_console->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	text_console->SetMarginWidth( 1, 16);
	text_console->SetMarginSensitive( 1, true );
	text_console->SetProperty( wxT("fold"), wxT("1") );
	text_console->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	text_console->SetMarginWidth( 0, 0 );
	{
		wxFont font = wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas") );
		text_console->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
	}
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	text_console->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	text_console->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	text_console->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	text_console->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	text_console->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	text_console->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	text_console->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	text_console->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	text_console->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	text_console->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	text_console->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	text_console->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas") ) );

	bSizer81->Add( text_console, 1, wxALL|wxEXPAND, 5 );


	bSizer6->Add( bSizer81, 1, wxEXPAND, 5 );


	panel_activity->SetSizer( bSizer6 );
	panel_activity->Layout();
	bSizer6->Fit( panel_activity );
	m_auinotebook3->AddPage( panel_activity, wxT("Activity"), true, wxNullBitmap );
	panel_settings = new wxPanel( m_auinotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText1 = new wxStaticText( panel_settings, wxID_ANY, wxT("Number of enemies before attacking"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	gSizer3->Add( m_staticText1, 0, wxALL, 5 );

	txt_enemyNumberTreshold = new wxTextCtrl( panel_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( txt_enemyNumberTreshold, 0, wxALL, 5 );

	m_staticText2 = new wxStaticText( panel_settings, wxID_ANY, wxT("Aggro distance"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	gSizer3->Add( m_staticText2, 0, wxALL, 5 );

	txt_aggroDistance = new wxTextCtrl( panel_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( txt_aggroDistance, 0, wxALL, 5 );

	m_staticText3 = new wxStaticText( panel_settings, wxID_ANY, wxT("Fight script"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->Enable( false );

	gSizer3->Add( m_staticText3, 0, wxALL, 5 );

	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	m_choice1->Enable( false );

	gSizer3->Add( m_choice1, 0, wxALL, 5 );

	m_staticText6 = new wxStaticText( panel_settings, wxID_ANY, wxT("Heal upon HP % bellow"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	gSizer3->Add( m_staticText6, 0, wxALL, 5 );

	txt_healUponHP = new wxTextCtrl( panel_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( txt_healUponHP, 0, wxALL, 5 );

	m_staticText7 = new wxStaticText( panel_settings, wxID_ANY, wxT("Use mana potion upon MP % bellow"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	gSizer3->Add( m_staticText7, 0, wxALL, 5 );

	txt_manaPotUponMP = new wxTextCtrl( panel_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( txt_manaPotUponMP, 0, wxALL, 5 );


	bSizer4->Add( gSizer3, 0, wxEXPAND, 5 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText4 = new wxStaticText( panel_settings, wxID_ANY, wxT("Start inventory sell-off after item number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	gSizer4->Add( m_staticText4, 0, wxALL, 5 );

	wxString choice_sellItemAfterSlotChoices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10"), wxT("11"), wxT("12"), wxT("13"), wxT("14"), wxT("15"), wxT("16"), wxT("17"), wxT("18") };
	int choice_sellItemAfterSlotNChoices = sizeof( choice_sellItemAfterSlotChoices ) / sizeof( wxString );
	choice_sellItemAfterSlot = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_sellItemAfterSlotNChoices, choice_sellItemAfterSlotChoices, 0 );
	choice_sellItemAfterSlot->SetSelection( 0 );
	gSizer4->Add( choice_sellItemAfterSlot, 0, wxALL, 5 );


	bSizer4->Add( gSizer4, 0, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer11;
	gSizer11 = new wxGridSizer( 0, 2, 0, 0 );

	label_moveMode = new wxStaticText( panel_settings, wxID_ANY, wxT("Movement skill"), wxDefaultPosition, wxDefaultSize, 0 );
	label_moveMode->Wrap( -1 );
	gSizer11->Add( label_moveMode, 0, wxALL, 5 );

	wxString choice_moveModeChoices[] = { wxT("(Walk)"), wxT("4") };
	int choice_moveModeNChoices = sizeof( choice_moveModeChoices ) / sizeof( wxString );
	choice_moveMode = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_moveModeNChoices, choice_moveModeChoices, 0 );
	choice_moveMode->SetSelection( 0 );
	gSizer11->Add( choice_moveMode, 0, wxALL, 5 );

	label_targeting_mode = new wxStaticText( panel_settings, wxID_ANY, wxT("Targeting mode"), wxDefaultPosition, wxDefaultSize, 0 );
	label_targeting_mode->Wrap( -1 );
	gSizer11->Add( label_targeting_mode, 0, wxALL, 5 );

	wxString choice_targetingModeChoices[] = { wxT("AOE"), wxT("Click closest") };
	int choice_targetingModeNChoices = sizeof( choice_targetingModeChoices ) / sizeof( wxString );
	choice_targetingMode = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_targetingModeNChoices, choice_targetingModeChoices, 0 );
	choice_targetingMode->SetSelection( 0 );
	choice_targetingMode->Enable( false );

	gSizer11->Add( choice_targetingMode, 0, wxALL, 5 );

	m_staticText5 = new wxStaticText( panel_settings, wxID_ANY, wxT("Delay between attacks (ms)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	gSizer11->Add( m_staticText5, 0, wxALL, 5 );

	txt_delayBetweenAttacks = new wxTextCtrl( panel_settings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer11->Add( txt_delayBetweenAttacks, 0, wxALL, 5 );


	bSizer4->Add( gSizer11, 0, wxEXPAND, 5 );

	m_staticline5 = new wxStaticLine( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer14;
	gSizer14 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText16 = new wxStaticText( panel_settings, wxID_ANY, wxT("Attack key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	gSizer14->Add( m_staticText16, 0, wxALL, 5 );

	wxString choice_attackKeyChoices[] = { wxT("1") };
	int choice_attackKeyNChoices = sizeof( choice_attackKeyChoices ) / sizeof( wxString );
	choice_attackKey = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_attackKeyNChoices, choice_attackKeyChoices, 0 );
	choice_attackKey->SetSelection( 0 );
	gSizer14->Add( choice_attackKey, 0, wxALL, 5 );

	m_staticText241 = new wxStaticText( panel_settings, wxID_ANY, wxT("Self heal key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText241->Wrap( -1 );
	gSizer14->Add( m_staticText241, 0, wxALL, 5 );

	wxString choice_healKeyChoices[] = { wxT("--- Unused ---"), wxT("2") };
	int choice_healKeyNChoices = sizeof( choice_healKeyChoices ) / sizeof( wxString );
	choice_healKey = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_healKeyNChoices, choice_healKeyChoices, 0 );
	choice_healKey->SetSelection( 0 );
	gSizer14->Add( choice_healKey, 0, wxALL, 5 );

	m_staticText19 = new wxStaticText( panel_settings, wxID_ANY, wxT("Use potion key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	gSizer14->Add( m_staticText19, 0, wxALL, 5 );

	wxString choice_usePotionKeyChoices[] = { wxT("3") };
	int choice_usePotionKeyNChoices = sizeof( choice_usePotionKeyChoices ) / sizeof( wxString );
	choice_usePotionKey = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_usePotionKeyNChoices, choice_usePotionKeyChoices, 0 );
	choice_usePotionKey->SetSelection( 0 );
	gSizer14->Add( choice_usePotionKey, 0, wxALL, 5 );

	m_staticText18 = new wxStaticText( panel_settings, wxID_ANY, wxT("Use portal key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	gSizer14->Add( m_staticText18, 0, wxALL, 5 );

	wxString choice_usePortalKeyChoices[] = { wxT("5") };
	int choice_usePortalKeyNChoices = sizeof( choice_usePortalKeyChoices ) / sizeof( wxString );
	choice_usePortalKey = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_usePortalKeyNChoices, choice_usePortalKeyChoices, 0 );
	choice_usePortalKey->SetSelection( 0 );
	gSizer14->Add( choice_usePortalKey, 0, wxALL, 5 );

	m_staticText20 = new wxStaticText( panel_settings, wxID_ANY, wxT("Buff 1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	gSizer14->Add( m_staticText20, 0, wxALL, 5 );

	wxString choice_buff1Choices[] = { wxT("--- Unused ---"), wxT("6") };
	int choice_buff1NChoices = sizeof( choice_buff1Choices ) / sizeof( wxString );
	choice_buff1 = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_buff1NChoices, choice_buff1Choices, 0 );
	choice_buff1->SetSelection( 0 );
	gSizer14->Add( choice_buff1, 0, wxALL, 5 );

	m_staticText21 = new wxStaticText( panel_settings, wxID_ANY, wxT("Buff 2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	gSizer14->Add( m_staticText21, 0, wxALL, 5 );

	wxString choice_buff2Choices[] = { wxT("--- Unused ---"), wxT("7") };
	int choice_buff2NChoices = sizeof( choice_buff2Choices ) / sizeof( wxString );
	choice_buff2 = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_buff2NChoices, choice_buff2Choices, 0 );
	choice_buff2->SetSelection( 0 );
	gSizer14->Add( choice_buff2, 0, wxALL, 5 );

	m_staticText22 = new wxStaticText( panel_settings, wxID_ANY, wxT("Buff 3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	gSizer14->Add( m_staticText22, 0, wxALL, 5 );

	wxString choice_buff3Choices[] = { wxT("--- Unused ---"), wxT("8") };
	int choice_buff3NChoices = sizeof( choice_buff3Choices ) / sizeof( wxString );
	choice_buff3 = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_buff3NChoices, choice_buff3Choices, 0 );
	choice_buff3->SetSelection( 0 );
	gSizer14->Add( choice_buff3, 0, wxALL, 5 );

	m_staticText23 = new wxStaticText( panel_settings, wxID_ANY, wxT("Buff 4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	gSizer14->Add( m_staticText23, 0, wxALL, 5 );

	wxString choice_buff4Choices[] = { wxT("--- Unused ---"), wxT("9") };
	int choice_buff4NChoices = sizeof( choice_buff4Choices ) / sizeof( wxString );
	choice_buff4 = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_buff4NChoices, choice_buff4Choices, 0 );
	choice_buff4->SetSelection( 0 );
	gSizer14->Add( choice_buff4, 0, wxALL, 5 );

	m_staticText24 = new wxStaticText( panel_settings, wxID_ANY, wxT("Buff 5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	gSizer14->Add( m_staticText24, 0, wxALL, 5 );

	wxString choice_buff5Choices[] = { wxT("--- Unused ---"), wxT("0") };
	int choice_buff5NChoices = sizeof( choice_buff5Choices ) / sizeof( wxString );
	choice_buff5 = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_buff5NChoices, choice_buff5Choices, 0 );
	choice_buff5->SetSelection( 0 );
	gSizer14->Add( choice_buff5, 0, wxALL, 5 );

	m_staticText26 = new wxStaticText( panel_settings, wxID_ANY, wxT("\"Stop Action\" key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	gSizer14->Add( m_staticText26, 0, wxALL, 5 );

	wxString choice_stopActionKeyChoices[] = { wxT("=") };
	int choice_stopActionKeyNChoices = sizeof( choice_stopActionKeyChoices ) / sizeof( wxString );
	choice_stopActionKey = new wxChoice( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_stopActionKeyNChoices, choice_stopActionKeyChoices, 0 );
	choice_stopActionKey->SetSelection( 0 );
	gSizer14->Add( choice_stopActionKey, 0, wxALL, 5 );


	bSizer4->Add( gSizer14, 0, wxEXPAND, 5 );

	m_staticline3 = new wxStaticLine( panel_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline3, 0, wxALL|wxEXPAND, 5 );


	panel_settings->SetSizer( bSizer4 );
	panel_settings->Layout();
	bSizer4->Fit( panel_settings );
	m_auinotebook3->AddPage( panel_settings, wxT("Settings"), false, wxNullBitmap );
	panel_pathEdit = new wxPanel( m_auinotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 0, 2, 0, 0 );

	button_startRecording = new wxToggleButton( panel_pathEdit, wxID_ANY, wxT("Start recording"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer5->Add( button_startRecording, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxGridSizer* gSizer8;
	gSizer8 = new wxGridSizer( 0, 2, 0, 0 );

	button_moveToLast = new wxButton( panel_pathEdit, wxID_ANY, wxT("Move to last point"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer8->Add( button_moveToLast, 0, wxALL|wxALIGN_RIGHT, 5 );

	button_undoCheckPoint = new wxButton( panel_pathEdit, wxID_ANY, wxT("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer8->Add( button_undoCheckPoint, 0, wxALL|wxALIGN_RIGHT, 5 );


	gSizer5->Add( gSizer8, 1, wxEXPAND, 5 );


	bSizer7->Add( gSizer5, 0, wxEXPAND, 5 );

	wxArrayString list_pathPointsChoices;
	list_pathPoints = new wxCheckListBox( panel_pathEdit, wxID_ANY, wxDefaultPosition, wxDefaultSize, list_pathPointsChoices, 0 );
	bSizer7->Add( list_pathPoints, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer71;
	gSizer71 = new wxGridSizer( 0, 2, 0, 0 );

	label_merchantId = new wxStaticText( panel_pathEdit, wxID_ANY, wxT("Merchant NPC's id"), wxDefaultPosition, wxDefaultSize, 0 );
	label_merchantId->Wrap( -1 );
	gSizer71->Add( label_merchantId, 0, wxALL, 5 );

	text_merchantNPCid = new wxTextCtrl( panel_pathEdit, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer71->Add( text_merchantNPCid, 0, wxALL, 5 );

	label_homePortal = new wxStaticText( panel_pathEdit, wxID_ANY, wxT("Home portal"), wxDefaultPosition, wxDefaultSize, 0 );
	label_homePortal->Wrap( -1 );
	gSizer71->Add( label_homePortal, 0, wxALL, 5 );

	wxString choice_homePortalChoices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9") };
	int choice_homePortalNChoices = sizeof( choice_homePortalChoices ) / sizeof( wxString );
	choice_homePortal = new wxChoice( panel_pathEdit, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_homePortalNChoices, choice_homePortalChoices, 0 );
	choice_homePortal->SetSelection( 0 );
	gSizer71->Add( choice_homePortal, 0, wxALL, 5 );

	label_huntingGroundPortal = new wxStaticText( panel_pathEdit, wxID_ANY, wxT("Hunting ground portal"), wxDefaultPosition, wxDefaultSize, 0 );
	label_huntingGroundPortal->Wrap( -1 );
	gSizer71->Add( label_huntingGroundPortal, 0, wxALL, 5 );

	wxString choice_huntingGroundPortalChoices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9") };
	int choice_huntingGroundPortalNChoices = sizeof( choice_huntingGroundPortalChoices ) / sizeof( wxString );
	choice_huntingGroundPortal = new wxChoice( panel_pathEdit, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_huntingGroundPortalNChoices, choice_huntingGroundPortalChoices, 0 );
	choice_huntingGroundPortal->SetSelection( 1 );
	gSizer71->Add( choice_huntingGroundPortal, 0, wxALL, 5 );

	m_staticText25 = new wxStaticText( panel_pathEdit, wxID_ANY, wxT("Follow up path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	m_staticText25->SetToolTip( wxT("Path that will be loaded after the bot reaches the end of the current one.") );

	gSizer71->Add( m_staticText25, 0, wxALL, 5 );

	txt_followUpPathName = new wxTextCtrl( panel_pathEdit, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !txt_followUpPathName->HasFlag( wxTE_MULTILINE ) )
	{
	txt_followUpPathName->SetMaxLength( 200 );
	}
	#else
	txt_followUpPathName->SetMaxLength( 200 );
	#endif
	gSizer71->Add( txt_followUpPathName, 0, wxALL, 5 );

	label_pathName = new wxStaticText( panel_pathEdit, wxID_ANY, wxT("Path's name"), wxDefaultPosition, wxDefaultSize, 0 );
	label_pathName->Wrap( -1 );
	gSizer71->Add( label_pathName, 0, wxALL, 5 );

	text_pathName = new wxTextCtrl( panel_pathEdit, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !text_pathName->HasFlag( wxTE_MULTILINE ) )
	{
	text_pathName->SetMaxLength( 200 );
	}
	#else
	text_pathName->SetMaxLength( 200 );
	#endif
	gSizer71->Add( text_pathName, 0, wxALL, 5 );


	bSizer82->Add( gSizer71, 1, wxEXPAND, 5 );


	bSizer7->Add( bSizer82, 0, wxEXPAND, 5 );

	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer( 0, 2, 0, 0 );

	button_savePath = new wxButton( panel_pathEdit, wxID_ANY, wxT("Save path to file"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer6->Add( button_savePath, 0, wxALL|wxALIGN_RIGHT, 5 );

	button_loadPath = new wxButton( panel_pathEdit, wxID_ANY, wxT("Load existing path"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer6->Add( button_loadPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer7->Add( gSizer6, 0, wxEXPAND, 5 );


	panel_pathEdit->SetSizer( bSizer7 );
	panel_pathEdit->Layout();
	bSizer7->Fit( panel_pathEdit );
	m_auinotebook3->AddPage( panel_pathEdit, wxT("Path edition"), false, wxNullBitmap );
	panel_radar = new wxPanel( m_auinotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	text_radar = new wxStyledTextCtrl( panel_radar, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	text_radar->SetUseTabs( true );
	text_radar->SetTabWidth( 4 );
	text_radar->SetIndent( 4 );
	text_radar->SetTabIndents( true );
	text_radar->SetBackSpaceUnIndents( true );
	text_radar->SetViewEOL( false );
	text_radar->SetViewWhiteSpace( false );
	text_radar->SetMarginWidth( 2, 0 );
	text_radar->SetIndentationGuides( true );
	text_radar->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	text_radar->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	text_radar->SetMarginWidth( 1, 16);
	text_radar->SetMarginSensitive( 1, true );
	text_radar->SetProperty( wxT("fold"), wxT("1") );
	text_radar->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	text_radar->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	text_radar->SetMarginWidth( 0, text_radar->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	text_radar->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	text_radar->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	text_radar->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	text_radar->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	text_radar->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	text_radar->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	text_radar->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	text_radar->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	text_radar->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	text_radar->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	text_radar->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	bSizer11->Add( text_radar, 1, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer9;
	gSizer9 = new wxGridSizer( 0, 2, 0, 0 );

	label_radarRange = new wxStaticText( panel_radar, wxID_ANY, wxT("Radar's range"), wxDefaultPosition, wxDefaultSize, 0 );
	label_radarRange->Wrap( -1 );
	gSizer9->Add( label_radarRange, 0, wxALL, 5 );

	text_radarRange = new wxTextCtrl( panel_radar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer9->Add( text_radarRange, 0, wxALL, 5 );

	m_panel6 = new wxPanel( panel_radar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	gSizer9->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );

	checkbox_filterOutIdleUnits = new wxCheckBox( panel_radar, wxID_ANY, wxT("Filter out idle units"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer9->Add( checkbox_filterOutIdleUnits, 0, wxALL, 5 );


	bSizer11->Add( gSizer9, 0, wxEXPAND, 5 );


	panel_radar->SetSizer( bSizer11 );
	panel_radar->Layout();
	bSizer11->Fit( panel_radar );
	m_auinotebook3->AddPage( panel_radar, wxT("Radar"), false, wxNullBitmap );
	panel_misc = new wxPanel( m_auinotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	checkbox_keepConsole = new wxCheckBox( panel_misc, wxID_ANY, wxT("Keep std::out console opened (requires restart)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( checkbox_keepConsole, 0, wxALL, 5 );

	checkbox_autoLogin = new wxCheckBox( panel_misc, wxID_ANY, wxT("Auto-log in (requires restart)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( checkbox_autoLogin, 0, wxALL, 5 );

	checkbox_resizeWindow = new wxCheckBox( panel_misc, wxID_ANY, wxT("Resize the game's window"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( checkbox_resizeWindow, 0, wxALL, 5 );

	checkbox_keepAlive = new wxCheckBox( panel_misc, wxID_ANY, wxT("Keep alive with XenMon"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( checkbox_keepAlive, 0, wxALL, 5 );

	wxGridSizer* gSizer12;
	gSizer12 = new wxGridSizer( 0, 2, 0, 0 );

	m_staticText27 = new wxStaticText( panel_misc, wxID_ANY, wxT("Bring bot UI to the foreground (in-game hotkey)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	gSizer12->Add( m_staticText27, 0, wxALL, 5 );

	wxString choice_uiToForegroundChoices[] = { wxT("F5") };
	int choice_uiToForegroundNChoices = sizeof( choice_uiToForegroundChoices ) / sizeof( wxString );
	choice_uiToForeground = new wxChoice( panel_misc, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_uiToForegroundNChoices, choice_uiToForegroundChoices, 0 );
	choice_uiToForeground->SetSelection( 0 );
	gSizer12->Add( choice_uiToForeground, 0, wxALL, 5 );

	m_staticText29 = new wxStaticText( panel_misc, wxID_ANY, wxT("Stop the bot (in-game hotkey)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	gSizer12->Add( m_staticText29, 0, wxALL, 5 );

	wxString choice_stopBotGameHotkeyChoices[] = { wxT("F6") };
	int choice_stopBotGameHotkeyNChoices = sizeof( choice_stopBotGameHotkeyChoices ) / sizeof( wxString );
	choice_stopBotGameHotkey = new wxChoice( panel_misc, wxID_ANY, wxDefaultPosition, wxDefaultSize, choice_stopBotGameHotkeyNChoices, choice_stopBotGameHotkeyChoices, 0 );
	choice_stopBotGameHotkey->SetSelection( 0 );
	gSizer12->Add( choice_stopBotGameHotkey, 0, wxALL, 5 );


	bSizer10->Add( gSizer12, 0, wxEXPAND, 5 );


	panel_misc->SetSizer( bSizer10 );
	panel_misc->Layout();
	bSizer10->Fit( panel_misc );
	m_auinotebook3->AddPage( panel_misc, wxT("Misc"), false, wxNullBitmap );

	bSizer2->Add( m_auinotebook3, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer2 );
	this->Layout();
	m_statusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

	this->Centre( wxBOTH );
}

XenGUI_template::~XenGUI_template()
{
}
