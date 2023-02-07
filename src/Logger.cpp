#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <ctime>

#include "Logger.h"
#include "Utils.h"
#include "Bot.h"

std::ofstream				Logger::log_file;
wxStyledTextCtrl *			Logger::gui_output;
std::vector<std::string> *	Logger::buff;

std::mutex buff_lock;


// Letting know a potential calling process that we exited
// from a user interaction
BOOL WINAPI consoleHandler(DWORD signal) {
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
		Bot::getInstance()->stop();
		exit(USER_EXIT);
	}
	return TRUE;
}


void Logger::show_bootstrap_console(char * title)
{
	AllocConsole();
	SetConsoleTitle(title);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
	std::cout << std::endl;
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		log("ERROR: Could not set control handler");
	}
}


void Logger::add_log(std::string str) {
	add_log(str.c_str());
}


void Logger::add_log(LOG_LEVEL level, const char * fmt) {
	std::string str = std::string(fmt);
	add_log(level, str);
}


void Logger::add_log(LOG_LEVEL level, std::string str) {
	switch (level)  {
	case INFO:
		str = "[+] " + str;
		break;
	case WARN:
		str = "[?] " + str;
		break;
	case ERR:
		str = "[!] " + str;
		break;
	default:
		str = "[*] " + str;
		break;
	};
	
	add_log(timeStamp() + " " + str.c_str());
}


void Logger::add_log(const char * fmt) {

	std::string outPut = std::string(fmt) + "\n";

	// Simple console
	std::cout << outPut.c_str();
	// UI's activity log
	if (gui_output != NULL) {
		buff_lock.lock();
		buff->push_back(outPut);
		buff_lock.unlock();
	}
	// Log file
	if (log_file.is_open()) {
		log_file << outPut.c_str();
		log_file.flush();
	}
}


void Logger::setGUIOutPut(wxStyledTextCtrl * textField) {
	gui_output = textField;
	buff = new std::vector<std::string>();
	log(I, "Logger: GUI output set");
}


std::vector<std::string> Logger::getOutputBuff() {
	std::vector<std::string> * newBuff = new std::vector<std::string>();
	std::vector<std::string> * res = buff;
	buff = newBuff;
	return *res;
}


bool Logger::setFileOutput(char * filename) {

	std::string file = GetExePath();
	file += "\\";
	file += BASE_BOT_DIR;

	const time_t time = std::time(0);
	struct tm * dt;
	char buffer[50];
	dt = localtime(&time);

	strftime(buffer, sizeof(buffer), "%Y_%m_%d - %H_%M_%S - activity.log", dt);
	file += "\\logs\\" + std::string(buffer);
	log_file.open(file.c_str());
	
	return log_file.is_open();
}
