#pragma once
#include <Windows.h>
#include "defines.h"

#define MENU_NONE			0x00
#define MENU_STARTUP		0x01
#define MENU_CHAR_SELECT	0x09
#define MENU_INVENTORY		0x10
#define MENU_MERCHANT		0x11
#define MENU_LOG_OFF		0x15
#define MENU_ESC			0x16

// Semi-generic template to manipulate the game's memory
class MemoryManager {

public:
	static bool		init(char * hostAppName);
	static void xen_SetPotLabelUpdate();
	static int xen_checkIsSpellCasting();
	static void xen_SetZoomOut();
	static int xen_GetSpellCounter();
	static bool		detourCode(DETOUR_ITEM item);
	static bool		detourCode(DETOUR_ITEM * item_array, int len);

	// Xenimus specific functions
	static bool		xen_InjectPassword(char * password, int len);
	static bool		xen_SetGameCursorPos(int x, int y);
	static POINT	xen_GetGameCursorPos();
	static int		xen_GetCurrentMenu();

private:
	static char *	hostAppName;
	static DWORD	dwBaseModuleAddr;
	static DWORD	dwMenuAddr;

	static bool		compare(BYTE * data, int * signature, int len);
	static DWORD	findPattern(DWORD dwAddress, DWORD dwLen, int * signature, int sign_len);
};