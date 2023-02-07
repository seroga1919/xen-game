#include "XenMonConnect.h"
#include "Utils.h"

#define REQUEST_RELEASE		0
#define REQUEST_MONITOR		1
#define UPDATE_STATUS		5

HWND hw_monitor = NULL;

BOOL CALLBACK FindXenimusWindowCallback(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		hw_monitor = hwnd;
		return FALSE;
	}
	return TRUE;
}

BOOL sendRequestToMonitor(HWND hwnd_monitor, int request) {

	WPARAM wParam;
	LPARAM lParam;
	COPYDATASTRUCT cds;
	UINT nMsg;
	BOOL res;

	wParam = (WPARAM)GetCurrentProcessId();

	if (request == UPDATE_STATUS) {
		std::string status = Bot::getInstance()->gamestatus->status;
		if (status.empty())
			status = " ? ";

		nMsg = WM_COPYDATA;
		cds.cbData = status.size();
		cds.lpData = (void *)status.c_str();

		lParam = (LPARAM)(LPVOID)&cds;
		// simple ping before data send
		res = PostMessage(hw_monitor, WM_USER+10, wParam, request);
		SendMessageTimeout(hw_monitor, nMsg, wParam, lParam, SMTO_ABORTIFHUNG, 200, 0);
	}
	else {
		nMsg = WM_USER;
		res = PostMessage(hw_monitor, nMsg, wParam, request);
	}
	
	if (FALSE == res) {
		log(E, "Error sending request message. Assuming XenMon was closed");
		log(E, GetLastErrorAsString());
		hw_monitor = NULL;
	}
	return res;
}

void XenMonConnect(HANDLE* botThread) {

	log(I, "XenMonConnect thread started");
	Bot * bot = Bot::getInstance();
	MSG msg;
	bool isMonitored = false;

	do {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {					// Exits thread
				log(I, "Exiting 'XenMon connect' thread.");
				return;
			}
		}
		Sleep(50);

		if (hw_monitor == NULL) {
			isMonitored = false;
			hw_monitor = FindWindow(NULL, "Xenimus Monitor");
		}
		else if (bot->isRunning() && !isMonitored) {
			// TODO confirmation message if it proves necessary
			if(sendRequestToMonitor(hw_monitor, REQUEST_MONITOR))
				isMonitored = true;
		}
		else if ((!bot->isRunning() || !bot->settings->keep_alive)
			&& isMonitored) {
			if (sendRequestToMonitor(hw_monitor, REQUEST_RELEASE))
				isMonitored = false;
		}
		else if (isMonitored) {
			sendRequestToMonitor(hw_monitor, UPDATE_STATUS);
		}

	} while (bot->settings->keep_alive);

	log(I, "'keep alive' disabled, exiting XenMonConnect thread.");
	(*botThread) = NULL;
}