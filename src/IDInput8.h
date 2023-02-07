#pragma once
#include "IDInputDevice8.h"
#include "IDInputDevice8_Mouse.h"
#include "IDInputDevice8_Keyboard.h"


interface hkIDirectInput8 : public IDirectInput8
{
public:
	static hkIDirectInput8Device_Keyboard* hk_keyboard;
	static hkIDirectInput8Device_Mouse* hk_mouse;
	static bool isReady();

	HRESULT APIENTRY QueryInterface(REFIID iid, void ** ppvObject);
	ULONG APIENTRY AddRef(void);
	ULONG APIENTRY Release(void);
	HRESULT APIENTRY ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData);
	HRESULT APIENTRY CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8 *lpDirectInputDevice, LPUNKNOWN pUnkOuter);
	HRESULT APIENTRY EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
	HRESULT APIENTRY EnumDevicesBySemantics(LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags);
	HRESULT APIENTRY FindDevice(REFGUID rguidClass, LPCTSTR ptszName, LPGUID pguidInstance);
	HRESULT APIENTRY GetDeviceStatus(REFGUID rguidInstance);
	HRESULT APIENTRY Initialize(HINSTANCE hinst, DWORD dwVersion);
	HRESULT APIENTRY RunControlPanel(HWND hwndOwner, DWORD dwFlags);

private:
	static bool keyboard_hooked;
	static bool mouse_hooked;
};