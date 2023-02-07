#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <conio.h>
#include <dinput.h>
#include <time.h>

#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "defines.h"
#include "GameWorld.h"
#include "Path.h"

#define BASE_BOT_DIR		"XenGrinder"

#define WND_X				900		// Position and size for the auto-resize
#define WND_Y				 50
#define WND_WIDTH			930
#define WND_HEIGHT			864

#define MIN_PACKET_CD		150		// In milliseconds
#define MAX_MOVE_DISTANCE	110		// In game "distance units"

const int DIK_DIGICODES[10] = { DIK_0, DIK_1,DIK_2,DIK_3,DIK_4,DIK_5,DIK_6,DIK_7,DIK_8,DIK_9 };

struct Status;
struct SETTINGS;
struct TASK_META;
struct TASK;

struct SETTINGS {		// Saved to 'bot.settings'
	// Bootstraps
	bool resize_window			= false;
	bool keep_console			= true;
	bool auto_log				= false;
	bool auto_save_password		= true;
	char password[16]			= "upwork123";
	// Activity
	bool walk_path				= false;
	bool attack_enemies			= false;
	bool collect_items			= false;
	bool sell_to_merchant		= false;
	char currentPathFile[256] = "sample";
	// Algorithm
	int enemiesBeforeAttack		= 5;
	int aggroDistance			= 50;
	int healOnHPbellow			= 50;
	int usePotOnMPbellow		= 50;
	int startSellOffAfterSlot	= 5;
	int delayBetweenAttacks	= 10;
	// Misc
	int radar_range				= 100;
	bool filterOutIdleUnits		= false;
	int moveSkillKey = 0;
	int targetingMode; // unused
	// Keybinds
	int attackKey				= DIK_1;
	int selfHealKey				= DIK_2;
	int usePotionKey			= DIK_3;
	int usePortalKey			= DIK_5;
	int buff1					= DIK_6;
	int buff2 = 0;
	int buff3 = 0;
	int buff4 = 0;
	int buff5 = 0;
	// Additional features
	bool keep_alive				= true;
	int stopActionKey			= DIK_EQUALS;
	int toForegroundKey			= DIK_F5;
	int stopBotKey				= DIK_F6;
};

// Those two structs are used as a basic mean of communication
// between the hook functions injected into the game's thread,
// and our additional thread in which the bot is executing.
// We obviously need to execute the bot and its UI into dedicated
// threads, so they don't interact with the game's normal flow.
struct GAME_STATUS {
	bool game_ready			= false;
	bool server_online		= false;
	bool character_spawned	= false;
	std::string status		= "Loading";
};

struct FLAGS {
	bool waiting_charSpawn	= false;
	bool waiting_loot		= false;
	bool waiting_skillcast	= false;
	bool pending_password	= false;
	bool password_saved		= false;
	bool is_moving			= false;
	bool inventory_is_full  = false;
	bool inventory_open		= false;
	bool item_acquired		= false;
	bool item_sold			= false;
	bool item_used			= false;
	bool out_of_mpPots		= false;
	bool merchant_too_far	= false; // Dirty fix
	bool merchant_lost		= false;
	bool need_ui_on_front	= false;
};

// NB: Only one instance at a time ever created
class Bot
{
public:
	const char * TARGET = "Xenimus.exe";
	HWND		 m_gameWindow;
	HINSTANCE	 m_hinst;
	HANDLE		 m_keepAliveThread = NULL;
	bool packetLock = false;
	static Bot * currentBot;	// Easy access from outside (getInstance())
	GameWorld  * world;
	
	bool		 mainLoopEntered		= false;
	bool		 run					= true;
	bool		 recordPath				= false;
	TASK *		 currentTask;
	Path*		 currentPath;
	Path*		 currentRecordedPath;
	
	std::vector<TASK>		*task_stack;	// Used as fifo
	std::vector<std::string> paths_names;	// For selection in UI
	
	///////////////////////////////////////////////////////////////////////////
	//					Character's status									 //
	///////////////////////////////////////////////////////////////////////////
	int m_id;
	int x_pos;
	int y_pos;
	int hp;
	int mp;
	int xp;
	clock_t nextBuff;
	clock_t nextHeal;
	clock_t nextAttack;
	clock_t nextPacket;
	clock_t nextPacketHeal;
	clock_t nextPacketMaxie;
	clock_t freeTrans;
	///////////////////////////////////////////////////////////////////////////
	//					Task functions										 //
	///////////////////////////////////////////////////////////////////////////
	TASK FetchNextTask();

	bool waitForSpawn(TASK_META * meta);
	bool displayIntro(TASK_META * meta);
	bool showUI(TASK_META * meta);
	bool resizeWindow(TASK_META * meta);
	bool login(TASK_META * meta);
	bool castTrans();
	bool resetPath(TASK_META * meta);
	bool castTrans(TASK_META* meta);

	bool selfHeal(TASK_META * meta);
	bool takeManaPotion(TASK_META * meta);
	bool selfBuff(TASK_META * meta);

	bool walk(TASK_META * meta);
	bool attack(TASK_META * meta);
	bool loot(TASK_META * meta);
	bool sellItems(TASK_META * meta);

	bool end(TASK_META * meta);
	bool warnOnFail(TASK_META * meta);
	bool exitOnFailedTask(TASK_META * meta);
	bool castTransCheck = false;
	///////////////////////////////////////////////////////////////////////////
	//					Misc.       										 //
	///////////////////////////////////////////////////////////////////////////

	void keepAlive();
	void prepareInitSequence();
	bool checkFlags();
	bool checkMenuWindows();

	bool isTooFar(Unit * unit);
	bool TransWithPacket();
	bool AdegriWithPacket();
	bool BeamWithPacket();
	bool AttackWithPacket();
	bool HealWithPacket();
	bool MaxieWithPacket();
	bool sendMoveOrder(int x, int y, bool useSalt = true, bool mustWalk=false);
	void clickOnMapPosition(int map_x, int map_y);
	void selfClick();
	bool teleportToPortal(int i);
	bool talkToMerchant();
	void doSellingMacro();
	void setPointerToSellingPosition();

	// NB: The bot settings are UI-independant, so we save/load them here
	void loadSettingsFile();
	void saveSettingsFile(bool quiet = false);

public:
	// Those are accessible from outside, to avoid having too many useless
	// wrapper functions.
	SETTINGS	* settings		= new SETTINGS;
	GAME_STATUS * gamestatus	= new GAME_STATUS;
	FLAGS		* flags			= new FLAGS;
	bool leftClickToggle = false;
	Bot(HWND gameWindow, HINSTANCE mHinst);
	
	void hideGameWindow(bool b = true);
	void gameToForeground();

	bool init();
	bool loadSettings();
	bool saveSettings(bool quiet=false);
	bool loadPathsNames();
	
	bool enterMainLoop();
	bool start();
	bool stop();
	bool record(bool b);

	bool healWithPacket();
	int spellPacketCounter;
	int enemyToAttack1;
	int enemyToAttack2 = NULL;

	void onServerLost();
	void onCoherenceLost();
	void onUnrecoverableState();

	void onPasswordSent(uint8 * packet);
	void onMoveOrder(uint8 opcode, uint16* packet, int len);
	void onMove(int x, int y);
	void onLifeStatChange(int hp, int mp, int xp);
	void onMerchantLost();
	void onDeath();
	void onExit();

	int getX() { return x_pos; };
	int getY() { return y_pos; };
	int getHPpercent() { return 100*hp/255; };
	int getMPpercent() { return 100*mp/255; };
	int getXPpercent() { return 100*xp/255; };

	static Bot * getInstance()				 { return currentBot; };
	bool isRunning()						 { return run; }
	bool isRecording()						 { return recordPath; }
	bool canUseMoveSkill();

	std::vector<std::string> getPathsNames() { return paths_names; }
	Path * getCurrentPath()					 { return currentPath; }
	Path * getCurrentRecordedPath()			 { return currentRecordedPath; }
	
	void setId(uint16 id) { m_id = id; }

	// Only called from the game's main flow (not the bot's thread)
	void updateSelf(int x, int y, int hp, int mp, int xp);
};

