#pragma once

// This class holds results used for regression testing
// Results are read in from the Cabrillo file using the following Cabrillo extensions:
// QPLC-SCORE: 30
// QPLC-MULTIPLIERS : 3
// QPLC-QSOPOINTS : 10
// QPLC-BONUSPOINTS : 0

class StationResults
{
public:
	StationResults();
	~StationResults();

public:
	int m_score;
	int m_multipliers;
	int m_qsoPoints;
	int m_bonusPoints;

public:
	// Extract the key value pair and update the given data member
	bool ProcessKeyValue(const string& key, const string& value);
};

