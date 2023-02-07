#include "IDInputDevice8_Mouse.h"

#define MOUSEBUTTONDOWN(key)	(key & 0x80)
#define MOUSEBUTTON_LEFT		0
#define MOUSEBUTTON_RIGHT		1

void hkIDirectInput8Device_Mouse::sendLeftCLick() {
	inject_LClick = true;
}

void hkIDirectInput8Device_Mouse::sendRightCLick() {
	inject_RClick = true;
}

HRESULT APIENTRY hkIDirectInput8Device_Mouse::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	HRESULT res = hkIDirectInputDevice8::GetDeviceState(cbData, lpvData);

	if (blockUserInput || res!= DI_OK) {
		memset(lpvData, 0, sizeof(DIMOUSESTATE));
	}

	if (inject_LClick) {
		inject_LClick = false;
		((DIMOUSESTATE*)lpvData)->rgbButtons[MOUSEBUTTON_LEFT] |= 0x80;
		res = DI_OK;
	}
	if (inject_RClick) {
		inject_RClick = false;
		((DIMOUSESTATE*)lpvData)->rgbButtons[MOUSEBUTTON_RIGHT] |= 0x80;
		res = DI_OK;
	}
	return res;
}