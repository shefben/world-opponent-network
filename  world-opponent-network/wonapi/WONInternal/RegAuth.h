
#if !defined(REG_AUTH_H)
#define REG_AUTH_H

#include <string>
#include "WONCrypt/Blowfish.h"

namespace WONAPI
{

	const unsigned char gKeyData[8] = { 0x23, 0x77, 0xCF, 0x67, 0x99, 0xAB, 0xBA, 0xE5 };
	const Blowfish      gPasswordKey(gKeyData, 8);

	const std::string KEY_TOOLS("Software\\Sierra\\MPTools");
	const std::string KEY_TOOLS_AUTH(KEY_TOOLS + "\\Auth");
	const std::string VALUE_AUTHSERVERIP("AuthServerIP");
	const std::string VALUE_AUTHSERVERPORT("AuthServerPort");
	const std::string VALUE_USERNAME("UserName");
	const std::string VALUE_COMMUNITY("Community");
	const std::string VALUE_PASSWORD("Password");

	bool ReadAuthSettings(std::string& theAuthAddressR, unsigned short& theAuthPortR, std::string& theLoginNameR, std::string& theCommunityNameR, std::string& thePasswordR);
	bool WriteAuthSettings(const std::string& theAuthAddress, unsigned short theAuthPort, const std::string& theLoginName, const std::string& theCommunityName, const std::string& thePassword);

	bool ReadAuthSettings(std::string& theLoginNameR, std::string& theCommunityNameR, std::string& thePasswordR);
	bool WriteAuthSettings(const std::string& theLoginName, const std::string& theCommunityName, const std::string& thePassword);

};

#endif