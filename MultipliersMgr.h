#pragma once

// Calculates Multipliers for a single station
enum MultipliersType { eMultPerContest, eMultPerMode, eMultPerBand, eMultUnknown };

class Qso;
class Station;
class MultipliersBase;

class MultipliersMgr
{
public:
	MultipliersMgr();
	~MultipliersMgr();

	MultipliersType GetMultipliersType() const { return m_multipliersType; }
	void            SetMultipliersType(const MultipliersType m) { m_multipliersType = m; }
	int   MultiplierPoints() const;

	// Setup the Multipliers
	bool SetupMultipliers(Station *station, MultipliersType multType);

	bool FindMultipliers(vector<Qso>& qsos);

	// The number of unique bonus stations worked
	int  BonusStationsWorked() const;

	// Return the number of dxcc multipliers
	int DxccMultipliers() const;
private:
	MultipliersType m_multipliersType;
	Station *m_station;
	bool m_instate;  // instate station

	MultipliersBase *m_multipliersBase;
};

