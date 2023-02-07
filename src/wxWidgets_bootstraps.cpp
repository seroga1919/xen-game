#include <windows.h>
#include <process.h>

#include "wxWidgets_bootstraps.h"
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "wx/msw/wrapwin.h"
#include "XenGUI.h"

#include "Bot.h"
#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////
class UI : public wxApp
{
private:
	XenGUI * gui;
public:
	virtual bool OnInit();
	virtual int OnExit();
	int FilterEvent(wxEvent& event);
};

bool UI::OnInit() {
	gui = new XenGUI(0);
	gui->Show(true);
	return true;
}

int UI::OnExit() {
	FreeConsole();
	Bot::getInstance()->onExit();
	Sleep(300);
	exit(USER_EXIT);
	return 0;
}

// Transforming DIK_ codes into wxWidgets codes.
// Currenly only works for F5.
int toForegroundKey() {
	int key = -1;
	Bot * bot = Bot::getInstance();
	if (bot != NULL) {
		switch (bot->settings->toForegroundKey) {
		case DIK_F5:
			key = WXK_F5;
			break;
		}
	}
	return key;
}

// Those are application-wide events, filtered before being sent
// to the different wxWidgets components.
int UI::FilterEvent(wxEvent& event)
{
	// Currently only one event monitored
	if (event.GetEventType() == wxEVT_KEY_DOWN
		&& ((wxKeyEvent&)event).GetKeyCode() == toForegroundKey()
		)
	{
		Bot * bot = Bot::getInstance();
		if (bot != NULL) {
			bot->gameToForeground();
		}
		return true;
	}

	return -1;
}
///////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_APP_NO_MAIN(UI);

HINSTANCE			hInstance;
HANDLE				gs_wxMainThread = NULL;
wxCriticalSection	gs_wxStartupCS;

static const int CMD_SHOW_WINDOW = wxNewId();
static const int CMD_TERMINATE = wxNewId();

//  wx application startup code -- runs from its own thread
unsigned wxSTDCALL MyAppLauncher(void* event)
{
	if (!hInstance)
		return 0;
	wxDISABLE_DEBUG_SUPPORT();

	wxInitializer wxinit;
	if (!wxinit.IsOk())
		return 0;

	HANDLE hEvent = *(static_cast<HANDLE*>(event));
	if (!SetEvent(hEvent))
		return 0; // failed setting up the mutex

				  // Run the app:
	wxEntry(hInstance);

	return 1;
}


void run_wx_gui_from_dll(const char *title, HINSTANCE hInst)
{
	hInstance = hInst;
	wxCriticalSectionLocker lock(gs_wxStartupCS);

	if (!gs_wxMainThread)
	{
		HANDLE hEvent = CreateEvent
		(
			NULL,  // default security attributes
			FALSE, // auto-reset
			FALSE, // initially non-signaled
			NULL   // anonymous
		);
		if (!hEvent)
			return; // error

		gs_wxMainThread = (HANDLE)_beginthreadex
		(
			NULL,           // default security
			0,              // default stack size
			&MyAppLauncher,
			&hEvent,        // arguments
			0,              // create running
			NULL
		);

		if (!gs_wxMainThread)
		{
			CloseHandle(hEvent);
			return; // error
		}

		// Wait until MyAppLauncher signals us that wx was initialized. This
		// is because we use wxMessageQueue<> and wxString later and so must
		// be sure that they are in working state.
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}

	// Send a message to wx thread to show a new frame:
	wxThreadEvent *event =
		new wxThreadEvent(wxEVT_THREAD, CMD_SHOW_WINDOW);
	event->SetString(title);
	wxQueueEvent(wxApp::GetInstance(), event);
}
