#pragma once

class Station;
class Qso;
class MultipliersPerMode;


// Base class for multipliers
class MultipliersBase
{
public:
	MultipliersBase();
	~MultipliersBase();

	virtual bool SetupMultipliers(Station *station) = 0;
	virtual bool FindMultipliers(vector<Qso>& m_qsos) = 0;
	virtual int  MultiplierPoints() const = 0;

	// The number of unique bonus stations worked
	virtual int  BonusStationsWorked() const { return 0; }

	// If bonus stations count for multipliers, add the bonus stations here
	virtual bool SetBonusStations(list<string>& bonusStations) = 0;

	// Return the number of dxcc multipliers 
	virtual int DxccMultipliers() const { return 0;  }

protected:
	Station *m_station;
	MultipliersPerMode *m_multipliersPerMode;
};

class MultipliersPerModeInstate : public MultipliersBase
{
public:
	virtual bool SetupMultipliers(Station *station) override;
	virtual bool FindMultipliers(vector<Qso>& m_qsos) override;
	virtual int  MultiplierPoints() const override;
	virtual int  BonusStationsWorked() const;
	virtual bool SetBonusStations(list<string>& bonusStations) override;
	virtual int DxccMultipliers() const override;
};

// Multipliers are per mode; cw, phone, digital
class MultipliersPerModeOutstate : public MultipliersBase
{
public:
	virtual bool SetupMultipliers(Station *station) override;
	virtual bool FindMultipliers(vector<Qso>& m_qsos) override;
	virtual int  MultiplierPoints() const override;
	virtual int  BonusStationsWorked() const;
	virtual bool SetBonusStations(list<string>& bonusStations) override;
};

