#include <windows.h>

#include "dllmain.h"
#include "Logger.h"
#include "defines.h"
#include "wrap_dinput8.h"
#include "MemAlter.h"
#include "Bot.h"
#include "IDInput8.h"

#pragma comment ( lib, "Dxguid.lib" )
#pragma comment (lib, "detours.lib")

#define LEGIT_DI8		(char *)"xen_dinput8.dll"

/*
 *			=== Note regarding the bot's functioning ===
 *
 * > The bot will wrap the game's functions sending and receiving packets to
 * the server, in order to intercept those packets and read their content
 * easily, and also to be able to send our own packets when necessary.
 *
 * See files: MemAlter, MemoryManager
 *
 *
 * > The input method used to make the game send actions is keyboard and mouse
 * emulation, with the exception of movement orders that are also sent through
 * fake packets. This method is supposedly the safest, as the bot can only do
 * what the game client allows it to do.
 * This input is made by using Dinput8 wrapper classes, that will allow us
 * to filter and change the input data retrieved by the original Dinput dll.
 * 
 * Additionally, the game client uses a static map size for its interface,
 * meaning that the position of the game cursor will always have the same
 * memory value when hovering above the UI elements that are of interest to
 * us.
 *
 * In summary, the cusor's position is set through memory writting in the
 * game's variables, the mouse clicks are made through a fake dinput8 mouse
 * device, and the key presses are made through a fake dinput8 keyboard device
 *
 * See files: IDInputDevice8_Keyboard, IDInputDevice8_Mouse, MemoryManager
 *
 */

/*
 * For easy input, you can add those defines:
 *
 * #define setGameCursorPos(x, y)	MemoryManager::xen_SetGameCursorPos(x, y)
 * #define sendLeftClick()			hkIDirectInput8::hk_mouse->sendLeftCLick()
 * #define injectKey(DIK_code)		hkIDirectInput8::hk_keyboard->inject_key(DIK_code)
 *
 */


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH) {
		// NB: We will detour the send/receive functions in the current thread,
		// to ensure that we are ready in time to receive the server's initial
		// ping. It happens that all the functions we want to detour are already
		// loaded in memory at this point, so we can do them all at once,
		// but this could have been different.

		Logger::show_bootstrap_console((char*)"Xenimus autogrinder - loading");
		log(I, "Preparing the host process' memory ...");

		// We install our dinput8 functions and connect to the original dll
		mHinst = hinstDLL;
		if (installWrappers(LEGIT_DI8)) {
			log(I, "Original dll loaded and wrapped");
		}
		else {
			// We exit now, as the game would crash a few seconds later
			log(E,"Failed to wrap the original dll, will exit now");
			system("PAUSE");
			exit(1);
		}
		// NB: At this point the game is viable memory-wise
		if (initMemoryManager((char*)"Xenimus.exe")) {
			log(I, "Required functions detoured");
			// We create our own thread to release the game's normal flow
			if (NULL == CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)init_bot, NULL, NULL, NULL)) {
				log(E, "Failed to create new thread, the bot couldn't be initiated");
			}
		}
		else {
			log(E, "Failed to detour functions, the bot won't be loaded");
		}
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {
		FreeLibrary(mHinstDLL);
	}
	return (TRUE);
}

// Finishes the initialization process, in a new thread
bool init_bot() {
	
	bool res = true;
	int i = 1;
	int max_attempts = 10;
	
	do {	// Dinput8 is initiated after a small delay
		std::string msg = "Waiting for input devices ... [Attempt ";
		msg += std::to_string(i);
		msg += "/";
		msg += std::to_string(max_attempts);
		msg += "]";
		log(W, msg);
		i++;
		Sleep(1000);
	} while (!hkIDirectInput8::isReady() && i <= max_attempts);
	
	if (hkIDirectInput8::isReady()) {
		Bot bot = Bot(FindWindow(NULL, "Xenimus"), mHinst);
		log(I, "Initiating bot ...");
		if (!bot.init()) {
			log(E, "Failled to initialize the auto-grinder !");
			res = false;
		}
		else {
			bot.enterMainLoop();
		}
	}
	else {
		log(E, "Input wasn't properly patched, cannot start.");
		res = false;
	}
	return res;
}
