#include "stdafx.h"
#include "BonusStationsPerBandPerMode.h"
#include "BonusStationsPerMode.h"
#include "Qso.h"
#include "StringUtils.h"
#include "Freq.h"
#include "Mode.h"



BonusStationsPerBandPerMode::BonusStationsPerBandPerMode()
{
	m_bonusStationsPerMode.resize(eHamBandSize);
	auto iter    = m_bonusStationsPerMode.begin();
	auto iterend = m_bonusStationsPerMode.end();
	for (; iter != iterend; ++iter)
	{
		(*iter) = nullptr;
	}
}


BonusStationsPerBandPerMode::~BonusStationsPerBandPerMode()
{
	auto iter    = m_bonusStationsPerMode.begin();
	auto iterend = m_bonusStationsPerMode.end();
	for (; iter != iterend; ++iter)
	{
		auto ptr = *iter;
		if (ptr != nullptr)
		{
			delete ptr;
			*iter = nullptr;
		}
	}

	m_bonusStationsPerMode.clear();
}


// Calculate the bonus points for the given bonus station callsign
int BonusStationsPerBandPerMode::CalculateBonusPoints(const string& bonusStation, const int bonusStationPoints, vector<Qso>& qsos)
{
	int points = 0;
	string workedCallsign;
	string suffix;

	for (Qso& qso : qsos)
	{
		if (qso.IsDuplicate())
			continue;

		if (qso.ValidQso() && !qso.IsIgnored())
		{
			const Callsign& theirCallsign = qso.GetTheirCallsign();
			workedCallsign = theirCallsign.GetCallsign();
			suffix = theirCallsign.GetSuffix();
			StringUtils::ToLower(workedCallsign);

			if (bonusStation == workedCallsign)
			{
				BonusStationsPerMode *bonusStationsPerMode = GetBonusStationsPerMode(qso);

				// At this point we don't know if the bonus station has already been worked on this band and mode,
				// so this call may return 0 points
				points += bonusStationsPerMode->CalculateBonusPoints(bonusStation, qso, bonusStationPoints);
			}
		}
	}

	return points;
}

// Get the band for the qso and return the appropriate BonusStationsPerMode object
// Separate BonusStationsPerMode objects for each band
BonusStationsPerMode *BonusStationsPerBandPerMode::GetBonusStationsPerMode(Qso& qso)
{
	const Freq& freq = qso.GetFreq();
	HamBand band = freq.GetBand();

	BonusStationsPerMode *bonusStationsPerMode = m_bonusStationsPerMode[band];
	if (bonusStationsPerMode == nullptr)
	{
		bonusStationsPerMode = new BonusStationsPerMode();
		m_bonusStationsPerMode[band] = bonusStationsPerMode;
	}

	return bonusStationsPerMode;
}

// Return the number of bonus stations for all bands and all modes
int BonusStationsPerBandPerMode::NumberOfBonusStations() const
{
	int count = 0;

	auto iter = m_bonusStationsPerMode.begin();
	auto iterend = m_bonusStationsPerMode.end();
	for (; iter != iterend; ++iter)
	{
		auto ptr = *iter;
		if (ptr != nullptr)
		{
			count += ptr->NumberOfBonusStations();
		}
	}

	return count;
}
