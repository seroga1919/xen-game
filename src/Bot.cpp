#include <Windows.h>
#include <shlwapi.h>
#include <fstream>
#include <stdlib.h>
#include <time.h> 

#include "Bot.h"
#include "Logger.h"
#include "MemAlter.h"
#include "wxWidgets_bootstraps.h"
#include "IDInput8.h"

#include "MemoryManager.h"
#include "PacketManager.h"

#include "XenGUI.h"
#include "Utils.h"
#include "XenMonConnect.h"

///////////////////////////////////////////////////////////////////////////////
#define TASK(action, name, onFail)		(TASK)(TASK{&Bot::action,(TASK_META)(TASK_META{ (char*)name }),&Bot::onFail})

// Those structures have been written with the intention of allowing the bot
// to use non-blocking tasks, meaning that a task would be able to keep track
// of an execution context, but release control of the flow before being done.
// In the end I didn't need to push this concept further than the embryonary
// state in which it currently is.
//
struct TASK_META {
	char * name			= (char*)"Unnamed"; // For user friendliness
	bool done			= false;
	int timesAttempted	= 0;
	int maxAttempts		= 5;
	int cooldown_time	= 1;				// Time to wait after the call
	int priority		= 0;				// Higher value = higher priority
};

struct TASK {
	bool(Bot::* action)(TASK_META * self_meta);
	TASK_META meta;
	bool(Bot::* onfail)(TASK_META * self_meta) = NULL;
};
///////////////////////////////////////////////////////////////////////////////
// Current instance is kept for easy remote access
Bot *	Bot::currentBot;

// Global variables that don't need to be class members
bool	coolDown;
char	pendingPassword[256];
int		currentCheckPoint	= 0;
int		enemiesAround		= 0;
Unit *	nextLootable		= NULL;
int		antiDrift			= 0;
int		antiStuck_walk		= 0;
int		antiStuck_loot		=  0;
int		unitPurgeCountDown	= 10;
bool	sellQuantityIncreased = false;
POINT	home;		// Dirty fix to avoid perma-tp when the merchant is lost
bool healused = false;

///////////////////////////////////////////////////////////////////////////
//					Task functions										 //
///////////////////////////////////////////////////////////////////////////

// Only called by enterMainLoop()
// Checks the game's context and decides what to do next
TASK Bot::FetchNextTask() {

	TASK next = (TASK)(TASK{ 0 });
	next.action = NULL;
	checkFlags();
	//std::cout << MemoryManager::xen_checkIsSpellCasting() << std::endl;
	task_stack->clear();
	if (gamestatus->character_spawned)
	{
		checkMenuWindows();
	}
	///////////////////////////////////////////////////////////////////////////
	// Checking for potential issues before finding the next grinding task
	if (!gamestatus->character_spawned) {
		gamestatus->status = "Waiting for character to be spawned";
	}
	else if (isRunning() && PacketManager::timeSinceLastReceived() >= 10.0) {
		onServerLost();
	}
	else if (hp <= 0) {
		onDeath();
	}

	///////////////////////////////////////////////////////////////////////////
	// Self-protection

	else if (!flags->out_of_mpPots && settings->usePotionKey != 0
		&& getMPpercent() < settings->usePotOnMPbellow
		&& clock() > nextHeal
		) {
		healused = false;
		task_stack->push_back(TASK(takeManaPotion, "Using mana potion", warnOnFail));
	}
	else if (settings->selfHealKey != 0
		&& getHPpercent() < settings->healOnHPbellow
		&& clock() > nextHeal)
	{
		healused = false;
		task_stack->push_back(TASK(selfHeal, "Self-heal", warnOnFail));
	}
	///////////////////////////////////////////////////////////////////////////
	// Mob grinding tasks
	else if (settings->sell_to_merchant	&& flags->inventory_is_full) {
		log(I, "Inventory must be sold.");
		task_stack->clear();
		task_stack->push_back(TASK(sellItems, "Sell items", exitOnFailedTask));
	}
	else if (settings->attack_enemies
		&& (currentCheckPoint != 0 || !settings->walk_path) // Only attack on path if not walking or path started
		&& (enemiesAround = GameWorld::getInstance()->getEnemyNumberInRange(settings->aggroDistance))
		>= settings->enemiesBeforeAttack) {

		//log(I, std::to_string(enemiesAround) + " enemies around : attacking");
		healused = true;
		task_stack->push_back(TASK(attack, "Attack", exitOnFailedTask));
	}
	else if (clock() > nextBuff) {
		healused = true;
		log(I, "Applying buffs");
		task_stack->push_back(TASK(selfBuff, "Self-buff", warnOnFail));
	}
	else if (settings->collect_items
		&& currentCheckPoint != 0		// Only loot in hunting zone
		&& (nextLootable = GameWorld::getInstance()->getNextLootable(canUseMoveSkill())) != NULL
		&& !isTooFar(nextLootable)
		) {
		healused = true;
		flags->waiting_loot = true;
		task_stack->push_back(TASK(loot, "Looting", exitOnFailedTask));
	}
	else if (settings->walk_path) {
		bool canWalk = true;
		if (!canUseMoveSkill() && currentPath != NULL
			&& 0 < currentCheckPoint && currentCheckPoint < currentPath->size()
			) {
			healused = true;
			// If engaged in the path, looking for units on the way
			// On the way = close to the bot, and closer to the destination
			// than the bot itself.
			UNIT_MAP meleeRange = GameWorld::getInstance()->getUnitsInRange(30);
			CPacket_Move p = currentPath->getPoint(currentCheckPoint);
			int distanceFromCheckpoint = distanceBetween(x_pos, y_pos, p.x, p.y);

			for (auto unit : meleeRange) {
				if (distanceFromCheckpoint > unit.second.distanceFrom(p.x, p.y)) {
					canWalk = false;
					break;
				}
			}
		}
		if (canWalk)
			task_stack->push_back(TASK(walk, "Walk to next checkpoint", exitOnFailedTask));
		else
			task_stack->push_back(TASK(attack, "Clearing path to checkpoint", exitOnFailedTask));
	}

	// Currently there is always only one task at a time in the stack
	if (!task_stack->empty()) {
		gamestatus->status = (*task_stack)[0].meta.name;
		next = (*task_stack)[0];
		task_stack->erase(task_stack->begin());
	}
	else {
		gamestatus->status = "Idle";
	}
	return next;
}

bool Bot::waitForSpawn(TASK_META * meta) {
	flags->waiting_charSpawn = !gamestatus->character_spawned;
	meta->timesAttempted++;
	meta->done = gamestatus->character_spawned;

	if (!meta->done && meta->timesAttempted == meta->maxAttempts) {
		log(E, "Error while trying to spawn the character");
	}
	if (!meta->done)
		Sleep(300);

	return meta->done;
}

bool Bot::displayIntro(TASK_META * meta) {
	log("");
	log("//////////////////////////////////////////////////////////////////////////");
	log("//                     Xenimus Auto-Grinder                             //");
	log("//////////////////////////////////////////////////////////////////////////");
	log("");
	return (meta->done = true);
}

bool Bot::showUI(TASK_META * meta) {

	log(I, "Loading UI ...");
	run_wx_gui_from_dll("", this->m_hinst);
	Sleep(2000);
	if (true || !settings->keep_console) { // Console useless after debug phase
		//log(I, "This console will now close ...");
		//HWND console = GetConsoleWindow();
	//	FreeConsole();
		//PostMessage(console, WM_CLOSE, 0, 0);
	}

	return (meta->done = true);
}

bool Bot::resizeWindow(TASK_META * meta) {

	std::string msg;
	meta->timesAttempted++;
	meta->cooldown_time = 2000;

	// The game's window seems to wait for it too
	if (!gamestatus->server_online) {
		msg += "resizeWindow: Waiting for server ping ... [Attempt ";
		msg += std::to_string(meta->timesAttempted);
		msg += "/";
		msg += std::to_string(meta->maxAttempts);
		msg += "]";
		log(W, msg.c_str());
	}
	else if (!SetWindowPos(m_gameWindow, HWND_TOP, WND_X, WND_Y, WND_WIDTH, WND_HEIGHT, 0)) {
		log(E, GetLastErrorAsString());
		msg += "resizeWindow: Unable to resize the game's Window ... [Attempt ";
		msg += std::to_string(meta->timesAttempted);
		msg += "/";
		msg += std::to_string(meta->maxAttempts);
		msg += "]";
		log(W, msg.c_str());
	}
	else {
		log(I, "The game's window has been resized to nominal proportions.");
		meta->done = true;
	}
	return meta->done;
}

bool Bot::login(TASK_META * meta) {
	meta->maxAttempts = 10;
	meta->timesAttempted++;
	meta->done = true;

	if (gamestatus->character_spawned) {
		// Nothing to do
	}
	else if (!gamestatus->server_online) {
		meta->done = false;
		log(I, "Waiting for server ping before auto-login ...");
		Sleep(4000);
	}
	else if (!settings->auto_log) {
		log(I, "Auto-login disabled, waiting for task.");
	}

	else if (settings->password[0] != 0) {
		MemoryManager::xen_SetGameCursorPos(1840, 920);
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		Sleep(500);	// The object might take a while to initialize
		MemoryManager::xen_InjectPassword(settings->password, sizeof(settings->password));
		Sleep(500);
		MemoryManager::xen_SetGameCursorPos(1110, 820);
		Sleep(100);
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		Sleep(1000);
		// check ping
		MemoryManager::xen_SetGameCursorPos(230, 840);
		Sleep(100);
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		flags->waiting_charSpawn = true;
	}
	else {
		log(E, "Password isn't set yet, cannot log");
	}
	return true;
}
int oldpos = NULL;

bool Bot::resetPath(TASK_META * meta) {
	meta->done = true;
	if (currentPath != NULL) {
		teleportToPortal(currentPath->getHomePortal());
		currentCheckPoint = 0;
	}
	return true;
}

bool Bot::selfHeal(TASK_META * meta) {
	meta->done = true;
	int healcount = 0;
	if (healcount > 20)
	{
		healcount = 0;
		nextHeal = clock() + 250;
	}
	else
		healcount++;
	log(I, "Using self-heal");
	flags->waiting_skillcast = true;
			MaxieWithPacket();
	if (flags->waiting_skillcast) {
		log(W, "Tried using self heal, but no skill was cast");
	}
	return true;
}

bool Bot::takeManaPotion(TASK_META * meta) {
	meta->done = true;
	log(I, "Taking MP potion");
	MaxieWithPacket();

	return flags->item_used;
}

bool Bot::selfBuff(TASK_META * meta) {
	meta->done = true;
	int delta;
	srand(time(NULL));
	delta = (rand() % 120 + 60);
	nextBuff = clock() + delta * CLOCKS_PER_SEC;
	
	if (settings->buff1 != 0) {
		log(I, "Applying buff 1");
		hkIDirectInput8::hk_keyboard->inject_key(settings->buff1);
		Sleep(50);
	//	selfClick();
		Sleep(MIN_PACKET_CD);
	}
	if (settings->buff2 != 0) {
		log(I, "Applying buff 2");
		hkIDirectInput8::hk_keyboard->inject_key(settings->buff2);
		Sleep(50);
	//	selfClick();
		Sleep(MIN_PACKET_CD);
	}
	if (settings->buff3 != 0) {
		log(I, "Applying buff 3");
		hkIDirectInput8::hk_keyboard->inject_key(settings->buff3);
		Sleep(50);
	//	selfClick();
		Sleep(MIN_PACKET_CD);
	}
	if (settings->buff4 != 0) {
		log(I, "Applying buff 4");
		hkIDirectInput8::hk_keyboard->inject_key(settings->buff4);
		Sleep(50);
		selfClick();
		Sleep(MIN_PACKET_CD);
	}
	if (settings->buff5 != 0) {
		log(I, "Applying buff 5");
		hkIDirectInput8::hk_keyboard->inject_key(settings->buff5);
		Sleep(50);
	//	selfClick();
		Sleep(MIN_PACKET_CD);
	}
	log(I, "Buff will be applied again in about " + std::to_string(delta) + " seconds");
	return true;
}

bool Bot::walk(TASK_META * meta) {
	meta->done = true;

	if (currentPath->size() == 0) {
		log(W, "Current path is empty, canceling");
		stop();
	}
	else if (currentCheckPoint == 0) {		// Teleport to hunting ground
		GameWorld::getInstance()->purgeUnits();
		GameWorld::getInstance()->clearBlacklist();
		if (teleportToPortal(currentPath->getHuntingGroundPortal())) {
			currentCheckPoint++;
		}
		else
		{
			log(W, "Teleport was called, but seemingly failed");
		}
	}
	else if (currentCheckPoint < currentPath->size()) {
		CPacket_Move p = currentPath->getPoint(currentCheckPoint - 1);
		int distance = distanceBetween(p.x, p.y, x_pos, y_pos);

		if (distance <= 80) {
			// arrived
			antiStuck_walk = 0;
			currentCheckPoint++;
			flags->is_moving = false; // Makes movement more fluid
			log(I, "Arrived to checkpoint");
			coolDown = false;
		}
		else if (!sendMoveOrder(p.x, p.y)) {
			stop();
		}
		else if ((antiStuck_walk++) > 100) { // <=> min 100*150ms = 15s
			antiStuck_walk = 0;
			log(W, "Bot appears to be stuck, calling path reset");
			task_stack->push_back(TASK(resetPath, "Resetting path", warnOnFail));
		}
	}
	//else
	if (currentCheckPoint >= currentPath->size()) {	// Path finished
		coolDown = false;
		currentCheckPoint = 0;
		std::string followUp = getCurrentPath()->getFollowUpPathName();

		if (!followUp.empty()) {
			log(I, "Current path finished, loading " + followUp);

			if (!getCurrentPath()->loadFromFile(
				(char *)(std::string(BASE_BOT_DIR) + "\\paths\\" + followUp + ".path").c_str()
			)) {
				log(E, "Error loading " + followUp + ". Will loop current path.");
				getCurrentPath()->setFollowUpPathName("");
			}
		}
	}

	return true;
}

bool Bot::attack(TASK_META * meta) {
	meta->done = true;
	//if (GameWorld::getInstance()->getLocX() != 0) {
	//	MemoryManager::xen_SetGameCursorPos(
	//		1000 + ((GameWorld::getInstance()->getLocX() - x_pos) * 8.33),	// 8.33 and 5.21 and the ratio mouse distance / map distance
	//		600 + ((GameWorld::getInstance()->getLocY() - y_pos) * 5.21)
	//	);
//	}
	// Non-blocking attack cooldown
	if (clock() < nextAttack) {
		return true;
	}

	if (GameWorld::getInstance()->getEnemyNumberInRange(10) > 0
		&& currentCheckPoint != NULL
		) {
		CPacket_Move p = currentPath->getPoint(currentCheckPoint - 1);
		gamestatus->status = "Disentangling character from units";
		sendMoveOrder(p.x, p.y, true, true);
		Sleep(MIN_PACKET_CD);
	}
	flags->waiting_skillcast = true;
	hkIDirectInput8::hk_keyboard->inject_key(settings->attackKey);
	//Sleep(30);
	//hkIDirectInput8::hk_mouse->sendLeftCLick();
	//Sleep(30);
	if (!flags->waiting_skillcast) {
		if (enemiesAround == GameWorld::getInstance()->getEnemyNumberInRange(settings->aggroDistance)) {
			unitPurgeCountDown--;
			//log(W, "Unit count didn't change after attack");
		}
		if (unitPurgeCountDown <= 0) {
			GameWorld::getInstance()->purgeUnits();
			unitPurgeCountDown = 10;
			log(W, "Attacked too often without changing unit count, purging unit map");
		}
	}
	else
	{
	//	log(W, "AOE skill key was pressed, but no skill was cast");
		meta->timesAttempted++;
	}
	//Sleep(settings->delayBetweenAttacks);
	nextAttack = clock() + settings->delayBetweenAttacks;
	return true;
}

bool Bot::loot(TASK_META * meta) {

	if (nextLootable == NULL) {
		flags->waiting_loot = false;
		coolDown = false;
	}
	else if (nextLootable->distanceFrom(x_pos, y_pos) < 15
		|| flags->item_acquired) {

		flags->waiting_loot = false;

		if (flags->item_acquired) {
			flags->item_acquired = false;
			antiStuck_loot = 0;
			log(I, "Looted at <"
				+ std::to_string(x_pos) + " ; " + std::to_string(y_pos) + ">"
			);
			coolDown = false;
		}
		else {
			// Sometimes the loot doesn't work, this gives a "second chance"
			// (We know the loot is reachable since we're close to it)
			antiStuck_loot++;
			sendMoveOrder(nextLootable->getUnitMovement().positionX,
				nextLootable->getUnitMovement().positionY);
		}
		nextLootable = NULL;
	}
	else {
		flags->waiting_loot = true;
		antiStuck_loot++;
		sendMoveOrder(nextLootable->getUnitMovement().positionX,
			nextLootable->getUnitMovement().positionY,
			false);

		if (GameWorld::getInstance()->getEnemyNumberInRange(30) > 0) {
			TASK_META m;
			attack(&m);
		}
	}

	// In case the loot is behind a wall or something
	if (nextLootable != NULL && flags->waiting_loot
		&& antiStuck_loot >= 66	// 66*150ms = ~10s min
		) {
		antiStuck_loot = 0;
		GameWorld::getInstance()->blacklistLootPosition(
			nextLootable->getUnitMovement().positionX,
			nextLootable->getUnitMovement().positionY
		);
		log(W, "Cant reach loot, adding position to temporary blacklist");
		nextLootable = NULL;
	}
	meta->done = true;
	return meta->done;
}

bool Bot::sellItems(TASK_META * meta) {

	currentCheckPoint = 0;
	nextLootable = NULL;
	bool reachedHome = false;

	if (home.x == 0 || flags->merchant_too_far) {
		
		if (flags->merchant_too_far) {
			log(W, "Merchant seems too far away");
		}
		log(I, "Teleporting home");

		reachedHome = teleportToPortal(currentPath->getHomePortal());
		flags->merchant_too_far = reachedHome;
	}
	else if (distanceBetween(x_pos, y_pos, home.x, home.y) > 200) {

		reachedHome = teleportToPortal(currentPath->getHomePortal());
		Sleep(500);
		reachedHome = reachedHome && distanceBetween(x_pos, y_pos, home.x, home.y) < 200;
	}
	else {
		reachedHome = true;
	}

	if (!reachedHome) {
		log(E, "Failed to teleport to merchant"); // Will come back next iteration of fetch
		// Necessary to re-assess current situation
		meta->done = true;
	}
	else if (talkToMerchant()) {
		log(I, "Shop is opened");
		if (home.x == 0) {
			log(I, "Will record home portal's position after a 3s timeout to prevent laggy coordinates ...");
			Sleep(3000);
			home.x = x_pos;
			home.y = y_pos;
			log(I, "Home portal's position set : <"
				+ std::to_string(home.x) + ";" + std::to_string(home.y) + ">"
			);
		}
		log(I, "Starting sell off procedure ...");
		doSellingMacro();
		hkIDirectInput8::hk_keyboard->inject_key(DIK_ESCAPE);
		flags->inventory_open = false;
		flags->inventory_is_full = false;
		meta->done = true;
	}
	else {
		onMerchantLost();
	}
	return true;
}

bool Bot::end(TASK_META * meta) {
	// Nothing to do here, at least for now.
	return TRUE;
}

bool Bot::warnOnFail(TASK_META * meta) {
	log(W, std::string("Failed task: ") + meta->name);
	currentTask = NULL;
	return true;
}

bool Bot::exitOnFailedTask(TASK_META * meta) {
	log(E,
		(std::string("The bot has encountered an error and will exit: ")
			.append(meta->name))
	);
	Sleep(1000);
	onUnrecoverableState();
	return true;
}


///////////////////////////////////////////////////////////////////////////
//					Misc.       										 //
///////////////////////////////////////////////////////////////////////////

void Bot::keepAlive() {
	m_keepAliveThread = CreateThread(NULL, NULL,
		(LPTHREAD_START_ROUTINE)XenMonConnect, &m_keepAliveThread, NULL, NULL);
	if (m_keepAliveThread == NULL) {
		log(E, "Failed to create the 'keep alive' thread.");
		m_keepAliveThread = (HANDLE)-1;
	}
}

void Bot::prepareInitSequence() {

	// First task is a simple header display
	currentTask = new TASK;
	currentTask->action = &Bot::showUI;
	currentTask->meta.cooldown_time = 3000;

	task_stack->push_back(TASK(displayIntro, "Display Intro", exitOnFailedTask));
	if (settings->resize_window) {
		task_stack->push_back(TASK(resizeWindow, "Resize Window", warnOnFail));
	}
	if (settings->auto_log) {
		task_stack->push_back(TASK(login, "login", exitOnFailedTask));
	}
}

bool Bot::checkFlags() {
	if (flags->waiting_charSpawn) {
		task_stack->push_back((TASK)(TASK{
			&Bot::waitForSpawn,
			(TASK_META)(TASK_META{ (char*)"WaitCharSpawn", false, 0, 10 }),
			&Bot::exitOnFailedTask
			}));
	}
	if (!flags->password_saved && flags->pending_password) {
		flags->pending_password = false;
		if (gamestatus->character_spawned) {
			strcpy(settings->password, pendingPassword);
			log(I, "New password was saved to the settings");
			saveSettings();
			flags->password_saved = true;
		}
	}
	return true;
}

bool Bot::checkMenuWindows() {
	int menu = MemoryManager::xen_GetCurrentMenu();

	if (!flags->waiting_charSpawn && menu != 0) {
		std::string str = "A menu windows was opened : ";
		switch (menu) {
		case MENU_INVENTORY:
			str += "Inventory menu";
			break;
		case MENU_MERCHANT:
			str += "Merchant menu";
			break;
		case MENU_LOG_OFF:
			str += "Log off menu (bot will stop)";
			stop();
			break;
		case MENU_ESC:
			str += "ESC menu";
			break;
		case MENU_STARTUP:
			str += "Start menu";
			break;
		default:
			str += "Unknown";
			break;
		}

		log(E, str);
		if (menu != MENU_NONE)
		{
			log(I, "Attempting to get rid of unexpected menu window ...");
			int timeOut = 50;
			while (timeOut > 0
				&& MemoryManager::xen_GetCurrentMenu() != MENU_NONE
				) {
				hkIDirectInput8::hk_keyboard->inject_key(DIK_ESCAPE);
				Sleep(1000);
				timeOut--;
			}
			if (MemoryManager::xen_GetCurrentMenu() == MENU_NONE) {
				log(I, "Menu window cleared.");
			}
		}
	}
	return MemoryManager::xen_GetCurrentMenu() == MENU_NONE;
}

// Currently unused, added as place-holder if needed later on
bool Bot::isTooFar(Unit * unit) {
	return false;
}
bool Bot::TransWithPacket()
{
	if (clock() > freeTrans)
	{
		freeTrans = clock() + (rand() % 50 + 250);
		int OnScreenX = MemoryManager::xen_GetGameCursorPos().x;
		int OnScreenY = MemoryManager::xen_GetGameCursorPos().y;
		
		uint8* packet = (uint8*)malloc(0xC);
		memset(packet, 0, 0xC);
		*((uint16*)packet) = (uint16)m_id;
		*((uint8*)packet + 3) = (uint8)MemoryManager::xen_GetSpellCounter();
		*((uint16*)packet + 2) = (uint16)76;
		*((uint16*)packet + 3) = (uint16)38;
		*((uint16*)packet + 4) = (uint16)(((OnScreenX- 1000) / 8.33) + x_pos);
		*((uint16*)packet + 5) = (uint16)(((OnScreenY - 600) / 5.21) + y_pos);
		*((uint16*)packet + 6) = (uint16)0;
		*((uint16*)packet + 7) = (uint16)19;
		*((uint8*)packet + 16) = (uint8)19;
		sendPacketHook(0x8, (uint8*)packet, 0xC);

	}

	return true;
}
bool Bot::AdegriWithPacket()
{
	Unit* enemy = GameWorld::getInstance()->getClosestUnitFromRealId(enemyToAttack1);
	if (enemy != NULL && clock() > nextPacket)
	{
		nextPacket = clock() + (rand() % 50 + 80);
		std::cout << "ID: " + std::to_string((int)enemy->getId()) << std::endl;

		uint8* packet = (uint8*)malloc(0xC);
		memset(packet, 0, 0xC);
		*((uint16*)packet) = (uint16)m_id;
		*((uint8*)packet + 3) = (uint8)MemoryManager::xen_GetSpellCounter(); //
		*((uint8*)packet + 4) = (uint8)76;
		*((uint8*)packet + 5) = (uint8)0;
		*((uint8*)packet + 6) = (uint8)6;
		*((uint8*)packet + 7) = (uint8)0;
		*((uint16*)packet + 4) = (uint16)(int)enemy->getUnitMovement().positionX;
		*((uint16*)packet + 5) = (uint16)(int)enemy->getUnitMovement().positionY;
		*((uint16*)packet + 6) = (uint16)0;
		*((uint16*)packet + 7) = (uint16)19;
		*((uint8*)packet + 16) = (uint8)19;

		sendPacketHook(0x8, (uint8*)packet, 0xC);

	}

	return true;
}
bool Bot::BeamWithPacket()
{
	Unit* enemy = GameWorld::getInstance()->getClosestUnitFromRealId(enemyToAttack1);
	if (enemy != NULL && clock() > nextPacket)
	{
		nextPacket = clock() + (rand() % 50 + 250);
		std::cout << "ID: " + std::to_string((int)enemy->getId()) << std::endl;

		uint8* packet = (uint8*)malloc(0xC);
		memset(packet, 0, 0xC);
		*((uint16*)packet) = (uint16)m_id;
		*((uint8*)packet + 3) = (uint8)MemoryManager::xen_GetSpellCounter(); //
		*((uint8*)packet + 4) = (uint8)80;
		*((uint8*)packet + 5) = (uint8)0;
		*((uint8*)packet + 6) = (uint8)48;
		*((uint8*)packet + 7) = (uint8)0;
		*((uint16*)packet + 4) = (uint16)(int)enemy->getId();
		*((uint16*)packet + 5) = (uint16)(int)enemy->getUnitMovement().positionY;
		*((uint16*)packet + 6) = (uint16)0;
		*((uint16*)packet + 7) = (uint16)19;
		*((uint8*)packet + 16) = (uint8)19;

		sendPacketHook(0x8, (uint8*)packet, 0xC);

	}

	return true;
}
bool Bot::HealWithPacket()
{
	if (clock() > nextPacketHeal)
	{
		int delta = (rand() % 20 + 40);
		nextPacketHeal = clock() + delta;

		uint8* packet = (uint8*)malloc(0xC);
		memset(packet, 0, 0xC);
		*((uint16*)packet) = (uint16)m_id;
		*((uint8*)packet + 3) = (uint8)MemoryManager::xen_GetSpellCounter(); //
		*((uint8*)packet + 4) = (uint8)80;
		*((uint8*)packet + 5) = (uint8)0;
		*((uint8*)packet + 6) = (uint8)1;
		*((uint8*)packet + 7) = (uint8)0;
		*((uint16*)packet + 4) = (uint16)m_id;
		*((uint16*)packet + 5) = (uint16)y_pos;
		*((uint16*)packet + 6) = (uint16)0;
		*((uint16*)packet + 7) = (uint16)19;
		*((uint8*)packet + 16) = (uint8)19;

		sendPacketHook(0x8, (uint8*)packet, 0xC);

	}

	return true;
}
bool Bot::MaxieWithPacket()
{
	if (clock() > nextPacketMaxie)
	{
		int delta = (rand() % 50 + 450);
		nextPacketMaxie = clock() + 500;
		uint8* packet = (uint8*)malloc(0xC);
		memset(packet, 0, 0xC);
		*((uint16*)packet) = (uint16)m_id;
		*((uint8*)packet + 3) = (uint8)MemoryManager::xen_GetSpellCounter(); //
		*((uint8*)packet + 4) = (uint8)78;
		*((uint8*)packet + 5) = (uint8)0;
		*((uint8*)packet + 6) = (uint8)221;
		*((uint8*)packet + 7) = (uint8)0;
		*((uint16*)packet + 4) = (uint16)m_id;
		*((uint16*)packet + 5) = (uint16)y_pos;
		*((uint16*)packet + 6) = (uint16)0;
		*((uint16*)packet + 7) = (uint16)19;
		*((uint8*)packet + 16) = (uint8)19;

		sendPacketHook(0x7, (uint8*)packet, 0xC);
	}
	MemoryManager::xen_SetPotLabelUpdate();
	Sleep(10);
	checkMenuWindows();
	return true;
}
// Uses both clicks and packets sending. The packets' structure hasn't
// changed in years so it's relatively safe.
bool Bot::sendMoveOrder(int x, int y, bool useSalt, bool mustWalk) {

	if (useSalt) {
		srand(time(NULL));
		x += (rand() % 15 + 1);
		y += (rand() % 15 + 1);
	}
	int d;
	int i = 100;

	if (antiDrift < 10
		&& (d = distanceBetween(x, y, x_pos, y_pos))
	> MAX_MOVE_DISTANCE * 10) {
		log(W,
			std::string("Distance from destination seems too high: ")
			+ std::to_string(d)
			+ ". Not moving."
		);
		Sleep(700);
		antiDrift++;
	}
	else if (d > MAX_MOVE_DISTANCE * 10) {
		log(E, "Coherence check: Move order sent for "
			+ std::to_string(d) + " units away.");
		onCoherenceLost();
	}
	else if (d > MAX_MOVE_DISTANCE) {
		//log(W, "A move order was issued to a destination too far away. Will interpolate middle point.");
		int middle_x = (x + x_pos) / 2;
		int middle_y = (y + y_pos) / 2;
		return sendMoveOrder(middle_x, middle_y);
	}

	if (!mustWalk && canUseMoveSkill()
		) {
		hkIDirectInput8::hk_keyboard->inject_key(settings->moveSkillKey);
		Sleep(50);
		clickOnMapPosition(x, y);	// For looting, must walk on the corpse
		Sleep(150);
	}
	else
	{
		Sleep(150);
		//clickOnMapPosition(x, y);		// Using packets for better accuracy
		uint8* packet = (uint8*)malloc(0x10);
		memset(packet, 0, 0x10);
		*((uint16*)packet) = (uint16)m_id;
		*((uint16*)packet + sizeof(uint16)) = (uint16)x;
		*((uint16*)packet + sizeof(uint16) * 2) = (uint16)y;
		*((uint16*)packet + sizeof(uint16) * 3) = (uint16)PacketManager::genMoveChecksum((uint16)m_id, (uint16)x, (uint16)y, 0x4D);

		sendPacketHook(1, (uint8*)packet, 0x10);
	}
	Sleep(50);
	antiDrift = 0;
	flags->is_moving = true;
	return true;
}

void Bot::clickOnMapPosition(int map_x, int map_y) {

	if (gamestatus->character_spawned) {
		hkIDirectInput8::hk_mouse->blockUserInput = true;
		MemoryManager::xen_SetGameCursorPos(
			1000 + ((map_x - x_pos) * 8.33),	// 8.33 and 5.21 and the ratio mouse distance / map distance
			600 + ((map_y - y_pos) * 5.21)
			
		);
		Sleep(50);
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		hkIDirectInput8::hk_mouse->blockUserInput = false;
	}
}
void Bot::selfClick() {
	//POINT curPos = MemoryManager::xen_GetGameCursorPos();
	MemoryManager::xen_SetGameCursorPos(1000, 600 - 15);
	hkIDirectInput8::hk_mouse->sendLeftCLick();
	Sleep(30);
	//MemoryManager::xen_SetGameCursorPos(curPos.x, curPos.y);
	//Sleep(30);
}
bool Bot::teleportToPortal(int i) {

	bool res = false;
	int old_mp = mp;

	if (settings->stopActionKey != 0) {
		hkIDirectInput8::hk_keyboard->inject_key(settings->stopActionKey);
		Sleep(250);
	}

	flags->waiting_skillcast = true;
	hkIDirectInput8::hk_keyboard->inject_key(settings->usePortalKey);
	Sleep(300);
	hkIDirectInput8::hk_keyboard->inject_key(DIK_DIGICODES[i]);
	Sleep(600);

	res = !flags->waiting_skillcast;//old_mp >= mp;
	if (!res) {
		log(E, "Teleport called, but no skill cast");
	}

	return res;
}
bool Bot::talkToMerchant() {
	bool merchantClicked = false;
	flags->waiting_loot = false;

	currentCheckPoint = 0;
	nextLootable = NULL;
	Sleep(1000);

	if (!checkMenuWindows()) {
		log(E, "Unable to close the opened menu window, bot must stop");
		onUnrecoverableState();
	}

	log(I, "Looking for merchant with model id " + std::to_string(currentPath->getMerchantModelId()));
	Unit * merchant = GameWorld::getInstance()->getClosestUnitFromModel(currentPath->getMerchantModelId());
	int d;

	if (merchant == NULL) {
		log(E, "Couldn't find the merchant npc");
	}
	else if ((d = merchant->distanceFrom(x_pos, y_pos)) > 120) {
		log(E, "Merchant NPC is too far away to reliably interact with : " + std::to_string(d));
		flags->merchant_too_far = true;
	}
	else if ((d = merchant->distanceFrom(x_pos, y_pos)) < 10) {
		sendMoveOrder(x_pos + 10, y_pos + 10);
	}
	else {
		Sleep(2000);
		flags->inventory_open = false;

		for (int i = 0;
			!(flags->inventory_open || MemoryManager::xen_GetCurrentMenu() == MENU_MERCHANT)
			&& i < 5;
			i++) {

			Sleep(1000);
			log(I, "Trying to open the shop's panel");
			merchant = GameWorld::getInstance()->getClosestUnitFromModel(currentPath->getMerchantModelId());
			if (merchant == NULL) {
				log(I, "Merchant lost");
				break;
			}
			else {
				merchantClicked = true;
				clickOnMapPosition(
					merchant->getUnitMovement().positionX,
					merchant->getUnitMovement().positionY
				);
				int i = 0;
				log(I, "Waiting for inventory to open ...");
				while (!flags->inventory_open && i < 50) {
					i++;
					Sleep(100);
				}
			}
		}
	}
	return merchantClicked && flags->inventory_open;
}
void Bot::doSellingMacro() {

	hkIDirectInput8::hk_mouse->blockUserInput = true;
	// Increase sell quantity to 15/20 once per runtime
	if (!sellQuantityIncreased) {
		MemoryManager::xen_SetGameCursorPos(1280, 800);
		Sleep(100);
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		Sleep(300);
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		Sleep(300);
		sellQuantityIncreased = true;
	}

	setPointerToSellingPosition();
	Sleep(200);
	int antiLag = 0;	// In case the bot clicks too fast for the game to sell
	int antiStuck = 0;	// In rare occasions this is necessary
	do {
		antiStuck++;
		flags->item_sold = false;
		hkIDirectInput8::hk_mouse->sendLeftCLick();
		antiLag++;
		Sleep(150);
		if (flags->item_sold) {
			antiLag = 0;
		}
	} while ((flags->item_sold || antiLag <= 3) && antiStuck <= 200);

	if (antiStuck >= 200) {
		log(W, "Sell-off loop exited by anti-stuck.");
	}
	hkIDirectInput8::hk_mouse->blockUserInput = false;
}
void Bot::setPointerToSellingPosition() {
	//columns = x: 1100 + i*70
	//rows = y: 430 + i*80
	// increase qty : 1280/800
	int position = settings->startSellOffAfterSlot + 1;
	int column = position % 5;
	column = (column == 0) ? 5 : column;
	int row = (position / 5) - ((position % 5) == 0 ? 1 : 0);

	log(I, "Sell off will start on item slot " + std::to_string(position));
	MemoryManager::xen_SetGameCursorPos(1100 + (column - 1) * 70, 430 + row * 80);
}
void Bot::loadSettingsFile() {
	std::string file = GetExePath();
	file += "\\";
	file += BASE_BOT_DIR;
	file += "\\bot.settings";

	std::ifstream ifs;
	ifs.open(file.c_str(), std::ios::binary);

	if (!ifs) {
		log(W,
			std::string("Error when trying to load settings from ").append(file)
		);
	}
	else {
		log(I,
			std::string("Reading settings from ").append(file)
		);
		ifs.read((char*)settings, sizeof(SETTINGS));
	}
	ifs.close();
}
void Bot::saveSettingsFile(bool quiet) {
	std::string file = GetExePath();
	file += "\\";
	file += BASE_BOT_DIR;
	file += "\\bot.settings";

	if (!quiet) {
		log(I, std::string("Saving settings to ").append(file));
	}

	std::ofstream ofs(file.c_str(), std::ios::binary);
	ofs.write((char*)settings, sizeof(SETTINGS));
	ofs.close();

	if (!ofs) {
		log(E,
			std::string("Error when trying to save settings to ").append(file)
		);
	}
}

///////////////////////////////////////////////////////////////////////////
//					Public methods										 //
///////////////////////////////////////////////////////////////////////////


Bot::Bot(HWND gameWindow, HINSTANCE mHinst) {

	this->m_hinst				= mHinst;
	this->m_gameWindow			= gameWindow;

	this->task_stack			= new std::vector<TASK>();
	this->world					= new GameWorld();
	this->currentRecordedPath	= new Path();
	this->currentPath			= new Path();
	this->currentTask			= new TASK;
	this->nextBuff				= 0;
	this->nextHeal = 0;

	currentBot = this;
}

void Bot::hideGameWindow(bool b) {
	UINT uFlags = b ? SWP_HIDEWINDOW : SWP_SHOWWINDOW;
	if (!SetWindowPos(m_gameWindow, HWND_TOP, WND_X, WND_Y, WND_WIDTH, WND_HEIGHT, uFlags)) {
		log(E, GetLastErrorAsString());
	}
}
void Bot::gameToForeground() {
	SetForegroundWindow(m_gameWindow);
}
// Check that we're in Xenimus, and loads the settings and paths list
bool Bot::init() {

	bool res = false;

	if (NULL == GetModuleHandle(this->TARGET)) {
		std::string msg = "The current process isn't ";
		msg	+=	TARGET;
		log(E, msg.c_str());
	}
	else if (!loadSettings()) {
		log(E, "Unable to load the bot's settings");
	}
	else if (!loadPathsNames()) {
		// Shouldn't happen if we accessed the folder in the above branch
	}
	else {
		res = true;
	}

	// Not mandatory to run the bot
	std::string buff = BASE_BOT_DIR;
	if (!CreateDirectory((buff + "\\logs").c_str(), NULL)
		&& ERROR_ALREADY_EXISTS != GetLastError()) {
		log(E, "Unable to access logs folder");
	} 
	else {
		if (!Logger::setFileOutput((char*)"")) {
			log(E, "Failed to open the log file");
		}
	}

	return res;
}
// Loads the bot's settings.
// Will also create the bot's dedicated folders if they don't exist.
bool Bot::loadSettings() {

	bool res = true;
	std::string buff = BASE_BOT_DIR;

	// Checking file system, and creating if necessary
	if (!CreateDirectory(buff.c_str(), NULL)
		&& ERROR_ALREADY_EXISTS != GetLastError())
	{
		log(E, "Unable to access the bot's base folder");
		res = false;
	} 
	else if (!CreateDirectory((buff+"\\paths").c_str(), NULL)
		&& ERROR_ALREADY_EXISTS != GetLastError())
	{
		log(E, "Unable to access the path's folder");
		res = false;
	}
	else if (!PathFileExists((buff + "\\bot.settings").c_str())) {
		log(I, "No settings file was found, using default presets");
		log(I, "NB: Settings will be saved upon exiting");
	}
	else {
		loadSettingsFile();
		std::string file = "XenGrinder\\paths\\" + std::string(settings->currentPathFile) + ".path";
		log(I, ("Loading path " + file));
		currentPath->loadFromFile((char*)file.c_str());
	}
	return res;
}
bool Bot::loadPathsNames() {

	std::string dir = GetExePath();
	dir += "\\";
	dir += BASE_BOT_DIR;
	dir += "\\paths";
	
	log(I, std::string("Looking for path files in: " + dir).c_str());
	read_directory(dir, (char*)"path", &paths_names);

	for (std::string s : paths_names) {
		log(I, ("---- Found path file : " + s).c_str());
	}

	return true;
}
bool Bot::saveSettings(bool quiet) {
	saveSettingsFile(quiet);
	return true;
}
// The bot will enter an infinite loop
// Every iteration, the bot will fetch the currentTask task from the list
bool Bot::enterMainLoop() {
	// In case of faulty call from another thread
	if (!mainLoopEntered) {
		mainLoopEntered = true;
	}
	else {
		log(E, "Main loop already running");
		return false;
	}
	prepareInitSequence();

	log(I, "Entering the bot's main loop");
	do {
		if (settings->keep_alive && m_keepAliveThread == NULL) {
			keepAlive();
		}
		coolDown = true;
		if (run) {
			// Call currentTask task if exists
			if (currentTask != NULL && currentTask->action != NULL
				&& !currentTask->meta.done
				&& currentTask->meta.timesAttempted < currentTask->meta.maxAttempts) {
				
				(this->*(currentTask->action))(&currentTask->meta);	// Current TASK's action
				if (currentTask!=NULL)
					Sleep(currentTask->meta.cooldown_time);
			}
			// Call onFail function attributed to the task, if the task failed
			else if (currentTask != NULL 
				&& currentTask->meta.maxAttempts != 0
				&& currentTask->meta.timesAttempted >= currentTask->meta.maxAttempts
				&& currentTask->onfail != NULL) {
				
				std::cout << "Failed: " << currentTask->meta.name << std::endl;
				(this->*(currentTask->onfail))(&currentTask->meta);
			}
			// Otherwise, fetch next task
			else {
				currentTask = &FetchNextTask();
				coolDown = false;	// Skipping wait as nothing was done
			}
		}
		else { // Bot disabled
			gamestatus->status = "Idle";
			Sleep(1000);
		}

		// Do NOT remove this unless 100% sure you won't be packet spamming
		if (coolDown) {
			Sleep(MIN_PACKET_CD);
		}

	} while (currentTask == NULL || currentTask->action != &Bot::end );
	///////////////////////////////////////////////////////////////////////////
	log(I, "Exit task reached, the game will be closed in 5 seconds ...");

	Sleep(5000);
	exit(0);
}
bool Bot::start() {
	log(I, "Starting ...");
	currentTask = NULL;
	currentCheckPoint = 0;
	nextLootable = NULL;
	nextBuff = 0;
	nextHeal = 0;
	flags->out_of_mpPots = false;
	home.x = 0;
	home.y = 0;
	run = true;
	return true;
}
bool Bot::stop() {
	log(I, "Stopping.");
	run = false;
	task_stack->clear();
	// Easy way to give some time for leftover threads to see that run = false
	Sleep(300);
	return true;
}
bool Bot::record(bool b) {

	bool res = true;

	if (b && !stop()) {
		res = false;
		log(E, "Failed to stop the bot");
	}
	else {
		recordPath = b;
	}
	return res;
}
void Bot::onServerLost() {
	log(E, "Server connection lost.");
	onUnrecoverableState();
}
// Currently only tries to teleport back to base, and restarts
void Bot::onCoherenceLost() {
	log(E, "Coherence lost ...");
	// Are we in game ?
	if (MemoryManager::xen_GetCurrentMenu() == MENU_NONE) {
		// Teleport back to safety
		if (!teleportToPortal(getCurrentPath()->getHomePortal())) {
			onUnrecoverableState();
		} // else
		log(I, "Teleported back to base portal");
		currentCheckPoint = 0;
		Sleep(500);
		stop();
		Sleep(500);
		start();
	}
}
void Bot::onUnrecoverableState() {
	log(E, "The game went into unknown state and cannot be recovered.");

	if (settings->keep_alive) {
		log(I, "Keep-alive enabled: turning auto-login in and closing.");
		settings->auto_log = true;
		saveSettings();
		exit(1);
	}
	else {
		stop();
	}
}
void Bot::onPasswordSent(uint8 * packet) {

	char * pass = (char*)(packet + 0x8);

	if (!flags->password_saved && strcmp(settings->password, pass) != 0) {

		memset(pendingPassword, 0, sizeof(pendingPassword));
		strcpy(pendingPassword, pass);
		flags->pending_password = true;
	}
}
void Bot::onMoveOrder(uint8 opcode, uint16* packet, int len) {
	if (recordPath) {
		CPacket_Move p;
		p.x = *(packet + sizeof(uint16));
		p.y = *(packet + sizeof(uint16) * 2);
		currentRecordedPath->push_back(p);
	}
}
void Bot::onMove(int x, int y) {
	x_pos = x;
	y_pos = y;
	GameWorld::getInstance()->setPlayerPos(POINT{ x, y });
}
void Bot::onLifeStatChange(int hp, int mp, int xp) {
	this->hp = hp;
	this->mp = mp;
	this->xp = xp;
}
// Sometimes the merchant gets glitched in the texture, or is just respawned
// It can take a few minutes, and we don't want the bot to go full retard
// bumping into walls in crowded area
// This is fairly rare so I left this function to the bare minimum
void Bot::onMerchantLost() {

	if (!flags->merchant_lost) {
		flags->merchant_lost = true;
		log(E, std::string("Merchant isn't reachable anymore. ")
			+ "Will wait until new packets arrive.");
	}
	gamestatus->status = "Waiting for merchant";
	int antiAfk = 0;
	while (flags->merchant_lost && isRunning() && settings->sell_to_merchant) {
		Sleep(200);
		antiAfk++;
		if (antiAfk >= 75) {	// 15 seconds
			sendMoveOrder(x_pos, y_pos);
			//antiAfk = 0;
			break;
		}
	}
	if (!flags->merchant_lost) {
		log(I, "Merchant data received");
	}
	else {
		log(I, "Waiting for merchant canceled");
	}
	//flags->merchant_lost = false;
}
void Bot::onDeath()
{
	Sleep(500);		// Fixes the post login stat refresh delay issue
	if (hp <= 0) {
		log(E, "Character is dead.");
		stop();
		if (settings->auto_log) {
			log(I, "Auto-log activated: will restart in 15 seconds.");
			Sleep(15000);
			log(I, "Restarting now ...");
			start();
		}
		else {
			log(I, "Stopping on character's death.");
		}
	}
}
void Bot::onExit() {
	stop();
	saveSettings();
	if (m_keepAliveThread != NULL)
		PostThreadMessage(GetThreadId(m_keepAliveThread), WM_QUIT, 0, 0);
}
bool Bot::canUseMoveSkill() {
	return settings->moveSkillKey != 0 && getMPpercent() >= 35;
}
// Only called from the game's main flow (not the bot's thread)
void Bot::updateSelf(int x, int y, int hp, int mp, int xp) {

	flags->is_moving = (x_pos != x || y_pos != y);
	if (x_pos != x || y_pos != y) {
		onMove(x, y);
	}
	if (this->hp != hp || this->mp != mp || this->xp != xp) {
		onLifeStatChange(hp, mp, xp);
	}
}
