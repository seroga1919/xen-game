#pragma once
#include <windows.h>
#include <dinput.h>
#include <vector>

#include "IDInputDevice8.h"

interface hkIDirectInput8Device_Keyboard : public hkIDirectInputDevice8
{
public:
	bool				blockUserInput = false;

	void				inject_key(int dik_code);
	void				write(std::string s);
	HRESULT APIENTRY	GetDeviceState(DWORD cbData, LPVOID lpvData);
	static int			GetDIcodeFromChar(char c);
	static char			GetCharFromDIcode(int code);

private:
	std::vector<int>	keys_to_inject;
	std::string			text_to_write;	 // [!] Only alphanumeric supported atm
	char *				di_buff;

	void				fetch_next_key_from_text();
};