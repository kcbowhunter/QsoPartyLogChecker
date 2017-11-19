#include "stdafx.h"
#include "CustomReport.h"
#include "StringUtils.h"
#include "boost/algorithm/string.hpp"
using namespace boost;

#include "TextFile.h"
#include "Station.h"



CustomReport::CustomReport()
	:
	m_fileName()
{
}


CustomReport::~CustomReport()
{
}


// Process the strings from the config file
bool CustomReport::ProcessConfigFile(vector<string>& text)
{
	bool status = true;
	size_t len = 0;
	char first, last;

	string key;
	string value;

	string keyLower;
	string errorMsg;

	string dataName;
	string columnHeading;

	// First: Process general key=value entries
	for (string temp : text)
	{
		if (temp.empty())
			continue;

		temp = StringUtils::RemoveComment(temp, '#');
		trim(temp);
		if (temp.empty()) continue;

		len = temp.size();
		first = temp.at(0);
		last = temp.at(len - 1);

		// Break and process Categories
		if (first == '[')
			break;

		size_t pos = temp.find('=');

		if (pos == string::npos)
		{
			printf("Error processing custom report file unknown line %s\n", temp.c_str());
			return false;
		}

		status = StringUtils::GetKeyValuePair(temp, key, value, errorMsg);
		keyLower = key;
		StringUtils::ToLower(keyLower);

		if (keyLower == "filename")
		{
			m_fileName = value;
		}
		else if (keyLower == "column")
		{
			pos = value.find(",");
			if (pos == string::npos)
			{
				printf("Error processing custom report file, unknown Column value: %s\n", value.c_str()); 
				return false;
			}
			size_t len = temp.size();
			dataName = value.substr(0, pos);
			columnHeading = value.substr(pos + 1, len - pos - 1);

			trim(dataName);
			trim(columnHeading);

			StringUtils::ToLower(dataName);

			m_columnData.push_back(dataName);
			m_columnHeading.push_back(columnHeading);
		}
	}

	// Second: Process Categories

	return true;
}


bool SortStationByScore(Station* s1, Station* s2)
{
	return s1->Score() < s2->Score();
}


// Generate the report in the results folder
bool CustomReport::GenerateReportForAllStations(const string& resultsFolder, vector<Station*>& stations)
{
	string fileName = resultsFolder + m_fileName;

	TextFile file;
	file.AddLine("Custom Report");
	file.AddLine(" ");

	string line;

	// Copy the stations to a local vector station sorting
	vector<Station*> stations2 = stations; 
	sort(stations2.begin(), stations2.end(), SortStationByScore);

	// Iterate over all the data members and build up the report string

	size_t count = m_columnData.size();
	size_t i = 0;

	// Write out the headings first
	string data, heading;
	for (i = 0; i < count; ++i)
	{
		heading = m_columnHeading[i];
		if (i == 0)
			line = heading;
		else
			line = heading + string(", ");
	}

	file.AddLine(line);

	line.clear();

	return true;
}
