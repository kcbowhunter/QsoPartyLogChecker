
#pragma once

enum ContestConfigError { eNoError, eLogFolderNotProvided, eLogFolderDoesNotExist };
enum MultipliersType;

class DxccCountryManager;
class CustomReportManager;

// Configuration data for a contest
class ContestConfig
{
public:
	ContestConfig();
	~ContestConfig();

	bool Process(vector<string>& data);

	const string& GetTitle() const { return m_title; }
	const string& GetAbbrev() const { return m_titleAbbrev; }
	const string& GetState() const { return m_state; }
	const string& GetStateAbbrev() const { return m_stateAbbrev; }

	map<string, string>& GetGeneralConfigData() { return m_generalConfigData; }
	map<string, string>& GetFilesConfigData() { return m_filesConfigData; }

	// abbrevs are lower case
	void GetCountyAbbrevs(set<string>& countyAbbrevs);

	// map is lower case abbrev vs county name
	void GetCountyMap(map<string, string>& countyMap);

	// Get the log file names
	void GetLogFileNames(vector<string>& logFileNames);

   // Get the value for a data item in the Folders section of the config file
   string GetFoldersSectionValue(const string& key);

   // Set the key,value for a data item in the Folders section of the config file
   void SetFoldersSectionValue(const string& key, const string& value);

   // state abbreviation map, ie MO Missouri, one key,value pair per line
   const map<string, string>& GetStateAbbrevMap() const { return m_stateAbbrevMap; }

   // Canada abbreviation map, ie "BC", "British Columbia", one key,value pair per line
   const map<string, string>& GetCanadaAbbrevMap() const { return  m_canadaAbbrevMap; }

   // Map of arrl section abbrev to state name, ie. EB, CA
   // This is used to map from arrl sections to provinces and states because most qso parties use states and provinces as multipliers
   const map<string, string>& GetArrlSectionsMap() const { return  m_arrlSectionsMap; }

   // Get the category data for processing
   const list<list<string>>& GetCategoryData() const { return m_categoryData; }

   // Get the category2 data for processing (usually a separate Certificates category file)
   const list<list<string>>& GetCategory2Data() const { return m_categoryData2; }

   // Get the Qso Data
   void GetQsoData(list<string> &qsoData);

   // This method is used for testing
   bool AddFolderKeyValuePair(const string& key, const string& value);

   ContestConfigError GetError() const { return m_contestConfigError; }

   void SetVerbose(const bool b) { m_verbose = b; }
   bool GetVerbose() const { return m_verbose; }

   const string& GetCategoriesFileName() { return m_categoriesFileName; }
   const string& GetCategories2FileName() { return m_categories2FileName; }

   // Points per Qso for each mode
   int GetCwPoints() const { return m_cwPoints; }
   int GetPhonePoints() const { return m_phonePoints; }
   int GetDigitalPoints() const { return m_digitalPoints; }

   bool GetInStateMultsCounties() const { return m_inStateMultsCounties; }

   const string& GetResultsFolder();
   const string& GetLogReportsFolder();

   MultipliersType GetMultipliersType() const { return m_multipliersType; }
   bool GetBonusStationMultipliers() const { return m_bonusStationMultipliers; }

private:
	string m_title;
	string m_titleAbbrev;
	string m_state;
	string m_stateAbbrev;
    string m_rootFolder;

    string m_resultsFolder;
    string m_logReportsFolder;

   // Points per Qso for each mode
   int m_cwPoints;
   int m_phonePoints;
   int m_digitalPoints;

   // Multipliers are per contest, per band or per mode
   MultipliersType m_multipliersType;

   // Bonus Stations are score multipliers
   bool m_bonusStationMultipliers;

   // Generate Missing Logs
   bool m_generateMissingLogs;

   ContestConfigError m_contestConfigError;
   bool m_verbose;  // write errors to console
   string m_categoriesFileName;

   // The categories2 file name can be used for a separate certificates category
   string m_categories2FileName;

	// map of config data key to variable
	// i.e. "conetesttitle", &m_title
	map<string, string*> m_dataMap;

	// key value pairs for general section of file
	map<string, string> m_generalConfigData;
	map<string, string> m_filesConfigData;

	// key value pairs for folders, i.e. logs, results, etc
	map<string, string> m_folders;

   // key value pairs for scoring, cwpoints=x, ...
   map<string, string> m_scoring;

	// qso token types, ie date,mode,freq etc
	list<string> m_qsoData;

	// county abbrev, county
	// abbrev is lower case
	map<string, string> m_countyMap;

   // lower case county abbreviations
	set<string> m_countyAbbrevs;

	// List of log file names
	list<string> m_logFileNames;

   // state abbreviation map, ie MO Missouri, one key,value pair per line
   map<string, string> m_stateAbbrevMap;

   // Canada abbreviation map, ie "BC", "British Columbia", one key,value pair per line
   map<string, string> m_canadaAbbrevMap;

   // Map of arrl section abbrev to state name, ie. EB, CA
   // This is used to map from arrl sections to provinces and states because most qso parties use states and provinces as multipliers
   map<string, string> m_arrlSectionsMap;

   // Configuration Files Folder
   string m_configFilesFolder;

   // Each list<string> contains the data for one category
   list<list<string> > m_categoryData;

   // Each list<string> contains the data for one category
   // Category2 data is usually for a separate list of categories for certificates
   list<list<string> > m_categoryData2;

   // true -> counties are multipliers for instate stations
   bool m_inStateMultsCounties;

   // Callsigns of Bonus Stations
   list<string> m_bonusStations;

   // Number of points for working the bonus station
   int m_bonusStationPoints;

   // Bonus points for submitting a Cabrillofile
   int m_cabrilloBonusPoints;

   DxccCountryManager *m_dxccCountryManager;

   bool ProcessDxccCountriesFile(const string& dxccfile);

   // Instate Stations get mults for dx contacts 
   // Based on dxcc countries worked
   bool m_instateDxccMults;

   // This contest has power multipliers
   bool   m_powerMultipliers;
   double m_powerMultiplierQRP;
   double m_powerMultiplierLow;
   double m_powerMultiplierHigh;

  // InState stations out of state point scaler
  // #SCQP 2017
  // # ie in state stations get 1 point for instate phone qso
  // # and get 2 points for out of state phone qso
   int m_inStateWorksOutOfStatePointsScaler;

   // Bonus station points are per band and per mode
   bool m_bonusStationPointsPerBandPerMode;

   // Validate Qso's in submitted logs
   bool m_validateQsos;

   // Counties activated by mobile stations are score multipliers
   bool m_mobileCountiesActivatedAreMultipliers;

   // This contest provides one multiplier for working a DX station
   bool m_dxMultiplier;

   CustomReportManager* m_customReportManager;

   // custom report configuration file names
   list<string> m_customReportConfigFilenames;

   // Points for working bonus counties
   int m_bonusCountyPoints;

   // List of bonus county abbreviations
   set<string> m_bonusCounties;

public:
	// Return the position of the next line of text to process
	int ProcessSection(vector<string>& data, map<string, string>& configData, int count, const string& sectionName);

	// Process the Qso data token types, date, mode, freq etc
	int ProcessQsoData(vector<string>& data, int count);

	void SetupDataMap();

	// Process the county abbreviation file, state file, etc
	bool ProcessConfigFiles();

	// Process the folders for logs, results etc
	bool ProcessFolders();

   // Process the scoring section 
   bool ProcessScoring();

   // Set up the root folder in m_rootFolder, add trailing slash to folder name if needed
   void SetupRootFolder();

   // Are the required folders (Logs...) specified and available?
   bool CheckRequiredFolders();

   // Create the output folders is needed
   bool CreateOutputFoldersIfNeeded();

   // Check that the subfolder is provided, or create under the root folder
   void CreateFolderIfNeeded(int &error, string& folder);

	// Setup the map of county abbrev, county
	bool SetupCountiesMap();

   // Setup the state abbrevation map, MO Missouri etc
   // Also setup the province map for Canada
   bool SetupAbbrevMap(map<string, string>& abbrevMap, const string& configKey);

	// Get the log file names from the logs directory
	bool GetLogFileNamesFromWindows();

	// return true if the given directory exists
	static bool DirExists(const string& dir);

   // return true if the key is found in the map, the value is returned as a reference
   static bool GetValueFromMap(map<string, string>& dataMap, const string& key, string& value);

   // return true if the key is found and set value to the boolean value
   static bool GetBooleanFromMap(map<string, string>& dataMap, const string& key, bool& value);

   // Process the categories file and read to the list<list<string>> m_CategoryData 
   // This method is used to process primary and secondary categories
   static bool ProcessCategories(const string& categoriesFileName, list<list<string>>& categoryData);

   // Dump the category data to the console for debugging
   void DumpCategoryData();

   // Get an integer value and issue error message if not an integer
   bool GetIntegerValue(const string& section, const string& key, const string& value, int& number);

   // Callsigns of Bonus Stations
   list<string> GetBonusStations() { return m_bonusStations; }

   // Number of points for working the bonus station
   int GetBonusStationPoints() { return m_bonusStationPoints; }

   // Bonus points for submitting a Cabrillofile
   int GetCabrilloBonusPoints() { return m_cabrilloBonusPoints; }

   DxccCountryManager *GetDxccCountryManager() { return m_dxccCountryManager; }

   bool InstateDxccMults() const { return m_instateDxccMults; }   

   // This contest has power multipliers
   bool   HasPowerMultipliers() const { return m_powerMultipliers; }
   double GetPowerMultiplierQRP() const { return m_powerMultiplierQRP; }
   double GetPowerMultiplierLow() const { return m_powerMultiplierLow; }
   double GetPowerMultiplierHigh() const { return m_powerMultiplierHigh; }

   bool GenerateMissingLogs() const { return m_generateMissingLogs; }
   void SetGenerateMissingLogs(bool b) { m_generateMissingLogs = b; }

   int GetInStateWorksOutOfStatePointsScaler() const { return m_inStateWorksOutOfStatePointsScaler; }

   bool GetBonusStationPointsPerBandPerMode() const { return m_bonusStationPointsPerBandPerMode; }

   bool GetValidateQsos() const { return m_validateQsos; }

   bool GetMobileCountiesActivatedAreMultipliers() const { return m_mobileCountiesActivatedAreMultipliers; }

   bool GetDxMultiplier() const { return m_dxMultiplier; }

   CustomReportManager* GetCustomReportManager() { return m_customReportManager; }

   // Points for working bonus counties
   int GetBonusCountyPoints() const { return m_bonusCountyPoints; }

   // List of bonus county abbreviations
   set<string> GetBonusCounties() const { return m_bonusCounties; }

};
