#pragma once

class Station;
class Qso;
class DxccCountryManager;
class DxccCountry;
class Station;

class MultipliersPerMode
{
public:
	MultipliersPerMode();
	~MultipliersPerMode();

public:
	bool SetupMultipliers(Station *station, set<string>& multipliers);
	bool FindMultipliers(vector<Qso>& m_qsos);
	int  MultiplierPoints() const;
	int  BonusStationsWorked() const;

	// Is the given callsign a bonus station?
	bool IsBonusStation(const string& callsign);

	// Has the user specified bonus stations are multipliers and also 
	// provided bonus stations?
	bool HasBonusStations() const { return !m_bonusStations.empty(); }

	// If bonus stations count for multipliers, add the bonus stations here
	bool SetBonusStations(list<string>& bonusStations);

	bool DxccCountriesAreMultipliers() const { return m_dxcc; }

	// Setting the pointer to the DxccCountryManager activates dxcc countries as multipliers
	void SetDxccCountryManager(DxccCountryManager *dxccCountryManager);

	// Return the number of dxcc multipliers 
	int DxccMultipliers() const;

private:
	// All Multipliers for this station (based on in state or out of state)
	set<string> m_allMultipliers;

private:
	Station *m_station;
	// Callsigns of Bonus Stations that are score multipliers
	// (VT Qso Party)
	set<string> m_bonusStations;

	// The multipliers that worked by this station
	set<string> m_workedMultipliersCW;
	set<string> m_workedMultipliersPhone;
	set<string> m_workedMultipliersDigital;

	set<string> m_bonusStationsCW;
	set<string> m_bonusStationsPhone;
	set<string> m_bonusStationsDigital;

	DxccCountryManager *m_dxccCountryManager;
	bool m_dxcc; // dxcc countries are multipliers
	bool m_dxMultiplier;  // contest allows one multiplier for DX
	set<int> m_dxccCountriesWorkedCW;
	set<int> m_dxccCountriesWorkedPhone;
	set<int> m_dxccCountriesWorkedDigital;

	bool CheckMultiplier(set<string>& workedMultipliers, Qso& qso, const string& location, const string& modeName);
	bool CheckBonusStation(set<string>& bonusStations, Qso& qso, const string& callsign, const string& modeName);
	bool CheckDxccMultiplier(Qso& qso, DxccCountry *country);
	bool CheckDxccMultiplier2(Qso& qso, DxccCountry *country, set<int>& dxccCountries, const string& modeName);

};

