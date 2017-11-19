#include "stdafx.h"
#include "StationResults.h"
#include "StringUtils.h"


// Results read in from the cabrillo file and used for regression testing
// Holds the results for a given station
StationResults::StationResults()
	:
	m_score(0),
    m_multipliers(0),
    m_qsoPoints(0),
    m_bonusPoints(0)
{
}

StationResults::~StationResults()
{
}

// Extract the key value pair and update the given data member
bool StationResults::ProcessKeyValue(const string& key, const string& value)
{
	if (key.empty())
	{
		printf("Error StationResults::ProcessKeyValue - key is empty\n");
		return false;
	}
	else if (value.empty())
	{
		printf("Error StationResults::ProcessKeyValue - value is empty\n");
		return false;
	}
	else if (StringUtils::IsInteger(value) == false)
	{
		printf("Error StationResults::ProcessKeyValue - value is not integer %s\n", value.c_str());
		return false;
	}

	if (key.compare("qplc-score") == 0)
		m_score = atoi(value.c_str());
	else if (key.compare("qplc-multipliers") == 0)
		m_multipliers = atoi(value.c_str());
	else if (key.compare("qplc-qsopoints") == 0)
		m_qsoPoints = atoi(value.c_str());
	else if (key.compare("qplc-bonuspoints") == 0)
		m_bonusPoints = atoi(value.c_str());
	else
		printf("Error: unknown station results: %s, %s\n", key.c_str(), value.c_str());

	return true;
}
