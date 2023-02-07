#pragma once
#include <vector>
#include <Windows.h>
#include <string>
#include <wx/string.h>
#include "ClientPackets.h"

class Path
{
private:
	int npc_merchant_modelId;
	int homePortal;
	int huntingGroundPortal;
	std::vector<CPacket_Move> path;
	std::string followUpPathName;

public:
						Path();
	void				setMerchantModelId(int id);

	void				setHomePortal(int id) { homePortal = id; }
	void				setHuntingGroundPortal(int id) { huntingGroundPortal = id; }
	void				setFollowUpPathName(std::string name) { followUpPathName = name; }

	int					getMerchantModelId();
	int					getHomePortal() { return homePortal; }
	int					getHuntingGroundPortal() { return huntingGroundPortal; }
	std::string			getFollowUpPathName() { return followUpPathName; }

	void				push_back(CPacket_Move p);
	void				pop_back();
	void				add(int index, CPacket_Move p);
	void				remove(int index);
	void				clear();
	int					size();
	bool				empty();
	CPacket_Move		back();
	CPacket_Move		getPoint(int index);
	std::vector<CPacket_Move>	getPath();
	
	std::string			toString();
	std::vector<wxString> to_wxStrings();
	void				saveToFile(char * filename);
	bool				loadFromFile(char * filename);
};