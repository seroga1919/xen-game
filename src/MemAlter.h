#pragma once
#include "defines.h"


bool initMemoryManager(char * moduleName);


typedef int		(__thiscall	*tDecrypt) (void* pthis, uint8* packet, int length);
typedef int		(__cdecl	*tSendPacket) (uint8 opcode, uint8* packet, int len);
typedef HWND	(__stdcall	*tGetFgWnd) ();
typedef BOOL	(__stdcall	*tSetCursorPos) (int x, int y);

extern tSendPacket oSendPacket;

int __fastcall	decryptHook(void* pthis, void* unk, uint8* packet, int len);
int __cdecl		sendPacketHook(uint8 opcode, uint8* packet, int len);
HWND __stdcall	m_GetForegroundWindow();
BOOL __stdcall m_SetCursorPos(int x, int y);
