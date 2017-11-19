
#include "stdafx.h"
#include "CheckDxccCountry.h"
#include "DxccCountryManager.h"
#include "DxccCountry.h"
#include "StringUtils.h"


bool CheckDxccCountry(string arg, DxccCountryManager* dxccCountryManager)
{
	string key;
	string callsign;
	string errorMsg;
	bool status = StringUtils::GetKeyValuePair(arg, key, callsign, errorMsg);
	if (!status)
	{
		printf("Error in CheckDxccCountry: %s\n", errorMsg.c_str());
		return false;
	}

	string country;
	status = dxccCountryManager->FindCountryName(callsign, country);

	if (status)
	{
		printf("Callsign '%s' corresponds to DXCC country '%s'\n", callsign.c_str(), country.c_str());
	}
	else
	{
		printf("Unable to locate country for callsign '%s'\n", callsign.c_str());
	}

	return status;
}