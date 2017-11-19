#include "stdafx.h"
#include "Multipliers.h"
#include "Station.h"
#include "Contest.h"
#include "Qso.h"
#include "Location.h"
#include "Mode.h"
#include "QsoItem.h"
#include "QsoInfo.h"
#include "StringUtils.h"
#include "MultipliersPerMode.h"


MultipliersBase::MultipliersBase()
{
	m_multipliersPerMode = new MultipliersPerMode();
}


MultipliersBase::~MultipliersBase()
{
	delete m_multipliersPerMode;
	m_multipliersPerMode = nullptr;
}


/////////////////////////////////////////////////////////////////
//
//   ***** Multipliers Per Mode In State
//
bool MultipliersPerModeInstate::SetupMultipliers(Station *station)
{
	bool status = true;
	m_station = station;
	Contest *contest = station->GetContest();

	set<string> multipliers;
	contest->GetInStateMultipliers(multipliers);

	m_multipliersPerMode->SetupMultipliers(station, multipliers);

	// Does this contest count dxcc countries as multipliers?
	if (station->InStateDxccMults())
	{
		auto *dxccmgr = station->GetDxccCountryManager();
		if (dxccmgr == nullptr)
		{
			printf("Error: dxcc multipliers active but no dxcc country manager :-(\n");
			status = false;
		}
		else
		{
			m_multipliersPerMode->SetDxccCountryManager(dxccmgr);
		}

	}

	return status;
}

bool MultipliersPerModeInstate::FindMultipliers(vector<Qso>& qsos)
{
	return m_multipliersPerMode->FindMultipliers(qsos);
}

int MultipliersPerModeInstate::MultiplierPoints() const
{
	return m_multipliersPerMode->MultiplierPoints();
}

int MultipliersPerModeInstate::BonusStationsWorked() const
{
	return m_multipliersPerMode->BonusStationsWorked();
}

bool MultipliersPerModeInstate::SetBonusStations(list<string>& bonusStations)
{
	return m_multipliersPerMode->SetBonusStations(bonusStations);
}

int MultipliersPerModeInstate::DxccMultipliers() const
{
	return m_multipliersPerMode->DxccMultipliers();
}

/////////////////////////////////////////////////////////////////
//
//   ***** Multipliers Per Mode Out State
//
bool MultipliersPerModeOutstate::SetupMultipliers(Station *station)
{
	m_station = station;
	Contest *contest = m_station->GetContest();

	set<string> multipliers;
	contest->GetCountyAbbrevs(multipliers);

	m_multipliersPerMode->SetupMultipliers(station, multipliers);

	return true;
}

bool MultipliersPerModeOutstate::FindMultipliers(vector<Qso>& qsos)
{
	return m_multipliersPerMode->FindMultipliers(qsos);
}

int MultipliersPerModeOutstate::MultiplierPoints() const
{
	return m_multipliersPerMode->MultiplierPoints();
}

int MultipliersPerModeOutstate::BonusStationsWorked() const
{
	return m_multipliersPerMode->BonusStationsWorked();
}

bool MultipliersPerModeOutstate::SetBonusStations(list<string>& bonusStations)
{
	return m_multipliersPerMode->SetBonusStations(bonusStations);
}
