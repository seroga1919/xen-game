#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//					Some tool samples                						//
///////////////////////////////////////////////////////////////////////////////

std::string add_padding(std::string s, size_t len);

int getProcessIdFromName(std::string pName);

double distanceBetween(int x1, int y1, int x2, int y2);

bool dirExists(const std::string& dirName_in);

std::string GetExeFileName();

std::string GetExePath();

void read_directory(std::string& name, char* file_extension, std::vector<std::string>* v);

std::string GetLastErrorAsString();

std::string timeStamp();
