#pragma once

class BonusStationsPerMode;
class Qso;

class BonusStationsPerBandPerMode
{
public:
	BonusStationsPerBandPerMode();
	~BonusStationsPerBandPerMode();

	// Calculate the bonus points for the given bonus station callsign
	int CalculateBonusPoints(const string& bonusStation, const int bonusStationPoints, vector<Qso>& qsos);

	// Return the number of bonus stations for all bands and all modes
	int NumberOfBonusStations() const;

private:
	// Bonus Stations per mode for each band
	vector<BonusStationsPerMode*> m_bonusStationsPerMode;

	BonusStationsPerMode *GetBonusStationsPerMode(Qso& qso);
};

