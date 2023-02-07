#include <Windows.h>
#include <psapi.h>
#include <detours.h>
#include <sstream>
#include "MemoryManager.h"
#include "Logger.h"


char *	MemoryManager::hostAppName;
DWORD	MemoryManager::dwBaseModuleAddr;
DWORD	MemoryManager::dwMenuAddr;

bool MemoryManager::init(char * hostAppName) {
	bool res = true;
	dwBaseModuleAddr = (DWORD)GetModuleHandle(hostAppName);

	if (dwBaseModuleAddr == NULL) {
		res = false;
		log(E, strcat((char*)"Couldn't find module ", hostAppName));
	}

	int			signature_menuAccess[] = { 0x56, 0x8b, 0x74, 0x24, 0x08, 0x89, 0x35, -1,-1,-1,-1, 0x83, 0xfe, 0x04 };
	MODULEINFO	modInfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), (HMODULE)dwBaseModuleAddr, &modInfo, sizeof(MODULEINFO));
	
	dwMenuAddr	= findPattern(dwBaseModuleAddr, modInfo.SizeOfImage, signature_menuAccess, (sizeof(signature_menuAccess) / sizeof(signature_menuAccess[0])));
	dwMenuAddr += 7;
	dwMenuAddr  = *((int*)dwMenuAddr);

	if (dwMenuAddr == NULL) {
		log(W, "Couldn't find the currently opened menu address in the game's memory");
	}

	return res;
}
int MemoryManager::xen_checkIsSpellCasting() {

	int count;
	count = *(int*)(dwBaseModuleAddr + 0x2AF7118);

	return count;

}
void MemoryManager::xen_SetPotLabelUpdate()
{
	int* num;
	num = (int*)(dwBaseModuleAddr + +0x2B00256);

	(*num)--;
}
void MemoryManager::xen_SetZoomOut() {
	// Those offsets can easily be found through cheat-engine, by searching for
	// unknown initial 4 bytes value, (not-)changing the cursor pos between searches,
	// and searching for (un-)changed values.
	float* camera;
	camera = (float*)(dwBaseModuleAddr + 0x15D5B8);

	(*camera) += 250.0F;
}

int MemoryManager::xen_GetSpellCounter() {
	int* countInc;
	countInc = (int*)(dwBaseModuleAddr + 0x2AF715C);
	if ((int)(uint8)*countInc > 255)
		(*countInc) = 0;
	else
	(*countInc)++;

	int count;
	count = *(int*)(dwBaseModuleAddr + 0x2AF715C);

	return count;
}

bool MemoryManager::detourCode(DETOUR_ITEM item) {

	bool res;
	DWORD addr = 0;

	if (!item.useSignature) {
		*item.pOriginalFunction = (void*)GetProcAddress(GetModuleHandle(item.moduleName), item.targetFunctionName);
	}
	else {
		MODULEINFO modInfo = { 0 };
		DWORD dwModuleStartAddr = (DWORD)GetModuleHandle(hostAppName);
		GetModuleInformation(GetCurrentProcess(), (HMODULE)dwModuleStartAddr, &modInfo, sizeof(MODULEINFO));
		*item.pOriginalFunction = (void*)findPattern(dwModuleStartAddr, modInfo.SizeOfImage, item.oFunctionSignature, item.signatureLength);
		addr = (DWORD)*item.pOriginalFunction;
	}

	if (res = (*item.pOriginalFunction != NULL)) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(item.pOriginalFunction, item.hookFunction);

		res = DetourTransactionCommit() == NO_ERROR;

		std::stringstream ss;
		ss.flags(std::ios_base::hex);
		ss << addr;
		log(I, "Detouring function " + std::string(item.targetFunctionName) + " at address " + ss.str());
	}
	
	return res;
}

bool MemoryManager::detourCode(DETOUR_ITEM * item_array, int len) {
	
	bool res = true;
	for (int i = 0; res && i < len; i++) {
		res = detourCode(item_array[i]);
	}
	return res;
}

bool MemoryManager::compare(BYTE* data, int * signature, int len)
{
	for (int i = 0; i < len; i++) {
		if (signature[i] != -1 && (int)data[i] != signature[i]) {
			return false;
		}
	}
	return true;
}

DWORD MemoryManager::findPattern(DWORD dwAddress, DWORD dwLen, int * signature, int sig_len)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (compare((BYTE*)(dwAddress + i), signature, sig_len))
			return (DWORD)(dwAddress + i);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//							Xenimus specific   								 //
///////////////////////////////////////////////////////////////////////////////
// Opened menu access signature: 56 8b 74 24 08 89 35 * * * * 83 fe 04
// ---------------------------------------------------^^^^^^^ DWORD here

bool MemoryManager::xen_InjectPassword(char * password, int len) {
	// The password object that we're modifying is created
	// when the dialog opens, so we can't store any static address
	
	// pass = [[[55ce14]+40]]		// 14 CE 55 00 -> 0x55ce14
	bool		res;
	int			signaturePasswordEdit[] = { 0x8b, 0x3d,  -1, -1, -1, -1, 0x8a, 0x47, 0x30, 0x33, 0xc9, 0x24, 0x01 };
	MODULEINFO	modInfo					= { 0 };
	DWORD		MEM_addrPassword		= 0;

	GetModuleInformation(GetCurrentProcess(), (HMODULE)dwBaseModuleAddr, &modInfo, sizeof(MODULEINFO));
	MEM_addrPassword = findPattern(dwBaseModuleAddr, modInfo.SizeOfImage, signaturePasswordEdit, (sizeof(signaturePasswordEdit) / sizeof(signaturePasswordEdit[0])));
	MEM_addrPassword += 2;										// Address of the hardcoded pointer's address in ASM (0x425f4b)
	MEM_addrPassword = *((int*)MEM_addrPassword);				// Address of the base pointer (0x55ce14)
	MEM_addrPassword = *((DWORD *)MEM_addrPassword);			// value / *(0x55ce14)

	// This pointer will be initialized once the password dialog is opened
	// If you get errors here, you have to increase the delay before trying
	// to write.
	if (MEM_addrPassword != NULL) {
		MEM_addrPassword = *((DWORD *)(MEM_addrPassword + 0x40));	// value / *(*(0x55ce14)+40)

		if (res = MEM_addrPassword != 0)
			for (int i = 0; ((char*)MEM_addrPassword)[i] !=0, i < len - 1; i++) {
				((char*)MEM_addrPassword)[i] = password[i];
			}
	}
	else {
		log(E,
			std::string("Error while trying to write the pass word")
		);
		res = false;
	}

	return res;
}

bool MemoryManager::xen_SetGameCursorPos(int x, int y) {
	// Those offsets can easily be found through cheat-engine, by searching for
	// unknown initial 4 bytes value, (not-)changing the cursor pos between searches,
	// and searching for (un-)changed values.
	int *px, *py;
	px = (int*)(dwBaseModuleAddr + 0x15FBF8);
	py = (int*)(dwBaseModuleAddr + 0x15FBFC);

	(*px) = x;
	(*py) = y;
	return true;
}

POINT MemoryManager::xen_GetGameCursorPos() {
	POINT p;
	p.x = *(int*)(dwBaseModuleAddr + 0x15FBF8);
	p.y = *(int*)(dwBaseModuleAddr + 0x15FBFC);
	return p;
}

int	MemoryManager::xen_GetCurrentMenu() {

	int res = 0;

	if (dwMenuAddr != NULL) {
		res  = *((DWORD *)dwMenuAddr);
	}

	return res;
}