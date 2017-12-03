#include "stdafx.h"
#include "CustomReportManager.h"
#include "CustomReport.h"
#include "TextFile.h"
#include "CategoryMgr.h"
#include "Category.h"
#include "Station.h"

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

	status = customReport->ProcessCategories(configFileName);
	if (!status)
	{
		printf("Error in CustomReportManager creating categories for custom report %s\n", configFileName.c_str());
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
		TextFile file;
		file.AddLine("Custom Report");
		file.AddLine(" ");

		customReport->GenerateReport(stations, file);

		string reportFileName = customReport->GetFileName();
		string fileName = resultsFolder + string("AllStations_") + reportFileName;

		file.Write(fileName);
	}

	return true;
}

// Generate the custom reports in the results folder
bool CustomReportManager::GenerateCustomReports(const string& resultsFolder, vector<Station*>& stations)
{
	// Create a set of Station* to keep track of stations that are not in any category
	set<Station*> allStations(stations.begin(), stations.end());

	for (CustomReport* customReport : m_customReports)
	{
		TextFile file;
		file.AddLine("Custom Report");
		file.AddLine(" ");

		CategoryMgr *catMgr = customReport->GetCategoryMgr();

		// Get a copy of the list of categories
		list<Category*> categories;
		catMgr->GetCategories(categories);
		string categoryTitle;

		for (Category* category : categories)
		{
			list<Station*>& catStations = category->m_stations;

			if (catStations.empty())
				continue;

			for (Station *s : catStations)
				allStations.erase(s);

			categoryTitle = category->m_title;
			if (!categoryTitle.empty())
				file.AddLine(categoryTitle);

			// Convert the list of station ptrs to a vector of station ptrs
			vector<Station*> stationVector(std::begin(catStations), std::end(catStations));
			customReport->GenerateReport(stationVector, file);

			file.AddLine(" ");
		}

		if (!allStations.empty())
		{
			file.AddLine("The following stations are not found in any category:");
			for (Station *s : allStations)
			{
				file.AddLine(s->StationCallsign());
			}

			file.AddLine(" ");
		}

		string reportFileName = customReport->GetFileName();
		string fileName = resultsFolder + reportFileName;

		file.Write(fileName);
	}

	return true;
}



// Determine the categories for each station in all custom reports
void CustomReportManager::DetermineStationCategories(vector<Station*>& stations)
{
	for (CustomReport *customReport : m_customReports)
	{
		customReport->DetermineStationCategories(stations);
	}

}

