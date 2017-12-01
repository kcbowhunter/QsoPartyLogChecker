#pragma once

#include "stdafx.h"
class Station;
class TextFile;
class CategoryMgr;

// Class to Generate Custom Reports
class CustomReport
{
public:
	CustomReport();
	~CustomReport();

	// Process the strings from the config file
	bool ProcessConfigFile(vector<string>& text);

	// Process the categories in the config file
	bool ProcessCategories(const string& categoriesFileName);

	// Generate the report in the results folder
//	bool GenerateReportForAllStations(const string& resultsFolder, vector<Station*>& stations);
	bool GenerateReport(vector<Station*>& stations, TextFile& file);

	string GetFileName() const { return m_fileName; }

	void DetermineStationCategories(vector<Station*>& stations);

	CategoryMgr *GetCategoryMgr() { return m_categoryMgr; }

private:
	std::map<string, string> m_keyValuePairs;
	vector<string> m_columnData;
	vector<string> m_columnHeading;
	string m_fileName;  // output file name for the report
	CategoryMgr *m_categoryMgr;
};

