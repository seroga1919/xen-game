#pragma once
#include <windows.h>
#include <dinput.h>

#include "IDInputDevice8.h"


interface hkIDirectInput8Device_Mouse : public hkIDirectInputDevice8
{
public:
	bool blockUserInput = false;

	void sendLeftCLick();
	void sendRightCLick();

	HRESULT APIENTRY GetDeviceState(DWORD cbData, LPVOID lpvData);

private:
	bool inject_LClick;
	bool inject_RClick;
};