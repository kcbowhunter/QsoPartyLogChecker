
#pragma once

class Station;
class ContestConfig;
class QsoTokenType;
class AllLocations;
class CategoryMgr;
class ClubMgr;
class DxccCountryManager;

// Define type for pointer to member function with no arguments
//using PStationFunc = void(Station::*)();
typedef void(Station::*PStationFunc)();

enum MultipliersType;

class Contest
{
public:
	Contest();
	virtual ~Contest();
private:
   Contest(const Contest& contest);
   ContestConfig *m_contestConfig;

public:
	const string& GetTitle() const { return m_title; }
	const string& GetAbbrev() const { return m_titleAbbrev; }
	const string& GetState() const { return m_state; }
	const string& GetStateAbbrev() const { return m_stateAbbrev; }

	bool ProcessConfigData(ContestConfig *config);

	ContestConfig *GetContestConfig() { return m_contestConfig; }

	// This method processes all the log files
	bool ProcessLogs(vector<string>& logFileNames);

   void GetCountyAbbrevs(set<string>& countyAbbrevs);

   void AddTokenTypesForMOQP();
   void AddTokenTypesForKSQP();

   // Clone the token types defined for the contest to the vector
   void CloneTokenTypes(vector<QsoTokenType*>& tokenTypes);

   const AllLocations& GetAllLocations() const { return *m_allLocations; }

   Station *GetStation(const string& callsign) const;

   int GetCwPoints() const { return m_cwPoints; }
   int GetPhonePoints() const { return m_phonePoints; }
   int GetDigitalPoints() const { return m_digitalPoints; }

   bool GetCwAndDigitalAreTheSameMode() const { return m_cwAndDigitalAreTheSameMode; }

   bool GetInStateMultsCounties() const { return m_inStateMultsCounties; }

   bool InStateDxccMults() const { return m_instateDxccMults; }

   DxccCountryManager *GetDxccCountryManager() { return m_dxccCountryManager; }

   MultipliersType GetMultipliersType() const { return m_multipliersType; }

   bool GetBonusStationMultipliers() const {return m_bonusStationMultipliers;}

   // Callsign of bonus stations
   list<string> GetBonusStations() const { return m_bonusStations; }

   // This contest has power multipliers
   bool   HasPowerMultipliers() const { return m_powerMultipliers; }
   double GetPowerMultiplierQRP() const { return m_powerMultiplierQRP; }
   double GetPowerMultiplierLow() const { return m_powerMultiplierLow; }
   double GetPowerMultiplierHigh() const { return m_powerMultiplierHigh; }

   // Setup In State Multipliers
   // (Out of state multipliers are Counties)
   bool SetupInStateMultipliers();

   // Get the In State Multipliers for this Contest
   bool GetInStateMultipliers(set<string>& inStateMultipliers) const;

   int GetInStateWorksOutOfStatePointsScaler() const;

   bool GetBonusStationPointsPerBandPerMode() const;

private:
	vector<Station*> m_stations;

	// callsign, station map
	map<string, Station*> m_stationMap;

   // stations missing log files (i.e. not submitted by users)
   map<string, Station*> m_missingStationsMap;

   // Lower case county abbreviations
   set<string> m_countyAbbrevs;

   // county abbrev, county
   // abbrev is lower case
   map<string, string> m_countyMap;

	string m_title;
	string m_titleAbbrev;
	string m_state;
	string m_stateAbbrev;

   list<QsoTokenType*> m_tokenTypes;

   // Folders
   string m_resultsFolder;

   // Log Reports Folder
   string m_logReportsFolder;

   // Missing Logs Folder
   string m_missingLogsFolder;

   // HTML output folder
   string m_htmlFolder;

   // In State County Count
   map<string, int> m_InstateCountyCount;

   // Manages state / province / section / country information
   AllLocations *m_allLocations;

   bool m_displayTimes;

   CategoryMgr *m_categoryMgr;
   CategoryMgr *m_categoryMgr2;  // secondary category, usually for certificates

   // Instate and out of state clubs
   ClubMgr *m_inStateClubs;
   ClubMgr *m_outStateClubs;

   // Total number of Qso's based on instate logs
   int m_totalQsos;
   int m_totalValidQsos;

   int m_cwPoints;
   int m_phonePoints;
   int m_digitalPoints;

   // true when cw and digital modes are considered to be the same mode
   bool m_cwAndDigitalAreTheSameMode;

   // true -> instate stations include counties as multipliers
   bool m_inStateMultsCounties;

   // Callsign of bonus stations
   list<string> m_bonusStations;

   // Number of points for working the bonus station
   int m_bonusStationPoints;

   // Number of points for submitting a Cabrillo File
   int m_cabrilloBonusPoints;

   // 'Owned' by ContestConfig object, do not delete
   DxccCountryManager *m_dxccCountryManager;

   // InState Stations get multipliers for DXCC Countries
   bool m_instateDxccMults;

   // Multipliers are per contest, per mode, per band
   MultipliersType m_multipliersType;

   // Bonus Stations are score multipliers
   bool m_bonusStationMultipliers;

   // This contest has power multipliers
   bool   m_powerMultipliers;
   double m_powerMultiplierQRP;
   double m_powerMultiplierLow;
   double m_powerMultiplierHigh;

   // In State Multipliers for this contest
   set<string> m_inStateMultipliers;

private:
   void DetermineInstateCountyCount();

   // Look in the other stations log file and validate the qso
   void ValidateQsos();

   void CheckForDuplicateQsos();

   // Find Multipliers and Count Qso Points
   void FindMultipliersAndCountQsoPoints();

   void BuildStationMap();

   void GenerateReports();

   void BuildMissingStations();

   void CalculateBonusPoints(const string& bonusStation, const int bonusStationPoints);

   void FindShowMeStations();

   void GenerateClubReports();

   // Create Qso Token Types
   void ProcessQsoTokenTypeData(ContestConfig *config);

   // If one station logs a callsign incorrectly, that station can get credit for the qso
   // and the station that logged the qso correctly can lose credict for the qso.
   // Find and fix these problems
   void FixQsosWithLoggingErrors();

   // Find the number of valid qso's each station had with a 1x1 callsign
   void CalculateStation1x1Count();

   // Calculate the total Qso's based on instate logs
   void CalculateTotalQsos();

   // Call a member function for each Station in parallel
   void CallStationMemberFnInParallel(PStationFunc pStnMemberFunc);
};
