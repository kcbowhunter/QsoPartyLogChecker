#pragma once

#include "stdafx.h"
class Station;

// Class to Generate Custom Reports
class CustomReport
{
public:
	CustomReport();
	~CustomReport();

	// Process the strings from the config file
	bool ProcessConfigFile(vector<string>& text);

	// Generate the report in the results folder
	bool GenerateReportForAllStations(const string& resultsFolder, vector<Station*>& stations);

private:
	std::map<string, string> m_keyValuePairs;
	vector<string> m_columnData;
	vector<string> m_columnHeading;
	string m_fileName;  // output file name for the report
};

