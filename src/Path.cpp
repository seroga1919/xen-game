#include "Path.h"
#include <fstream>
#include <wx/string.h>
#include "Logger.h"

Path::Path() {
	followUpPathName = "";
}

void Path::setMerchantModelId(int id) {
	npc_merchant_modelId = id;
}

int	Path::getMerchantModelId() {
	return npc_merchant_modelId;
}

void Path::push_back(CPacket_Move p) {
	path.push_back(p);
}

void Path::pop_back() {
	if (!path.empty())
		path.pop_back();
}

void Path::add(int index, CPacket_Move p) {

	std::vector<CPacket_Move> new_path;

	for (int i = 0; i < (int)path.size(); i++) {
		if (i == index) {
			new_path.push_back(p);
		}
		new_path.push_back(path.at(i));
	}
	path = new_path;
}

void Path::remove(int index) {
	path.erase(path.begin() + index);
}

void Path::clear() {
	path.clear();
}

int Path::size() {
	return path.size();
}

bool Path::empty() {
	return path.empty();
}

CPacket_Move Path::back() {
	return path.back();
}

std::vector<CPacket_Move> Path::getPath() {
	return path;
}

CPacket_Move Path::getPoint(int index) {
	return path.at(index);
}

std::string Path::toString() {
	std::string s;
	s += "NPC merchant's id : " + std::to_string(npc_merchant_modelId) + "\n";
	s += "home portal : " + std::to_string(homePortal) + "\n";
	s += "hunting portal : " + std::to_string(huntingGroundPortal) + "\n";
	for (int i = 0; i < (int)path.size(); i++) {
		s += "<" + std::to_string(path.at(i).x) + " ; " + std::to_string(path.at(i).y) + ">\n";
	}
	return s;
}

std::vector<wxString> Path::to_wxStrings() {
	std::vector<wxString> res;
	wxString tmp;
	for (CPacket_Move p : path) {
		tmp = "    ";
		tmp += "<" + std::to_string(p.x) + " ; " + std::to_string(p.y) + ">";
		res.push_back(tmp);
	}
	return res;
}

void Path::saveToFile(char * filename) {

	int size = path.size();
	std::ofstream ofs(filename, std::ios::binary);
	
	ofs.write((char*)&npc_merchant_modelId, sizeof(int));
	ofs.write((char*)&homePortal, sizeof(int));
	ofs.write((char*)&huntingGroundPortal, sizeof(int));
	ofs.write((char*) &size, sizeof(int));

	for (int i = 0; i < size; i++) {
		ofs.write((char*)(void*)&(path.at(i)), sizeof(CPacket_Move));
	}

	size = followUpPathName.size();
	ofs.write((char*)&size, sizeof(int));

	for (int i = 0; i < size; i++) {
		ofs.write((char*)(void*)&(followUpPathName.at(i)), sizeof(char));
	}
	ofs.close();
}

bool Path::loadFromFile(char * filename) {

	int size;
	char c;
	CPacket_Move p;
	std::ifstream ifs(filename, std::ios::binary);

	if (!ifs) {
		return false;
	}
	
	ifs.read((char*)&npc_merchant_modelId, sizeof(int));
	ifs.read((char*)&homePortal, sizeof(int));
	ifs.read((char*)&huntingGroundPortal, sizeof(int));

	ifs.read((char*)&size, sizeof(int));
	path.clear();
	for (int i = 0; i < size; i++) {
		ifs.read((char*)&p, sizeof(CPacket_Move));
		path.push_back(p);
	}

	ifs.read((char*)&size, sizeof(int));
	followUpPathName.clear();
	for (int i = 0; i < size; i++) {
		ifs.read((char*)&c, sizeof(char));
		followUpPathName.push_back(c);
	}
	
	ifs.close();

	return true;
}
