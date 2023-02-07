#include <Windows.h>

#include "defines.h"
#include "IDInput8.h"
#include "wrap_dinput8.h"


HINSTANCE	mHinst			= 0;
HINSTANCE	mHinstDLL		= 0;
UINT_PTR	mProcs[5]		= { 0 };
LPCSTR		mImportNames[]	= { "DirectInput8Create", "DllCanUnloadNow", "DllGetClassObject", "DllRegisterServer", "DllUnregisterServer" };

t_DirectInput8Create oDirectInput8Create = NULL;

// ************************************************************************* //
// Connects our proxy dll's functions to the legit dll's ones
bool installWrappers(char * legit_dll_name) {

	bool result = NULL != (mHinstDLL = LoadLibrary(legit_dll_name));
	int item_count = sizeof(mProcs) / sizeof(mProcs[0]);

	if (result) {
		for (int i = 0; i < item_count; i++) {
			mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);
		}
	}

	// Our custom DInputDevice8 classes will allow us to fake input from inside
	oDirectInput8Create = (t_DirectInput8Create)((DWORD)mProcs[0 * 4]);
	return result;
}


hkIDirectInput8		hkDInput8;
IDirectInput8 *		g_pDInInt8 = NULL;	// Dummy object
hkIDirectInput8 *	phkDInput8 = NULL;
LPVOID *			Value;

// Inserts our custom DInput object's reference, allows easy function hooking
// Out dinput8device objects will be kept as static variable in hkIDirectInput8
HRESULT WINAPI DirectInput8Create_wrapper(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	HRESULT hRes = oDirectInput8Create(hinst, dwVersion, riidltf, (LPVOID*)&g_pDInInt8, punkOuter);

	phkDInput8	= &hkDInput8;
	*ppvOut		= phkDInput8;
	Value		= ppvOut;

	return hRes;
}

// ************************************************************************* //
// Unused wrapped functions

extern "C" __declspec(naked) void __stdcall DllCanUnloadNow_wrapper() { __asm {jmp mProcs[1 * 4]} }
extern "C" __declspec(naked) void __stdcall DllGetClassObject_wrapper() { __asm {jmp mProcs[2 * 4]} }
extern "C" __declspec(naked) void __stdcall DllRegisterServer_wrapper() { __asm {jmp mProcs[3 * 4]} }
extern "C" __declspec(naked) void __stdcall DllUnregisterServer_wrapper() { __asm {jmp mProcs[4 * 4]} }