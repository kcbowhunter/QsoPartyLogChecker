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

	// Generate custom reports for all stations in the results folder
	bool GenerateReportsForAllStations(const string& resultsFolder, vector<Station*>& stations);

	// Generate the custom reports in the results folder
	bool GenerateCustomReports(const string& resultsFolder, vector<Station*>& stations);

	// Determine the categories for each station in all custom reports
	void DetermineStationCategories(vector<Station*>& stations);

private:
	list<CustomReport*> m_customReports;
};

