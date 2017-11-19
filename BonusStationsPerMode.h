#pragma once

class Qso;

// Unique Bonus Stations Per Mode
class BonusStationsPerMode
{
public:
	BonusStationsPerMode();
	~BonusStationsPerMode();

	// Return the number of bonus stations for this mode
	int NumberOfBonusStations() const;

	// Return the number of bonus points awareded.
	// Bonus points are only awarded when the bonus station has not been worked previously on this mode.
	int CalculateBonusPoints(const string& bonusStation, Qso& qso, const int bonusStationPoints);

private:
	// Bonus station callsigns are always stored lower case
	set<string> m_cwBonusStations;
	set<string> m_phoneBonusStations;
	set<string> m_digitalBonusStations;

	int CalculateBonusPoints(set<string>& bonusStations, const string& bonusStation, const int bonusStationPoints, const string& modeName, Qso& qso);
};

