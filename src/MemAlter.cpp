#include <Windows.h>
#include <tlhelp32.h>

#include "MemAlter.h"
#include "defines.h"
#include "Logger.h"
#include "MemoryManager.h"
#include "PacketManager.h"
#include "IDInput8.h"


// Pointers to the original functions that will be hooked
tDecrypt		 oDecrypt = NULL;
tSendPacket		 oSendPacket = NULL;
tGetFgWnd		 oGetForWnd = NULL;
tSetCursorPos	 oSetCursorPos = NULL;

// 8b 54 24 04 89 51 10 * * * * 56
int signatureDecrypt[] = { 0x8b, 0x54, 0x24, 0x04, 0x89, 0x51, 0x10, -1, -1, -1, -1, 0x56 };
// 81 * * * * * * * * * 0f 85 * * * * 8b 0d * * * * c1 e1 06
int signatureSendPkt[] = { 0x81, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x0f, 0x85, -1, -1, -1, -1, 0x8b, 0x0d, -1, -1, -1, -1, 0xc1, 0xe1, 0x06 };

DETOUR_ITEM detourlist[] = {
	DETOUR_ITEM{ PPVOID(oDecrypt), decryptHook, signatureDecrypt, SIZE(signatureDecrypt),(char*)"decryptHook" },
	DETOUR_ITEM{ PPVOID(oSendPacket), sendPacketHook, signatureSendPkt, SIZE(signatureSendPkt), (char*)"sendPacketHook" },
	DETOUR_ITEM{ PPVOID(oGetForWnd), m_GetForegroundWindow, 0, SIZE(signatureSendPkt), (char*)"GetForegroundWindow", (char*)"user32.dll", false },
	DETOUR_ITEM{ PPVOID(oSetCursorPos), m_SetCursorPos, 0, SIZE(signatureSendPkt), (char*)"SetCursorPos", (char*)"user32.dll", false }
};

// Simply passes the required hooks to the MemoryManager class
bool initMemoryManager(char * moduleName) {
	bool res;

	if (!(res = MemoryManager::init(moduleName))) {
		log(E, "Failed to initiate MemoryManager");
	}
	else if (!(res = MemoryManager::detourCode(detourlist, SIZE(detourlist)))) {
		log(E, "Failed to detour required functions");
	}

	return res;
}


///////////////////////////////////////////////////////////////////////////////
//							Hook functions to inject						 //
///////////////////////////////////////////////////////////////////////////////

int __fastcall	decryptHook(void *pthis, void* unk, uint8* packet, int len)
{
	int decryptResult = oDecrypt(pthis, packet, len);
	PacketManager::handleInboundPacket(packet, len);
	return decryptResult;
}

// Maybe not actually send the packet, depending on the packet handler's result
int __cdecl		sendPacketHook(uint8 opcode, uint8* packet, int len)
{
	int res = 0;
	if (PacketManager::handleOutboundPacket(opcode, packet, len)) {
		res = oSendPacket(opcode, packet, len);
	}
	return res;
}

// Makes the game think it's in the foreground
// Additionally, blocks user input when the game isn't actually in
// foreground, to avoid dinput from capturing it
HWND hGameWindow = NULL;
bool block;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);

	// Enumerating windows until we find the one of the game
	if (pid == GetProcessId(GetCurrentProcess())) {
		hGameWindow = hwnd;
		log(I, "Registered game's window handler");
		return FALSE;
	}
	return TRUE;
}

HWND __stdcall	m_GetForegroundWindow() {

	if (hGameWindow == NULL) {
		// Doesn't work with multiple clients
		//hGameWindow = FindWindow(NULL, "Xenimus");

		EnumWindows(EnumWindowsProc, NULL);
	}
	if (hkIDirectInput8::isReady()) {
		block = hGameWindow != oGetForWnd();
		hkIDirectInput8::hk_mouse->blockUserInput = block;
		hkIDirectInput8::hk_keyboard->blockUserInput = block;
	}
	return hGameWindow;
}

BOOL __stdcall m_SetCursorPos(int x, int y) {
	if (!block)
		return oSetCursorPos(x, y);
	else return TRUE;
}