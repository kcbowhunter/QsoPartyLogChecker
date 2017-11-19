#include "stdafx.h"
#include "DxccCountryManager.h"
#include "boost/algorithm/string.hpp"
using namespace boost;
#include "StringUtils.h"
#include "FileUtils.h"
#include "TextFile.h"
#include "DxccCountry.h"

DxccCountryManager::DxccCountryManager()
{
}


DxccCountryManager::~DxccCountryManager()
{
	auto iter = m_countries.begin();
	auto iterEnd = m_countries.end();

	for (; iter != iterEnd; ++iter)
	{
		DxccCountry *country = *iter;
		delete country;
	}
	m_countries.clear();
}


bool DxccCountryManager::ProcessDxccCountriesFile(const string& dxccfilename)
{
	if (dxccfilename.empty())
	{
		printf("Error: ProcessDxccCountriesFile - DXCC filename cannot be empty\n");
		return false;
	}

	if (!FileUtils::FileExists(dxccfilename))
	{
		printf("Error: DXCC Filename - unable to open file %s\n", dxccfilename.c_str());
		return false;
	}

	TextFile file;
	bool status = file.Read(dxccfilename);
	if (!status)
	{
		printf("Error: ProcessDxccCountriesFile - error reading file %s\n", dxccfilename.c_str());
		return false;
	}

	vector<string> lines;
	file.GetLines(lines);

	string line, lineLower;
	auto count = lines.size();
	size_t i = 0;
	int countryCount = 0;
	int prefixCount = 0;
	DxccCountry *country = nullptr;
	for (i = 0; i < count; ++i)
	{
		line = lines[i];

		line = StringUtils::RemoveComment(line, '#');
		trim(line);
		if (line.empty()) continue;

		lineLower = line;
		StringUtils::ToLower(lineLower);

		status = StringUtils::ExtractHeading(line);
		if (status)
		{
			// New Country!
			country = new DxccCountry();
			country->SetNumber(++countryCount);
			country->SetName(line);
			m_countries.push_back(country);
			prefixCount = 0;
		}
		else if (country == nullptr)
		{
			printf("Error in processing dxcc countries file %s, entry %s not recognized\n", dxccfilename.c_str(), line.c_str());
			return false;
		}
		else
		{
			if (++prefixCount == 1)
			{
				string prefix(lineLower);
				StringUtils::ToUpper(prefix);
				country->SetDefaultPrefix(prefix);
			}
			country->AddPrefix(lineLower);
		}

	}

	return true;
}

// Return the dxcc country for the given callsign
bool DxccCountryManager::FindCountryName(const string& callsign, string& country)
{
	bool status = false;

	DxccCountry *dxccCountry = FindCountry(callsign);

	if (dxccCountry != nullptr)
	{
		country = dxccCountry->GetName();
		status = true;
	}

	return status;
}


// Return the dxcc country for the given callsign
// Return nullptr if the country is not found
DxccCountry *DxccCountryManager::FindCountry(const string& callsign) const
{
	auto iter = m_countries.begin();
	auto iterEnd = m_countries.end();

	int matchCount = 0;
	int count = 0;
	DxccCountry *dxccCountry = nullptr;
	for (; iter != iterEnd; ++iter)
	{
		DxccCountry *country = *iter;
		if (country->Match(callsign, count))
		{
			if (count > matchCount)
			{
				matchCount = count;
				dxccCountry = country;
			}
		}
	}

	return dxccCountry;
}

// Return the dxcc country object with the given country name
DxccCountry *DxccCountryManager::FindCountryByName(const string& countryName) const
{
	string name;
	for (DxccCountry *dxcc : m_countries)
	{
		name = dxcc->GetName();
		if (boost::iequals(name, countryName))
		{
			return dxcc;
		}
	}

	return nullptr;
}
