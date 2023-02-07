#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XenGUI.h"

#define I		INFO
#define E		ERR
#define W		WARN
#define log		Logger::add_log

enum LOG_LEVEL {INFO, ERR, WARN};

class Logger {
private:
	static std::ofstream				log_file;
	static wxStyledTextCtrl *			gui_output;
	static std::vector<std::string> *	buff;

public:
	static void show_bootstrap_console(char * title);
	static void add_log(std::string str);
	static void add_log(LOG_LEVEL level, const char * fmt);
	static void add_log(LOG_LEVEL level, std::string str);
	static void add_log(const char * fmt);

	static void setGUIOutPut(wxStyledTextCtrl * textField);
	static bool setFileOutput(char * filename);
	static std::vector<std::string>getOutputBuff();
};
