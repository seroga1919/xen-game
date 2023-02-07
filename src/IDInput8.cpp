#include <windows.h>
#include <dinput.h>

#include "Logger.h"
#include "dllmain.h"
#include "IDInput8.h"
#include "IDInputDevice8_Mouse.h"
#include "IDInputDevice8_Keyboard.h"

extern IDirectInput8*	g_pDInInt8;
extern hkIDirectInput8*	phkDInput8;
extern LPVOID*			Value;

// Initiated by CreateDevice, which is called only once
hkIDirectInput8Device_Keyboard* hkIDirectInput8::hk_keyboard;
hkIDirectInput8Device_Mouse* hkIDirectInput8::hk_mouse;

bool hkIDirectInput8::keyboard_hooked = false;
bool hkIDirectInput8::mouse_hooked = false;

bool hkIDirectInput8::isReady() {
	return keyboard_hooked && mouse_hooked;
}


// Hook the di8 devices with the mouse and keyboard specific classes
HRESULT APIENTRY hkIDirectInput8::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8 *lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	HRESULT Result;
	hkIDirectInputDevice8 *phkDInputDevice8;

	if (rguid == GUID_SysMouse)
	{
		log(I, "Hooked mouse dinputdevice8");
		phkDInputDevice8 = new hkIDirectInput8Device_Mouse;
		hk_mouse = (hkIDirectInput8Device_Mouse*)phkDInputDevice8;
		mouse_hooked = true;
	}
	else if (rguid == GUID_SysKeyboard)
	{
		log(I, "Hooked keyboard dinputdevice8");
		phkDInputDevice8 = new hkIDirectInput8Device_Keyboard;
		hk_keyboard = (hkIDirectInput8Device_Keyboard*)phkDInputDevice8;
		keyboard_hooked = true;
	}
	else {
		log(I, "Hooked unknown dinputdevice8 (will be ignored)");
		phkDInputDevice8 = new hkIDirectInputDevice8;
	}

	Result = g_pDInInt8->CreateDevice(rguid, (LPDIRECTINPUTDEVICE8*)&phkDInputDevice8->g_pDInDev8, pUnkOuter);
	*lplpDirectInputDevice = (LPDIRECTINPUTDEVICE8)phkDInputDevice8;

	return Result;
}

HRESULT APIENTRY hkIDirectInput8::QueryInterface(REFIID iid, void ** ppvObject)
{
	HRESULT hRes = g_pDInInt8->QueryInterface(iid, ppvObject);
	return hRes;
}

ULONG APIENTRY hkIDirectInput8::AddRef(void)
{
	return g_pDInInt8->AddRef();
}

ULONG APIENTRY hkIDirectInput8::Release(void)
{
	return g_pDInInt8->Release();
}

HRESULT APIENTRY hkIDirectInput8::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
{
	return g_pDInInt8->ConfigureDevices(lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
}

HRESULT APIENTRY hkIDirectInput8::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return g_pDInInt8->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT APIENTRY hkIDirectInput8::EnumDevicesBySemantics(LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return g_pDInInt8->EnumDevicesBySemantics(ptszUserName, lpdiActionFormat, lpCallback, pvRef, dwFlags);
}

HRESULT APIENTRY hkIDirectInput8::FindDevice(REFGUID rguidClass, LPCTSTR ptszName, LPGUID pguidInstance)
{
	return g_pDInInt8->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT APIENTRY hkIDirectInput8::GetDeviceStatus(REFGUID rguidInstance)
{
	return g_pDInInt8->GetDeviceStatus(rguidInstance);
}

HRESULT APIENTRY hkIDirectInput8::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	return g_pDInInt8->Initialize(hinst, dwVersion);
}

HRESULT APIENTRY hkIDirectInput8::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	return g_pDInInt8->RunControlPanel(hwndOwner, dwFlags);
}
