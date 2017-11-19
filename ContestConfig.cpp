
#include "stdafx.h"
#include "ContestConfig.h"
#include "StringUtils.h"
#include "FileUtils.h"

#include "TextFile.h"
#include "boost/algorithm/string.hpp"
using namespace boost;
#include "DxccCountryManager.h"
#include "DxccCountry.h"
#include "MultipliersMgr.h"
#include "CustomReportManager.h"

ContestConfig::ContestConfig()
   :
   m_configFilesFolder(),
   m_rootFolder(),
   m_contestConfigError(eNoError),
   m_verbose(true),
   m_categoriesFileName(),
   m_categories2FileName(),
   m_cwPoints(0),
   m_phonePoints(0),
   m_digitalPoints(0),
   m_cabrilloBonusPoints(0),
   m_inStateMultsCounties(true),
   m_resultsFolder(),
   m_logReportsFolder(),
   m_bonusStations(),
   m_bonusStationPoints(0),
   m_dxccCountryManager(nullptr),
   m_instateDxccMults(false),
   m_multipliersType(eMultUnknown),
   m_bonusStationMultipliers(false),
   m_powerMultipliers(false),
   m_powerMultiplierQRP(1.0),
   m_powerMultiplierLow(1.0),
   m_powerMultiplierHigh(1.0),
   m_generateMissingLogs(true),
   m_inStateWorksOutOfStatePointsScaler(0),
   m_bonusStationPointsPerBandPerMode(false),
   m_validateQsos(true),
   m_mobileCountiesActivatedAreMultipliers(false),
   m_dxMultiplier(true),
   m_customReportManager(new CustomReportManager())
   {
	   SetupDataMap();
   }

ContestConfig::~ContestConfig()
{
	if (m_dxccCountryManager != nullptr)
	{
		delete m_dxccCountryManager;
		m_dxccCountryManager = nullptr;
	}

	delete m_customReportManager;
	m_customReportManager = nullptr;
}

bool ContestConfig::Process(vector<string>& data)
{
	string temp;
	int size = (int)data.size();
	int errorCount = 0;
	int count = 0;
	while (count < size)
	{
		// Look for a new [Section]
		const string& str = data[count++];
		if (str.empty()) continue;
		temp = str;

		temp = StringUtils::RemoveComment(temp, '#');
		trim(temp);
		if (temp.empty()) continue;

		size_t len = temp.size();
		char first = temp.at(0);
		char last = temp.at(len - 1);
		if (first != '[' || len < 3 || last != ']')
		{
			printf("Warning: Config file, unknown text outside section %s\n", str.c_str());
			continue;
		}

		string section = temp.substr(1, len - 2);

		if (boost::iequals(section, "general"))
		{
			count = ProcessSection(data, m_generalConfigData, count, section);
			GetBooleanFromMap(m_generalConfigData, "generatemissinglogs", m_generateMissingLogs);
		}
		else if (boost::iequals(section, "configfiles"))
		{
			count = ProcessSection(data, m_filesConfigData, count, section);
			if (!ProcessConfigFiles())
			{
				++errorCount;
			}
		}
		else if (boost::iequals(section, "folders"))
		{
			count = ProcessSection(data, m_folders, count, section);
			if (!ProcessFolders())
			{
				++errorCount;
			}
		}
      else if (boost::iequals(section, "scoring"))
      {
         count = ProcessSection(data, m_scoring, count, section);
         if (!ProcessScoring())
         {
            ++errorCount;
         }
      }
		else if (boost::iequals(section, "qso"))
		{
			count = ProcessQsoData(data, count);
		}
		else
		{
			printf("Error Processing Config File, Unknown Section: %s\n", temp.c_str());
			return false;
		}
	}

	return errorCount == 0;
}

bool ContestConfig::GetIntegerValue(const string& section, const string& key, const string& value, int& number)
{
   number = -1;
   bool status = StringUtils::IsInteger(value);
   if (status)
   {
      number = atoi(value.c_str());
   }
   else
   {
      printf("Error in %s Section\n   Value %s for key %s is not an integer\n\n", 
             section.c_str(), value.c_str(), key.c_str());
   }

   return status;
}

bool ContestConfig::ProcessScoring()
{
   int errorCount = 0;
   const string section("Scoring");
   bool status = true;
   for (auto iter : m_scoring)
   {
      string key = iter.first;
      string value = iter.second;

      string keyLower(key);
      StringUtils::ToLower(keyLower);

      if (keyLower == "cwpoints")
      {
         status = GetIntegerValue(section, keyLower, value, m_cwPoints);
         if (!status) ++errorCount;
      }
      else if (keyLower == "digitalpoints")
      {
         status = GetIntegerValue(section, keyLower, value, m_digitalPoints);
         if (!status) ++errorCount;
      }
      else if (keyLower == "phonepoints")
      {
         status = GetIntegerValue(section, keyLower, value, m_phonePoints);
         if (!status) ++errorCount;
      }
      else if (keyLower == "instatemultscounties")
      {
         StringUtils::ParseBoolean(m_inStateMultsCounties, value);
      }
	  else if (keyLower == "instatedxccmults")
	  {
		  StringUtils::ParseBoolean(m_instateDxccMults, value);
	  }
//      else if (keyLower == "bonusstation")
//      {
//         string bonusStation(value);
//         StringUtils::ToLower(bonusStation);
//         m_bonusStations.push_back(bonusStation);
//      }
      else if (keyLower == "bonusstationpoints")
      {
         string points(value);

         if (value.empty())
         {
            ++errorCount;
            printf("Error reading BonusStationPoints: value is missing\n");
            continue;
         }
         else if (!StringUtils::IsInteger(points))
         {
            ++errorCount;
            printf("Error in config file, [Scoring] BonusStationPoints: %s is not an integer\n", points.c_str());
            continue;
         }

         int num = atoi(points.c_str());
         m_bonusStationPoints = num;
      }
	  else if (keyLower == "bonusstationmults")
	  {
		  StringUtils::ParseBoolean(m_bonusStationMultipliers, value);
	  }
      else if (keyLower == "cabrillobonuspoints")
      {
         if (!StringUtils::IsInteger(value))
         {
            ++errorCount;
            printf("Error in config file, [Scoring] CabrilloBonusPoints: %s is not an integer\n", value.c_str());
            continue;
         }
         int num = atoi(value.c_str());
         m_cabrilloBonusPoints = num;
      }
	  else if (keyLower == "multipliers")
	  {
		  if (boost::iequals(value, "PerMode"))
			  m_multipliersType = eMultPerMode;
		  else if (boost::iequals(value, "PerContest"))
			  m_multipliersType = eMultPerContest;
		  else if (boost::iequals(value, "PerBand"))
			  m_multipliersType = eMultPerBand;
		  else
		  {
			  ++errorCount;
			  printf("Error in config file, [Scoring] section, Multipliers=%s unknown\n", value.c_str());
		  }
	  }
	  else if (keyLower == "powermults")
	  {
		  StringUtils::ParseBoolean(m_powerMultipliers, value);
	  }
	  else if (keyLower == "powermultqrp")
	  {
		  StringUtils::ParseDouble(m_powerMultiplierQRP, value);
	  }
	  else if (keyLower == "powermultlow")
	  {
		  StringUtils::ParseDouble(m_powerMultiplierLow, value);
	  }
	  else if (keyLower == "powermulthigh")
	  {
		  StringUtils::ParseDouble(m_powerMultiplierHigh, value);
	  }
	  else if (keyLower == "instateworksoutofstatepointsscaler")
	  {
		  if (StringUtils::IsInteger(value))
		  {
			  m_inStateWorksOutOfStatePointsScaler = atoi(value.c_str());
		  }
		  else
		  {
			  printf("Error in InStateWorksOutOfStatePointsScaler: %s\n", value.c_str());
		  }
	  }
	  else if (keyLower == "bonusstationpointsper")
	  {
		  if (boost::iequals(value, "perbandpermode"))
		  {
			  m_bonusStationPointsPerBandPerMode = true;
		  }
		  else
		  {
			  printf("Error in Scoring Section for key=BonusStationPointsPer, unknown value=%s\n", value.c_str());
			  ++errorCount;
		  }
	  }
	  else if (keyLower == "validateqsos")
	  {
		  StringUtils::ParseBoolean(m_validateQsos, value);
	  }
	  else if (boost::iequals(keyLower, "MobileCountiesActivatedAreMultipliers"))
	  {
		  StringUtils::ParseBoolean(m_mobileCountiesActivatedAreMultipliers, value);
	  }
	  else if (boost::iequals(keyLower, "DxMultiplier"))
	  {
		  StringUtils::ParseBoolean(m_dxMultiplier, value);
	  }
	  else
      {
		  ++errorCount;
         printf("Error in %s Section\n   Unknown key value pair '%s' = '%s'\n",
            section.c_str(), key.c_str(), value.c_str());
         printf("   Expected:\n      CwPoints=n or DigitalPoints=n or PhonePoints=n etc\n\n");
      }
   }

   return errorCount == 0;
}

// Process the folders for logs, results etc
bool ContestConfig::ProcessFolders()
{
   SetupRootFolder();

   bool status = CheckRequiredFolders();
   if (!status)
      return false;

   status = CreateOutputFoldersIfNeeded();
   if (!status)
      return false;

	status = GetLogFileNamesFromWindows();

   // The results folder must be specified
   if (status)
   {
      string folder;
      status = GetValueFromMap(m_folders, "results", folder);
      // TODO: check if the folder exists
      if (!status)
      {
         printf("Error Processing Config File, Results folder is missing\n");
      }
   }

	return status;
}

// Set up the root folder in m_rootFolder, add trailing slash to folder name if needed
void ContestConfig::SetupRootFolder()
{
   auto iter = m_folders.find("root");
   if (iter != m_folders.end())
   {
      m_rootFolder = (*iter).second;
      StringUtils::AddTrailingSlashIfNeeded(m_rootFolder);
   }
}

// Are the required folders (Logs...) specified and available?
bool ContestConfig::CheckRequiredFolders()
{
   // The only required folder is "Logs"
   // It can be provided explicitly or inferred from the root folder
   auto iter = m_folders.find("logs");
   if (iter == m_folders.end())
   {
      if (m_rootFolder.empty())
      {
         if (m_verbose)
            printf("Error in Folders Section, no Logs entry provided\n");

         m_contestConfigError = eLogFolderNotProvided;
         return false;
      }
      else
      {
         string logs = m_rootFolder + string("Logs");
         if (DirExists(logs))
         {
            m_folders["logs"] = logs;
         }
         else
         {
            if (m_verbose)
               printf("Error in Folders Section, Logs folder not found at %s\n", logs.c_str());

            m_contestConfigError = eLogFolderDoesNotExist;
            return false;
         }
      }
   }
   else
   {
      // The logs folder was provided by the user, does the Logs folder exist?
      string logs = (*iter).second;
      if (logs.empty())
      {
         printf("Error in Folders Section, Logs folder name is missing\n");
         return false;
      }
      if (!DirExists(logs))
      {
         if (m_verbose)
            printf("Error in Folders Section, Logs folder does not exist: \n\t%s\n", logs.c_str());

         m_contestConfigError = eLogFolderDoesNotExist;
         return false;
      }
   }

   return true;
}


// Check that the subfolders are specified in the config file
bool ContestConfig::CreateOutputFoldersIfNeeded()
{
   int error = 0;
   string folder("Results");
   CreateFolderIfNeeded(error, folder);

   return true;
}

// Check that the subfolder is provided, or create under the root folder
// Increment the error count if the root folder is null and the folder is not provided
void ContestConfig::CreateFolderIfNeeded(int &error, string& folder)
{
   auto iter = m_folders.find(folder);
   if (iter == m_folders.end())
   {
      if (!m_rootFolder.empty())
      {

      }
   }
   else
   {
      // check if folder exists or can be created
   }
}

// Get the log file names from the logs directory
bool ContestConfig::GetLogFileNamesFromWindows()
{
	auto iter = m_folders.find("logs");
	if (iter == m_folders.end())
	{
		printf("Error in Folders section, no Logs entry provided\n");
		return false;
	}

	string folder = iter->second;
	if (!DirExists(folder))
	{
		printf("Error in Files Section: Folder %s does not exist\n", folder.c_str());
		return false;
	}

	StringUtils::AddTrailingSlashIfNeeded(folder);

	string filemask = folder + string("*.log");

	WIN32_FIND_DATA findFileData;
	HANDLE hfind = FindFirstFile(filemask.c_str(), &findFileData);
	if (hfind == INVALID_HANDLE_VALUE)
	{
//		printf("Error processing log files directory %s (%d)\n", filemask.c_str(), GetLastError());
		printf("Warning: no log files found in director %s\n", folder.c_str());
		//		return false;
		return true;
	}

	string fullname = folder + string(findFileData.cFileName);
	m_logFileNames.push_back(fullname);

	while (FindNextFile(hfind, &findFileData) != 0)
	{
		fullname = folder + string(findFileData.cFileName);
		m_logFileNames.push_back(fullname);
	}

	return true;
}


// Process the county abbreviation file, state file, etc
bool ContestConfig::ProcessConfigFiles()
{
	bool status = SetupCountiesMap();

   status = SetupAbbrevMap(m_stateAbbrevMap, "states") && status;

   status = SetupAbbrevMap(m_canadaAbbrevMap, "canada") && status;

   status = SetupAbbrevMap(m_arrlSectionsMap, "arrlsections") && status;

   auto iter = m_filesConfigData.find("categories");
   if (iter == m_filesConfigData.end())
   {
      printf("Info: No Categories file provided\n");
   }
   else
   {
      string categoriesFile = (*iter).second;
      m_categoriesFileName = m_configFilesFolder + categoriesFile;
      status = ProcessCategories(m_categoriesFileName, m_categoryData);
//      DumpCategoryData();
   }

   iter = m_filesConfigData.find("categories2");
   if (iter != m_filesConfigData.end())
   {
      string categoriesFile = (*iter).second;
      m_categories2FileName = m_configFilesFolder + categoriesFile;
      status = ProcessCategories(m_categories2FileName, m_categoryData2);
      //      DumpCategoryData();
   }

   iter = m_filesConfigData.find("dxcc");
   if (iter != m_filesConfigData.end())
   {
	   string dxccfile = m_configFilesFolder + (*iter).second;
	   status = ProcessDxccCountriesFile(dxccfile);
   }

   // Create custom reports
   m_customReportManager->CreateCustomReports(m_configFilesFolder, m_customReportConfigFilenames);

	return status;
}

bool ContestConfig::ProcessCategories(const string& categoriesFileName, list<list<string>>& categoryData)
{
   if (categoriesFileName.empty())
   {
      printf("Error: ProcessCategories - categories filename cannot be empty\n");
      return false;
   }

   if (!FileUtils::FileExists(categoriesFileName))
   {
      printf("Error: ProcessCategories - unable to open file %s\n", categoriesFileName.c_str());
      return false;
   }

   TextFile file;
   bool status = file.Read(categoriesFileName);
   if (!status)
   {
      printf("Error: ProcessCategories - error reading file %s\n", categoriesFileName.c_str());
      return false;
   }

   vector<string> lines;
   file.GetLines(lines);

   list<string> category;

   string line, lineLower;
   auto count = lines.size();
   size_t i = 0;
   for (i = 0; i < count; ++i)
   {
      line = lines[i];

      line = StringUtils::RemoveComment(line, '#');
      trim(line);
      if (line.empty()) continue;

      lineLower = line;
      StringUtils::ToLower(lineLower);

      if (lineLower == "[category]")
      {
         if (!category.empty())
         {
            categoryData.push_back(category);
            category.clear();
         }
      }
      else
      {
         category.push_back(line);
      }
   }

   categoryData.push_back(category);
   category.clear();

   return true;
}

// Dump the category data to the console for debugging
void ContestConfig::DumpCategoryData()
{
   int count = 0;
   printf("ContestConfig - Dump Category Raw Data\n");
   for (list<string>& category : m_categoryData)
   {
      printf("Category %d\n", count++);
      for (string& str : category)
      {
         if (str.empty())
            printf("\n");
         else
            printf("   %s\n", str.c_str());
      }
   }
}

// Generic setup for key,value map, i.e. MO Missouri 
//                                       BC British Columbia
//   where the first space separates the key and the value
bool ContestConfig::SetupAbbrevMap(map<string,string>& abbrevMap, const string& configKey)
{
   if (m_configFilesFolder.empty())
   {
      printf("Error in ContestConfig::SetupStateAbbrevMap, config files folder is empty\n");
      return false;
   }
 
   // States=states.txt
   string locationFileName;
   auto iter = m_filesConfigData.find(configKey);
   if (iter == m_filesConfigData.end())
   {
      printf("Error in ConfigFiles Section: 'States=' not found\n");
      return false;
   }
   locationFileName = (*iter).second;

   locationFileName = m_configFilesFolder + locationFileName;
   if (!FileUtils::FileExists(locationFileName))
   {
      printf("Error in ConfigFiles Section: %s File %s not found\n", configKey.c_str(), locationFileName.c_str());
      return false;
   }

   string key;
   string value;

   TextFile statesFile;
   bool status = statesFile.Read(locationFileName);

   vector<string> lines;
   statesFile.GetLines(lines);

   for (string str : lines)
   {
      boost::trim(str);
      if (str.empty())
         continue;

      str = StringUtils::RemoveComment(str, '#');
      boost::trim(str);
      if (str.empty())
         continue;

      size_t pos = str.find(' ');
      if (pos == string::npos)
      {
         printf("Error processing %s file, unknown line %s\n",configKey.c_str(), str.c_str());
         return false;
      }

      size_t len = str.size();

      key = str.substr(0, pos);
      value = str.substr(pos + 1, len - pos);

      trim(key);
      trim(value);

      if (key.empty() || value.empty())
      {
         printf("Error Processing Section data, missing key and/or value %s\n", str.c_str());
         continue;
      }

      StringUtils::ToLower(key);
      abbrevMap[key] = value;
   }

   return true;
}


bool ContestConfig::SetupCountiesMap()
{
	string folder;
	auto iter = m_filesConfigData.find("folder");
	if (iter == m_filesConfigData.end())
	{
		printf("Error in ConfigFiles Section: 'Folder' not found\n");
		return false;
	}
	folder = iter->second;

	if (!DirExists(folder))
	{
		printf("Error in ConfigFiles Section: Folder %s does not exist\n", folder.c_str());
		return false;
	}

	StringUtils::AddTrailingSlashIfNeeded(folder);
   m_configFilesFolder = folder;

	string countiesFile;
	iter = m_filesConfigData.find("counties");
	if (iter == m_filesConfigData.end())
	{
		printf("Error in ConfigFiles Section: 'Counties' not found\n");
		return false;
	}
	countiesFile = (*iter).second;

	countiesFile = folder + countiesFile;
	if (!FileUtils::FileExists(countiesFile))
	{
		printf("Error in ConfigFiles Section: County File %s not found\n", countiesFile.c_str());
		return false;
	}

	string key;
	string value;

	TextFile countyFile;
	bool status = countyFile.Read(countiesFile);

	vector<string> lines;
	countyFile.GetLines(lines);

	// the file is of the form abbrev   county name
	// assume the first space separates the tokens and the county abbrev is first
	for (string str : lines)
	{
		boost::trim(str);
		if (str.empty())
			continue;

		str = StringUtils::RemoveComment(str, '#');
		boost::trim(str);
		if (str.empty())
			continue;

      size_t pos = str.find(',');
		
      if (pos == string::npos)
      {
         pos = str.find(' ');
      }

		if (pos == string::npos)
		{
			printf("Error processing county file, unknown line %s\n", str.c_str());
			return false;
		}

        size_t len = str.size();
		if (pos == 0 || pos == len - 1)
		{
			printf("Error Processing Section data, bad key=value pair: %s\n", str.c_str());
			continue;
		}

		key = str.substr(0, pos);
		value = str.substr(pos + 1, len - pos);

		trim(key);
		trim(value);

		if (key.empty() || value.empty())
		{
		printf("Error Processing Section data, missing key and/or value %s\n", str.c_str());
		continue;
		}

		StringUtils::ToLower(key);
		m_countyMap[key] = value;
		m_countyAbbrevs.insert(key);
	}

	return true;
}

// Collect all the qso token types in m_qsoData
int ContestConfig::ProcessQsoData(vector<string>& data, int count)
{
	int size = (int)data.size();

	string str;
	while (count < size)
	{
		str = data[count++];
		trim(str);

		if (str.empty())
			continue;

		str = StringUtils::RemoveComment(str, '#');
		trim(str);
		if (str.empty()) continue;

		// New Section?
		if (str.at(0) == '[')
			return count - 1;

		m_qsoData.push_back(str);
	}

	return count;
}

// Return the position of the next line of text to process
int ContestConfig::ProcessSection(vector<string>& data, map<string, string>& configData, int count, const string& sectionName)
{
	size_t len = 0;
	string key;
	string value;
	string str;
	int size = (int)data.size();
	while (count < size)
	{
		str = data[count++];
		trim(str);

		if (str.empty())
			continue;

		str = StringUtils::RemoveComment(str, '#');
		trim(str);
		if (str.empty()) continue;

		// New Section?
		if (str.at(0) == '[')
			return count - 1;

		size_t pos = str.find('=');
		if (pos == string::npos)
		{
			printf("Error Processing Section data, '=' not found: %s\n", str.c_str());
			continue;
		}

		len = str.size();
		if (pos == 0 || pos == len - 1)
		{
			printf("Error Processing Section data, bad key=value pair: %s\n", str.c_str());
			continue;
		}

		key = str.substr(0, pos);
		value = str.substr(pos + 1, len - pos);

		trim(key);
		trim(value);

		if (key.empty() || value.empty())
		{
			printf("Error Processing Section data, missing key and/or value %s\n", str.c_str());
			continue;
		}

		StringUtils::ToLower(key);

		// There can be many bonus stations
		if (key == "bonusstation")
		{
			StringUtils::ToLower(value);
			m_bonusStations.push_back(value);
		}
		else if (key == "customreport")
		{
			m_customReportConfigFilenames.push_back(value);
		}
		else
		{
			configData[key] = value;
		}

		// Copy some values to the data map
		auto iter = m_dataMap.find(key);
		if (iter != m_dataMap.end())
		{
			pair<const string, string*> element = *iter;
			string *pString = element.second;
			*pString = value;
		}

	}

	return count;
}

void ContestConfig::SetupDataMap()
{
	m_dataMap["title"] = &m_title;
	m_dataMap["titleabbrev"] = &m_titleAbbrev;
	m_dataMap["state"] = &m_state;
	m_dataMap["stateabbrev"] = &m_stateAbbrev;
}

// return true if the given directory exists
bool ContestConfig::DirExists(const string& dir)
{
	if (dir.empty())
		return false;

	DWORD ftyp = GetFileAttributesA(dir.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return true;
}

// all county abbrevs are lower case
void ContestConfig::GetCountyAbbrevs(set<string>& countyAbbrevs)
{
	countyAbbrevs.clear();
	for (string abbrev : m_countyAbbrevs)
	{
		countyAbbrevs.insert(abbrev);
	}
}

// map is lower case abbrev vs county name
void ContestConfig::GetCountyMap(map<string, string>& countyMap)
{
	countyMap.clear();
	for (std::pair<string, string> item : m_countyMap)
	{
		const string& key = item.first;
		const string& value = item.second;
		countyMap[key] = value;
	}
}

// Get the log file names
void ContestConfig::GetLogFileNames(vector<string>& logFileNames)
{
	logFileNames.clear();
	if (m_logFileNames.empty())
		return;

	int i = 0;
	logFileNames.resize(m_logFileNames.size());
	for (string str : m_logFileNames)
	{
		logFileNames[i++] = str;
	}
}

// return true if the key is found in the map, the value is returned as a reference
bool ContestConfig::GetValueFromMap(map<string, string>& dataMap, const string& key, string& value)
{
   value.erase();

   auto iter = dataMap.find(key);
   if (iter == dataMap.end())
   {
      return false;
   }

   value = (*iter).second;
   return true;
}

// return true if the key is found and set value to the boolean value
bool ContestConfig::GetBooleanFromMap(map<string, string>& dataMap, const string& key, bool& value)
{
	value = false;
	string stringValue;
	bool status = GetValueFromMap(dataMap, key, stringValue);

	if (status)
	{
		StringUtils::ParseBoolean(value, stringValue);
	}

	return status;
}


// Get the value for a data item in the Folders section of the config file
string ContestConfig::GetFoldersSectionValue(const string& key)
{
   string value;
   bool status = GetValueFromMap(m_folders, key, value);
   return value;
}

// Set the (key,value) pair in the Folders section of the config file
void ContestConfig::SetFoldersSectionValue(const string& key, const string& value)
{
   string keyLow(key);
   StringUtils::ToLower(keyLow);

   auto iter = m_folders.find(keyLow);
   if (iter == m_folders.end())
   {
      m_folders[keyLow] = value;
   }
   else
   {
      (*iter).second = value;
   }
}

// Get the Qso Data
void ContestConfig::GetQsoData(list<string> &qsoData)
{
   qsoData = m_qsoData;
}


// This method is used for testing
bool ContestConfig::AddFolderKeyValuePair(const string& key, const string& value)
{
   assert(key.empty() == false);
   assert(value.empty() == false);
   if (key.empty() || value.empty())
      return false;

   string lowerKey(key);
   StringUtils::ToLower(lowerKey);

   auto iter = m_folders.find(lowerKey);
   if (iter != m_folders.end())
   {
      assert(0 && "ContestConfig::AddFolderKeyValuePair - entry exists");
      return false;
   }

   m_folders[lowerKey] = value;

   return true;
}

const string& ContestConfig::GetResultsFolder()
{
   if (m_resultsFolder.empty())
   {
      m_resultsFolder = GetFoldersSectionValue("results");
   }

   return m_resultsFolder;
}

const string& ContestConfig::GetLogReportsFolder()
{
   if (m_logReportsFolder.empty())
   {
      m_logReportsFolder = GetFoldersSectionValue("logreports");
   }

   return m_logReportsFolder;
}

bool ContestConfig::ProcessDxccCountriesFile(const string& dxccfilename)
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

	m_dxccCountryManager = new DxccCountryManager();
	bool status = m_dxccCountryManager->ProcessDxccCountriesFile(dxccfilename);

	return status;
}