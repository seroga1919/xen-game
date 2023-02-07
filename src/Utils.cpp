#include <Windows.h>
#include <shlwapi.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <ctime>
#include <tlhelp32.h>

#include "Utils.h"


std::string add_padding(std::string s, size_t len) {
	while (s.size()<len) {
		s.push_back(' ');
	}
	return s;
}


int getProcessIdFromName(std::string pName) {

	HANDLE hProcessSnap;
	int pid = 0;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return 0;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		std::string s(pe32.szExeFile);
		
		// TODO add a "no case" parameter
		/*for (int i = 0; i < s.size(); i++) {
			s[i] = tolower(s[i]);
		}*/

		if (s.find(pName, 0) != std::string::npos) {
			pid = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return pid;
}


double distanceBetween(int x1, int y1, int x2, int y2) {

	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}


bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	return (ftyp != INVALID_FILE_ATTRIBUTES) && (ftyp & FILE_ATTRIBUTE_DIRECTORY);
}


std::string GetExeFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}


std::string GetExePath()
{
	std::string f = GetExeFileName();
	return f.substr(0, f.find_last_of("\\/"));
}


void read_directory(std::string& name, char* file_extension, std::vector<std::string>* v)
{
	WIN32_FIND_DATA data;
	HANDLE hFind;
	std::string pattern(name);
	pattern.append("\\*.");
	pattern.append(file_extension);

	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v->push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}


// Returns the last Win32 error, in string format.
// Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}


std::string timeStamp()
{
	const time_t rawtime = std::time(0);
	struct tm * dt;
	char buffer[50];
	dt = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", dt);
	return std::string(buffer);
}
