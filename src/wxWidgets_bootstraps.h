#pragma once

// The graphic interface is made with the wxWidget library
// The initialisation process will be triggered by this
// function.
// If you want to modify something, you should only need to
// touch the UI class
// I mostly re-used the code from:
//			wxWidgets-3.1.2\samples\dll\my_dll.cpp

void run_wx_gui_from_dll(const char *title, HINSTANCE hInst);