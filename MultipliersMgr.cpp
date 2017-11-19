#include "stdafx.h"
#include "MultipliersMgr.h"
#include "Multipliers.h"
#include "Station.h"
#include "Qso.h"
#include "Contest.h"


MultipliersMgr::MultipliersMgr()
	:
	m_multipliersType(eMultUnknown),
	m_station(nullptr),
	m_instate(false),
	m_multipliersBase(nullptr)
{
}


MultipliersMgr::~MultipliersMgr()
{
}

// Setup the Multipliers
bool MultipliersMgr::SetupMultipliers(Station *station, MultipliersType multType)
{
	m_station = station;
	m_instate = station->InState();
	m_multipliersType = multType;

	Contest *contest = station->GetContest();
	bool status = false;
	if (m_multipliersType == eMultPerMode)
	{
		if (m_instate)
		{
			m_multipliersBase = new MultipliersPerModeInstate();
		}
		else
		{
			m_multipliersBase = new MultipliersPerModeOutstate();
		}
	}

	if (m_multipliersBase != nullptr)
	{
		status = m_multipliersBase->SetupMultipliers(m_station);

		// Are bonus stations multipliers?
		bool bonusStationMults = contest->GetBonusStationMultipliers();
		if (bonusStationMults)
		{
			list<string> bonusStations = contest->GetBonusStations();
			m_multipliersBase->SetBonusStations(bonusStations);
		}
	}

	return status;
}


bool MultipliersMgr::FindMultipliers(vector<Qso>& qsos)
{
	bool status = false;
	if (m_multipliersBase != nullptr)
	{
		status = m_multipliersBase->FindMultipliers(qsos);
	}

	return status;
}

int MultipliersMgr::MultiplierPoints() const
{
	if (m_multipliersBase != nullptr)
	{
		return m_multipliersBase->MultiplierPoints();
	}

	return 0;
}

int MultipliersMgr::DxccMultipliers() const
{
	if (m_multipliersBase != nullptr)
	{
		return m_multipliersBase->DxccMultipliers();
	}

	return 0;
}

// The number of unique bonus stations worked
int MultipliersMgr::BonusStationsWorked() const
{
	if (m_multipliersBase != nullptr)
	{
		return m_multipliersBase->BonusStationsWorked();
	}

	return 0;
}
