#include "stdafx.h"
#include "CustomReportManager.h"
#include "CustomReport.h"
#include "TextFile.h"

CustomReportManager::CustomReportManager()
{
}


CustomReportManager::~CustomReportManager()
{
	for (auto pReport : m_customReports)
	{
		delete pReport;
	}
	m_customReports.clear();
}


// Create a custom report based off of the given report config file name
bool CustomReportManager::CreateCustomReport(const string& configFileName)
{
	bool status = true;

	TextFile configFile;
	status = configFile.Read(configFileName);

	if (!status)
	{
		printf("Error in CustomReportManager::CreateCustomReport: Unable to read file %s\n", configFileName.c_str());
		return false;
	}

	vector<string> text;
	configFile.GetLines(text);

	CustomReport *customReport = new CustomReport();

	status = customReport->ProcessConfigFile(text);
	if (!status)
	{
		printf("Error in CustomReportManager creating custom report %s\n", configFileName.c_str());
		return false;
	}

	m_customReports.push_back(customReport);

	return status;
}


// Create custom reports
bool CustomReportManager::CreateCustomReports(const string& configFilesFolder, list<string>& customReportConfigFilenames)
{
	string filename;
	bool status = true;
	for (string configFileName : customReportConfigFilenames)
	{
		filename = configFilesFolder + configFileName;
		status = CreateCustomReport(filename);
		if (!status)
		{
			printf("Error creating custom report: %s\n", configFileName.c_str());
			return false;
		}
	}

	return true;
}

// Generate the custom reports in the results folder
bool CustomReportManager::GenerateReportsForAllStations(const string& resultsFolder, vector<Station*>& stations)
{
	for (CustomReport* customReport : m_customReports)
	{
		customReport->GenerateReportForAllStations(resultsFolder, stations);
	}

	return true;
}
