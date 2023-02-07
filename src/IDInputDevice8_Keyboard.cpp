#include "IDInputDevice8_Keyboard.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include "Logger.h"
#include "Bot.h"
#include "MemoryManager.h"
#include "IDInput8.h"

// Inject the DIK_key codes inserted into keys_to_inject, then call
// the legit dinput8 function through our hook.
// Note: keys_to_inject is (obviously) flushed after each call.
// Does NOT check the validity of those codes.
Bot* bot;
HRESULT APIENTRY hkIDirectInput8Device_Keyboard::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	HRESULT res = hkIDirectInputDevice8::GetDeviceState(cbData, lpvData);
	if (bot == NULL)
		bot = bot->getInstance();

	if (blockUserInput || res != DI_OK) {
		memset(lpvData, 0, 256);
	}
	di_buff = (char*)lpvData;

	if (di_buff[DIK_F2] & 0x80) {
		//log(I, "UI to foreground hotkey pressed in-game.");
		Bot* bot = bot->getInstance();
		std::cout << std::to_string(bot->x_pos) + ' ' + std::to_string(bot->y_pos) + '\n';
		std::cout << "MX: " + std::to_string(MemoryManager::xen_GetGameCursorPos().x) + " MY: " + std::to_string(MemoryManager::xen_GetGameCursorPos().y) + '\n';
	}
	// Dirty update for the "window to foreground" functionality
	if (di_buff[DIK_F5] & 0x80) {
		//log(I, "UI to foreground hotkey pressed in-game.");
		Bot* bot = bot->getInstance();
		if (bot != NULL && res == DI_OK) {
			bot->flags->need_ui_on_front = true;
		}
	}
	if (di_buff[DIK_F7] & 0x80) {
		bot->enemyToAttack1 = 0;
		std::cout << std::to_string(bot->enemyToAttack1) << std::endl;
	}
	if (di_buff[DIK_L] & 0x80) {
		bot->BeamWithPacket();
	}
	if (di_buff[DIK_SEMICOLON ] & 0x80) {
		bot->AdegriWithPacket();
	}
	if (di_buff[DIK_CAPSLOCK] & 0x80) {
		MemoryManager::xen_SetZoomOut();
	}
	// Dirty update for the "stop bot hotkey" functionality
	if (di_buff[DIK_F6] & 0x80) {
		//log(I, "Stop hotkey presDASDed in-game.");
		Bot* bot = bot->getInstance();
		if (bot != NULL && res == DI_OK) {
			bot->stop();
		}
	}
	if (GetKeyState(VK_LBUTTON) < 0)
	{
		bot->getInstance()->leftClickToggle = true;
	}
	else
	{
		bot->getInstance()->leftClickToggle = false;
	}
	if (keys_to_inject.empty() && !text_to_write.empty()) {
		//std::cout << "looking for new key" << std::endl;
		this->fetch_next_key_from_text();
	}
	else while (!keys_to_inject.empty()) {
		di_buff[keys_to_inject.back()] |= 0x80;
		keys_to_inject.pop_back();
	}

	return res;
}

// Based in Dinput's defines
std::unordered_map<char, int> key_map = {
  { '1',                0x02
},{ '2',                0x03
},{ '3',                0x04
},{ '4',                0x05
},{ '5',                0x06
},{ '6',                0x07
},{ '7',                0x08
},{ '8',                0x09
},{ '9',                0x0A
},{ '0',                0x0B
},{ 'Q',                0x10
},{ 'W',                0x11
},{ 'E',                0x12
},{ 'R',                0x13
},{ 'T',                0x14
},{ 'Y',                0x15
},{ 'U',                0x16
},{ 'I',                0x17
},{ 'O',                0x18
},{ 'P',                0x19
},{ 'A',                0x1E
},{ 'S',                0x1F
},{ 'D',                0x20
},{ 'F',                0x21
},{ 'G',                0x22
},{ 'H',                0x23
},{ 'J',                0x24
},{ 'K',                0x25
},{ 'L',                0x26
},{ 'Z',                0x2C
},{ 'X',                0x2D
},{ 'C',                0x2E
},{ 'V',                0x2F
},{ 'B',                0x30
},{ 'N',                0x31
},{ 'M',                0x32
},{ '=',				DIK_EQUALS
}
};

int	hkIDirectInput8Device_Keyboard::GetDIcodeFromChar(char c) {
	int res = 0;
	auto elem = key_map.find(c);
	if (elem != key_map.end())
		res = elem->second;
	
	return res;
}

char hkIDirectInput8Device_Keyboard::GetCharFromDIcode(int code) {
	char res = 0;
	for (auto elem : key_map) {
		if (elem.second == code) {
			res = elem.first;
			break;
		}
	}
	return res;
}

// Insert the specified dik_code into the pending list of keys
// that will be faked on the next GetDeviceState call.
// Does NOT check the validity of the key code.
void hkIDirectInput8Device_Keyboard::inject_key(int dik_code) {
	keys_to_inject.push_back(dik_code);
}

void hkIDirectInput8Device_Keyboard::write(std::string s) {
	this->text_to_write += s;
}

void hkIDirectInput8Device_Keyboard::fetch_next_key_from_text() {

	char c = 0;
	if (!text_to_write.empty()) {
		c = toupper(text_to_write[0]);
		text_to_write.erase(text_to_write.begin());
	}

	auto search = key_map.find(c);
	if (search != key_map.end()) {
		keys_to_inject.push_back(search->first);
		std::cout << "Pushed back : " << search->first << std::endl;
	}
}
