#include "stdafx.h"
#include "MultipliersPerMode.h"

#include "Qso.h"
#include "QsoInfo.h"
#include "Location.h"
#include "StringUtils.h"
#include "DxccCountryManager.h"
#include "DxccCountry.h"
#include "Station.h"
#include "ContestConfig.h"

#include "boost/algorithm/string.hpp"
using namespace boost;

MultipliersPerMode::MultipliersPerMode()
	:
	m_dxcc(false),
	m_dxccCountryManager(nullptr),
	m_station(nullptr),
	m_dxMultiplier(true)
{
}

MultipliersPerMode::~MultipliersPerMode()
{
	m_dxccCountryManager = nullptr;  // do not delete
}

bool MultipliersPerMode::SetupMultipliers(Station *station, set<string>& multipliers)
{
	m_station = station;

	if (multipliers.empty())
		return false;

	m_allMultipliers = multipliers;

	m_dxMultiplier = station->GetContestConfig()->GetDxMultiplier();

	return true;
}

// Setting the pointer to the DxccCountryManager activates dxcc countries as multipliers
void MultipliersPerMode::SetDxccCountryManager(DxccCountryManager *dxccCountryManager)
{
	m_dxccCountryManager = dxccCountryManager;
	m_dxcc = true;
}


bool MultipliersPerMode::FindMultipliers(vector<Qso>& qsos)
{
	bool status = true;
	string modeNamePhone("Phone");
	string modeNameCW("CW");
	string modeNameDigital("Digital");
	string workedCallsign;
	string suffix;
	bool hasBonusStations = HasBonusStations(); // are multiplier bonus stations specified?
	bool isBonusStation = false;
	DxccCountry *country = nullptr;

//  This is needed to solve the KG4ABC FL problem (KG4 is in Florida, not Guantonamo Bay)
	DxccCountry *usa = m_dxccCountryManager == nullptr ? nullptr : m_dxccCountryManager->FindCountryByName("USA");
	string prefix;

	bool isUSA = false;
	bool isCanada = false;
	int  usaCount = 0;

	for (Qso& qso : qsos)
	{
		if (qso.ValidQso() && !qso.IsIgnored())
		{
			isUSA = false;
			isCanada = false;
			country = nullptr;
			const Location *theirLocation = qso.GetTheirLocation();
			string location = theirLocation->GetValue();
			auto iter = m_allMultipliers.find(location);

			const Callsign& theirCallsign = qso.GetTheirCallsign();
			workedCallsign = theirCallsign.GetCallsign();
			suffix = theirCallsign.GetSuffix();
			StringUtils::ToLower(workedCallsign);

			// Fix the KG4ABC 599 FL problem
			if (usa != nullptr && usa->Match(workedCallsign, usaCount) && iter != m_allMultipliers.end())
				isUSA = true;

			// Fix the VP9IT/W4  599 FL problem
			if (!isUSA && usa != nullptr && !suffix.empty() && suffix != "m" && suffix != "qrp")
			{
				if (usa->Match(suffix, usaCount) && iter != m_allMultipliers.end())
					isUSA = true;
			}

			if (!isUSA)
    			country = m_dxccCountryManager == nullptr ? nullptr : m_dxccCountryManager->FindCountry(workedCallsign);

			// If the station is a USA callsign and the location is in the State / Province / County list, it is not dx
			if (country != nullptr && country->GetName() == "USA" && iter != m_allMultipliers.end())
				isUSA = true;

			if (country != nullptr && country->GetName() == "Canada" && iter != m_allMultipliers.end())
				isCanada = true;

			if (isUSA || isCanada)
			{
			}
			else if (country != nullptr && country->IsDxStation())
			{
				string countryName = country->GetName();
				//					printf("MultipliersPerMode:: Callsign %10s maps to country %s\n", workedCallsign.c_str(), countryName.c_str());

				CheckDxccMultiplier(qso, country);
				continue;
			}

			if (isUSA || isCanada)
			{ }
			else if (m_dxccCountryManager != nullptr)
			{
				// Check the prefix
				prefix = theirCallsign.GetPrefix();
				if (!prefix.empty())
				{
					StringUtils::ToLower(prefix);
					country = m_dxccCountryManager->FindCountry(prefix);
					if (country != nullptr && country->IsDxStation())
					{
						CheckDxccMultiplier(qso, country);
						continue;
					}
				}

				// Check the suffix
				prefix = theirCallsign.GetSuffix();
				if (!prefix.empty())
				{
					StringUtils::ToLower(prefix);
					country = m_dxccCountryManager->FindCountry(prefix);
					if (country != nullptr && country->IsDxStation())
					{
						CheckDxccMultiplier(qso, country);
						continue;
					}
				}

				// Check the location
				prefix = location;
				if (!prefix.empty())
				{
					StringUtils::ToLower(prefix);
					country = m_dxccCountryManager->FindCountry(prefix);
					if (country != nullptr && country->IsDxStation())
					{
						CheckDxccMultiplier(qso, country);
						continue;
					}
				}

			}

			isBonusStation = hasBonusStations && IsBonusStation(workedCallsign);


			if (iter == m_allMultipliers.end())
			{
				status = false;
				// Internal error, to be a valid qso this should not happen
				printf("*** Internal Error *** -> MultipliersPerMode::FindMultipliers unknown location %3s callsign %s\n", location.c_str(), workedCallsign.c_str());
				continue;
			}

			Mode mode = qso.GetMode();
			QsoMode qsoMode = mode.GetMode();
			if (qsoMode == eModePhone)
			{
				CheckMultiplier(m_workedMultipliersPhone, qso, location, modeNamePhone);
				if (isBonusStation)
					CheckBonusStation(m_bonusStationsPhone, qso, workedCallsign, modeNamePhone);
			}
			else if (qsoMode == eModeCw)
			{
				CheckMultiplier(m_workedMultipliersCW, qso, location, modeNameCW);
				if (isBonusStation)
					CheckBonusStation(m_bonusStationsCW, qso, workedCallsign, modeNameCW);
			}
			else if (qsoMode == eModeDigital)
			{
				CheckMultiplier(m_workedMultipliersDigital, qso, location, modeNameDigital);
				if (isBonusStation)
					CheckBonusStation(m_bonusStationsDigital, qso, workedCallsign, modeNameDigital);
			}
			else
			{
				status = false;
				printf("Internal Error MultipliersPerModeOutstate: mode unknown\n");
			}
		}
	}

	return status;
}


// Is the given callsign a bonus station?
// Note that m_bonusStations only has entries when bonus stations are multipliers
bool MultipliersPerMode::IsBonusStation(const string& callsign)
{
	if (m_bonusStations.empty())
		return false;

	string c(callsign);
	StringUtils::ToLower(c);

	auto iter = m_bonusStations.find(c);
	bool status = iter != m_bonusStations.end();
	return status;
}

bool MultipliersPerMode::SetBonusStations(list<string>& bonusStations)
{
	for (auto s : bonusStations)
	{
		StringUtils::ToLower(s);
		m_bonusStations.insert(s);
	}

	return true;
}

int MultipliersPerMode::MultiplierPoints() const
{
	// x is the number of state / province / county multipliers
	size_t x = m_workedMultipliersCW.size() + m_workedMultipliersPhone.size() + m_workedMultipliersDigital.size();
	size_t y = 0;
	size_t z = 0;

	bool hasBonusStations = HasBonusStations(); // are multiplier bonus stations specified?
	if (hasBonusStations)
		y = m_bonusStationsCW.size() + m_bonusStationsPhone.size() + m_bonusStationsDigital.size();

	if (m_dxcc)
	{
		z = DxccMultipliers();
	}

	return (int)(x + y + z);
}

// Return the number of dxcc multipliers 
int MultipliersPerMode::DxccMultipliers() const
{
	size_t z = 0;
	// if dxcc countries are multipliers for this contest add them in to the total mults
	if (m_dxcc)
	{
		z = m_dxccCountriesWorkedCW.size() + m_dxccCountriesWorkedPhone.size() + m_dxccCountriesWorkedDigital.size();
	}

	return (int)z;
}

// Return the number of bonus stations worked for all modes
int MultipliersPerMode::BonusStationsWorked() const
{
	size_t x = m_bonusStationsCW.size() + m_bonusStationsPhone.size() + m_bonusStationsDigital.size();

	return (int)x;
}

bool MultipliersPerMode::CheckMultiplier(set<string>& workedMultipliers, Qso& qso, const string& location, const string& modeName)
{
	// If this contest does not support DX as a multiplier, filter out the "dx" location
	if (!m_dxMultiplier)
	{
		if (boost::iequals(location, "dx"))
		{
			return true;
		}
	}

	auto iter = workedMultipliers.find(location);
	if (iter == workedMultipliers.end())
	{
		workedMultipliers.insert(location);
		QsoInfo *info = new QsoInfo();
		string locUpper(location);
		StringUtils::ToUpper(locUpper);
		char buffer[80];
		sprintf_s(buffer, 80, "New Multiplier (%s): %s", modeName.c_str(), locUpper.c_str());
		info->m_msg = string(buffer);
		qso.AddQsoInfo(info);
	}

	// If this is a county, include the state multiplier for instate stations
	if (m_station->InState())
	{
		string state = m_station->GetContestStateAbbrev();
		const set<string> &countyAbbrevs = m_station->GetCountyAbbrevs();

		// Is it a county?
		auto iter = countyAbbrevs.find(location);
		if (iter != countyAbbrevs.end())
		{
			auto iter = workedMultipliers.find(state);
			if (iter == workedMultipliers.end())
			{
				workedMultipliers.insert(state);
				string stateUpper(state);
				StringUtils::ToUpper(stateUpper);
				char buffer[80];
				sprintf_s(buffer, 80, "New Multiplier (%s): %s", modeName.c_str(), stateUpper.c_str());
				QsoInfo *info = new QsoInfo();
				info->m_msg = string(buffer);
				qso.AddQsoInfo(info);
			}
		}
	}
	return true;
}

bool MultipliersPerMode::CheckDxccMultiplier(Qso& qso, DxccCountry *country)
{
	string modeNamePhone("Phone");
	string modeNameCW("CW");
	string modeNameDigital("Digital");

	bool status = false;
	Mode mode = qso.GetMode();
	QsoMode qsoMode = mode.GetMode();

	if (qsoMode == eModePhone)
	{
		status = CheckDxccMultiplier2(qso, country, m_dxccCountriesWorkedPhone, modeNamePhone);
	}
	else if (qsoMode == eModeCw)
	{
		status = CheckDxccMultiplier2(qso, country, m_dxccCountriesWorkedCW, modeNameCW);
	}
	else if (qsoMode == eModeDigital)
	{
		status = CheckDxccMultiplier2(qso, country, m_dxccCountriesWorkedDigital, modeNameDigital);
	}


	return true;
}

bool MultipliersPerMode::CheckDxccMultiplier2(Qso& qso, DxccCountry *country, set<int>& dxccCountriesWorked, const string& modeName)
{
	int countryCode = country->GetNumber();
	string countryName = country->GetName();

	auto iter = dxccCountriesWorked.find(countryCode);

	if (iter == dxccCountriesWorked.end())
	{
		dxccCountriesWorked.insert(countryCode);
		QsoInfo *info = new QsoInfo();
		char buffer[80];
		sprintf_s(buffer, 80, "New DXCC Multiplier (%s): %s", modeName.c_str(), countryName.c_str());
		info->m_msg = string(buffer);
		qso.AddQsoInfo(info);
	}

	return true;
}

bool MultipliersPerMode::CheckBonusStation(set<string>& bonusStations, Qso& qso, const string& callsign, const string& modeName)
{
	auto iter = bonusStations.find(callsign);

	if (iter == bonusStations.end())
	{
		bonusStations.insert(callsign);
		QsoInfo *info = new QsoInfo();
		string c(callsign);
		StringUtils::ToUpper(c);
		char buffer[80];
		sprintf_s(buffer, 80, "New Bonus Station Multiplier (%s): %s", modeName.c_str(), c.c_str());
		info->m_msg = string(buffer);
		qso.AddQsoInfo(info);
	}

	return true;
}
