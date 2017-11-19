#pragma once

#include "stdafx.h"

class CustomReport;
class Station;

// Class to manage Custom Reports
class CustomReportManager
{
public:
	CustomReportManager();
	virtual ~CustomReportManager();

	// Create a custom report based off of the given report config file name
	bool CreateCustomReport(const string& configFileName);

	// Create custom reports
	bool CreateCustomReports(const string& configFilesFolder, list<string>& customReportConfigFilenames);

	// Generate the custom reports in the results folder
	bool GenerateReportsForAllStations(const string& resultsFolder, vector<Station*>& stations);

private:
	list<CustomReport*> m_customReports;
};

