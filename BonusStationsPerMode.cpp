#include "stdafx.h"
#include "BonusStationsPerMode.h"
#include "Qso.h"
#include "QsoInfo.h"
#include "StringUtils.h"


// Unique Bonus Stations Per Mode
BonusStationsPerMode::BonusStationsPerMode()
{
}


BonusStationsPerMode::~BonusStationsPerMode()
{
}

int BonusStationsPerMode::NumberOfBonusStations() const
{
	size_t x = m_cwBonusStations.size();
	size_t y = m_phoneBonusStations.size();
	size_t z = m_digitalBonusStations.size();

	return (int)(x + y + z);
}

// Return the number of bonus points awarded.
// Bonus points are only awarded when the bonus station has not been worked previously on this mode.
int BonusStationsPerMode::CalculateBonusPoints(const string& bonusStation, Qso& qso, const int bonusStationPoints)
{
	int points = 0;
	const Mode& mode = qso.GetMode();
	QsoMode qsomode = mode.GetMode();

	const string modeStringPhone("Phone");
	const string modeStringCw("CW");
	const string modeStringDigital("Digital");

	if (qsomode == eModePhone)
	{
		points = CalculateBonusPoints(m_phoneBonusStations, bonusStation, bonusStationPoints, modeStringPhone, qso);
	}
	else if (qsomode == eModeCw)
	{
		points = CalculateBonusPoints(m_cwBonusStations, bonusStation, bonusStationPoints, modeStringCw, qso);
	}
	else if (qsomode == eModeDigital)
	{
		points = CalculateBonusPoints(m_digitalBonusStations, bonusStation, bonusStationPoints, modeStringDigital, qso);
	}

	return points;
}


int BonusStationsPerMode::CalculateBonusPoints(set<string>& bonusStations, const string& bonusStation, const int bonusStationPoints, const string& modeName, Qso& qso)
{
	int points = 0;

	auto iter = bonusStations.find(bonusStation);

	if (iter == bonusStations.end())
	{
		bonusStations.insert(bonusStation);
		points = bonusStationPoints;

		QsoInfo *info = new QsoInfo();
		string bonusStationUpper(bonusStation);
		StringUtils::ToUpper(bonusStationUpper);
		char buffer[80];
		sprintf_s(buffer, 80, "New Bonus Station (%s): %s", modeName.c_str(), bonusStationUpper.c_str());
		info->m_msg = string(buffer);
		qso.AddQsoInfo(info);
	}

	return points;
}

