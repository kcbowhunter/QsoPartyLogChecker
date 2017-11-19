
#pragma once

class Contest;
class ContestConfig;
class AllLocations;
class QsoTokenType;
class DxccCountryManager;
class MultipliersMgr;
class StationResults;
struct Category;
class BonusStationsPerBandPerMode;

#include "Qso.h"

enum StationCat      {
   eUnknownStationCat, eAnyStationCat, eFixedStationCat, eMobileStationCat,
   ePortableStationCat, eRoverStationCat, eRoverLimitedStationCat,
   eRoverUnlimitedStationCat, eExpeditionStationCat, eHQStationCat, eSchoolStationCat,
};

enum PowerCat        { eUnknownPowerCat, eAnyPowerCat, eHighPowerCat, eLowPowerCat, eQrpPowerCat};
enum OperatorCat     { eUnknownOperatorCat, eAnyOperatorCat, eSingleOperatorCat, eMultiOperatorCat, eCheckLogCat};
enum StationModeCat  { eUnknownStationModeCat, eAnyModeCat, eCwModeCat, eSsbModeCat, eMixedModeCat, eDigitalModeCat};
enum TxCat			 { eUnknownTxCat, eAnyTxCat, eSingleTxCat, eMultiTxCat, eSwlTxCat };

class Station
{
public:
	Station(Contest *contest);
	virtual ~Station();

	bool ReadLogFileCreateQsos(const string& logfilename);

	// Parses the lines from the log file
	// Extracts header information and creates qso objects
   bool ParseStringsCreateQsos(vector<string>& data, vector<QsoTokenType*>& tokenTypes);

	// Process one line from the file
   bool ParseStringCreateQso(string& str, bool& endoflog, vector<QsoTokenType*>& tokenTypes, list<string>& qsoLines);

	const string& StationCallsign() const { return m_callsign; }
	const string& StationCallsignLower() const { return m_callsignLower; }
   void SetStationCallsign(const string& s);

   string Operators() const { return m_operators; }

	bool InState() const { return m_instate; }
	bool OutState() const { return !m_instate; }
    void SetInState(const bool b) { m_instate = b; }

//	bool FixedLocation() const { return m_fixed; }
//	bool Mobile() const { return !m_fixed; }
   bool FixedLocation() const { return m_stationCat == eFixedStationCat; }
   bool Mobile() const { return m_stationCat == eMobileStationCat; }
   bool Portable() const { return m_stationCat == ePortableStationCat; }

   StationCat GetStationCat() { return m_stationCat; }
   void SetStationCat(StationCat cat) { m_stationCat = cat; }

   PowerCat   GetPowerCat() { return m_powerCat; }
   void       SetPowerCat(PowerCat cat) { m_powerCat = cat; }

   // Defaults to 1.0 if the contest does not use power multipliers
   double PowerMultiplier() const;

//	bool SingleOp() const { return m_singleop; }
//	bool MultiOp() const { return !m_singleop; }

   bool CheckLog() const { return m_stationOperatorCat == eCheckLogCat; }
   bool SingleOp() const { return m_stationOperatorCat == eSingleOperatorCat; }
   bool MultiOp() const { return m_stationOperatorCat == eMultiOperatorCat; }
   OperatorCat GetOperatorCat() const { return m_stationOperatorCat; }
   void        SetOperatorCat(OperatorCat cat) { m_stationOperatorCat = cat; }

   // Mode Category, cw, ssb, mixed
   bool CwMode() const { return m_stationModeCat == eCwModeCat; }
   bool SsbMode() const { return m_stationModeCat == eSsbModeCat; }
   bool MixedMode() const { return m_stationModeCat == eMixedModeCat; }
   StationModeCat GetStationModeCat() const { return m_stationModeCat; }

	string StationLocation() const { return m_stationLocation; }
	string GetContestState() const { return m_contestState; }

   void SetContestState(const string& state) { m_contestState = state; }
   void SetContestStateAbbrev(const string& abbrev) { m_contestStateAbbrev = abbrev; }
   string GetContestStateAbbrev() const { return m_contestStateAbbrev; }

   void SetCategory(Category* cat) { m_category = cat; }
   Category* GetCategory() const { return m_category; }
   bool HasCategory() const { return m_category != nullptr; }

   string GetCategoryAbbrev() const { return m_categoryAbbrev; }

	void WriteOneLineSummary();

   int NumberOfQsos() const { return (int)m_qsos.size(); }

   const set<string>& GetCountyAbbrevs() const { return m_countyAbbrevs; }

   // Write the report log for all qso's
   bool WriteLogReport(const string& filename);

   AllLocations* GetAllLocations() { return m_allLocations; }

   void ValidateQsos();

   // return the nth qso (0 based)
   Qso *GetQso(int num);

   int TotalMultipliers() const;
   int QsoPoints() const { return m_qsoPoints; }
   int BonusPoints() const { return m_bonusPoints; }

//   int Score() const { return MultiplierPoints() * QsoPoints() + BonusPoints(); }
   int Score() const;

   string Country() const { return m_country; }
   string State() const { return m_state; }
   string Province() const { return m_province; }

   // Number of ignored qsos because the station mode is different than the qso mode
   int GetIgnoredModeQsos() const { return m_ignoredModeQsos; }

   void SetCountry(const string& s) { m_country = s; }

   void AddQso(Qso* qso);

   bool ShowMe() const { return m_showme; }

   string GetOperatorName() const { return m_operatorName; }

   // Get the soapbox comments
   void GetSoapbox(list<string>& soapbox);

   string GetClub() const { return m_club; }
   string GetClubLower() const { return m_clubLower; }

   void ReserveQsos(int count);

   // Return the master moqp result string for this station
   string GetMOQPMasterResult();

   const string& LogFileName() const { return m_logFileName; }

   int GetValidCwQsos() const { return m_validCwQsos; }
   int GetValidPhoneQsos() const { return m_validPhoneQsos; }
   int GetValidDigitalQsos() const { return m_validDigitalQsos; }
   int GetPureDigitalScore() const { return m_pureDigitalScore; }
   int GetDigitalMultipliers() const { return m_digitalMultipliers; }

   int GetValidVhfQsos() const { return m_validVhfQsos; }

   bool AddChildStation(Station* pChild);

   // If one station logs a callsign incorrectly, that station can get credit for the qso
   // and the station that logged the qso correctly can lose credict for the qso.
   // Find and fix these problems
   int FixQsosWithLoggingErrors();

   // Return the qso's made with stations that are missing log files
   void GetQsosWithMissingLogFiles(vector<Qso*>& qsos);

   // Number of valid qso's with 1x1 stations
   int Get1x1Count() const       { return m_1x1Count; }
   int Get1x1CountUnique() const { return m_1x1CountUnique; }

   int GetNumberOfValidQsos() const { return m_numberValidQsos; }

   int GetNumberOfInvalidQsos() const {
	   return m_badCwQsos + m_badPhoneQsos + m_badDigitalQsos;
   }

   double GetInvalidQsoPercentage() const;

   int GetNumberOfTotalQsos() const { return (int)m_qsos.size(); }

   // Return true if the station category is a rover category
   static bool IsRoverStationCat(StationCat stationCat);

   int GetValidCountiesWorked() const { return (int) m_validCountiesWorked.size(); }

   // InState stations get multipliers for dx contacts based on ARRL DXCC countries
   bool InStateDxccMults();

   DxccCountryManager *GetDxccCountryManager();

   ContestConfig *GetContestConfig() const;
private:
	Contest *m_contest;

   vector<Qso> m_qsos;

   // If a station submits multiple log files, each log file generates a separate Station object.
   // Each station object is a member of the Contest vector of Stations.
   // The Contest has a map of callsign vs Station*, so only one station exists in the map.
   // m_childStations is a collection of stations owned by the station in the Contest station map 
   // to allow lookup of qso's when verifying qso's for a given callsign
   vector<Station*> m_childStations;

	string m_callsign;      // original callsign
	string m_callsignLower; // lower case callsign

	string m_stationLocation;

   string m_logFileName;

   // qso party state name and abbreviation
	string m_contestState;
	string m_contestStateAbbrev;

   // US State if ths is a usa station
   string m_state;

   // Province if this is a Candian station
   string m_province;

   // Country, 'dx' or actual country
   string m_country;

	bool m_instate;
//	bool m_fixed; // fixed location
   StationCat m_stationCat;

   // Station mode, cw, ssb or mixed
   StationModeCat m_stationModeCat;

   // Station Power Category
   PowerCat m_powerCat;

   // Transmitter Category
   TxCat m_txCat;
   string m_txCatString;

//	bool m_singleop;
   OperatorCat m_stationOperatorCat;

	map<string, string> m_datamap;

   // Number of qso's for each county for instate stations
   // This is the county for the instate station only
   // Includes valid and invalid qso's
   map<string, int> m_InstateCountyCount;

   // Lower case county abbreviations to verify counties in qso's
   set<string> m_countyAbbrevs;

   // Set of all valid counties worked during the contest
   set<string> m_validCountiesWorked;

   // Set of invalid counties worked (county does not appear in m_validCountiesWorked)
   set<string> m_invalidCountiesWorked;

   vector<QsoTokenType*> m_qsoTokenTypes;

   // Manage state / province / section / country data
   AllLocations *m_allLocations;

   // Stations that did not submit a logfile
   set<string> m_missingStations;

   // All Multipliers for this station (based on in state or out of state)
   set<string> m_allMultipliers;

   // The multipliers that worked by this station
   set<string> m_workedMultipliers;

   // MultipliersManager
   MultipliersMgr *m_multipliersMgr;

   // Number of points from qso's
   int m_qsoPoints;

   // Number of valid qso's
   int m_numberValidQsos;

   // Number of points from working bonus stations
   int m_bonusPoints;

   // True if the bonus station was worked
   bool m_workedBonusStation;

   // Number of valid and bad cw qsos
   int m_validCwQsos;
   int m_badCwQsos;

   // Number of valid and bad phone qsos
   int m_validPhoneQsos;
   int m_badPhoneQsos;

   // Number of valid and bad digital qsos
   int m_validDigitalQsos;
   int m_badDigitalQsos;

   // Pure Digital score; sum ( digital qso's ) x digital multipliers
   int m_pureDigitalScore;
   int m_digitalMultipliers;

   // map of showme letter vs count
   map<char, int> m_showmeMap;
   bool m_showme;

   // Station Category, i.e. Missouri Mobile, Single Op, Low Power, All Modes
   Category *m_category;

   // Category Abbreviation
   string m_categoryAbbrev;

   // Operator Name, from NAME: in Cabrillo Header
   string m_operatorName;

   // Operators: from Cab File
   string m_operators;

   // List of operators
   list<string> m_operatorsList;

   // email address if provided
   string m_email;

   // snail mail address
   string m_address;

   // Points per qso
   int m_cwPoints;
   int m_phonePoints;
   int m_digitalPoints;

   // Soapbox comments
   list<string> m_soapbox;

   // CLUB: entry from cab file
   string m_club;
   // Lower Case version of m_club
   string m_clubLower;

   // Category map from string to station category enum
   static map<string, StationCat> m_stationCategoryMap;

   // Category map from string to power category enum
   static map<string, PowerCat> m_powerCategoryMap;
   static map<PowerCat, string> m_powerCategoryMapInverse;
   static string GetPowerCategory(PowerCat powercat);

   // Category map from string to operator category enum
   static map<string, OperatorCat> m_operatorCategoryMap;
   static map<OperatorCat, string> m_operatorCategoryMapInverse;
   static string GetOperatorCategory(OperatorCat opcat);

   // Category map from string to station mode category enum
   static map<string, StationModeCat> m_stationModeCategoryMap;

   // Count of valid qso's for each band by mode
   vector<int> m_cwQsoCountByBand;
   vector<int> m_phQsoCountByBand;
   vector<int> m_ryQsoCountByBand;

   // Number of valid VHF Qso's (6m through 440mhz)
   int m_validVhfQsos;

   // number of valid qso's with 1x1 stations
   int m_1x1Count;

   // number of unique 1x1 stations worked
   int m_1x1CountUnique;

   // This log was submitted as a Cabrillo file
   // (not an xls or adi or paper log that had to be converted to a Cabrillo file)
   bool m_cabrilloFile;

   // Number of ignored qsos because the station mode is different than the qso mode
   int m_ignoredModeQsos;

   // Station Results used for regression testing
   StationResults *m_stationResults;

   BonusStationsPerBandPerMode *m_bonusStationsPerBandPerMode;

   // set of (lower case) counties activated by mobile stations
   set<string> m_mobileCountiesActivated;

   // Custom Report User Values Specified in Log Files
   map<string, string> m_customReportUserValues;

public:
   string GetCustomerReportUserValues(const string& key);

private:
   bool SetupStateProvinceCountry();

   bool SetupMultipliers();

   // Process single-op or mult-op
   void ProcessCategoryOperator(const string& value);

   // Process fixed or mobile/rover
   void ProcessCategoryStation(const string& value);

   // Process power category, qrp, low, high
   void ProcessCategoryPower(const string& value);

   // Process cw, ssb, mixed mode
   void ProcessCategoryMode(const string& value);

   // Process Transmitter Category
   void ProcessCategoryTransmitter(const string& value);

   // Return true if the station mode allows this qso to be included
   // i.e. if the station mode is ssb only, return false for a cw qso
   bool IncludeQso(QsoMode qsoMode);

public:
	// Return the data associated with dataName as a string
	string GetValue(const string& dataName);

	Contest *GetContest() const { return m_contest; }

   // Ignore the qso if the station mode category does not match the qso mode
   // i.e. if the station is ssb only category, ignore cw and digital qsos
   void FindIgnoredQsos();

   // Find the instate county count for each county that the instate station operated from
   void DetermineInstateCountyCount();

   // Count the number of valid VHF Qso's
   void CountValidVhfQsos();

   // Find the valid and invalid counties worked during the contest
   void DetermineValidAndInvalidCountiesWorked();

   void AddInstateCounties(map<string, int>& countyCount);

   void AddMissingStation(const string& callsign);

   Qso* FindAvailableQso(HamBand band, QsoMode mode, const string& callsign, const string& myLocation, const string& theirLocation);

   // Return the number of duplicate qso's found
   int CheckForDuplicateQsos();

   // Find multipiers worked in valid qso's
   bool FindMultipliers();

   bool CountQsoPoints();

   void GetMissingStationCallsigns(set<string>& missingStations, map<string, Station*>& m_stationMap);

   // Return the number of qso's added
   int AddMissingStationQsos(map<string, Station*>& missingStations);

   // Sort the qso's by serial number
   // This is used when generating missing logs
   void SortQsosBySerialNumber();

   // Sort the qso's by time
   // This is used when generating missing logs
   void SortQsosByTime();

   // Calculate bonus points for working the bonus station, once per contest
   // Also calculate the points for submitting a Cabrillo file
   void CalculateBonusPoints(const string& bonusStation, const int bonusStationPoints, int cabrilloBonusPoints);

   // This method is used by 1x1 stations to set the 'showme' letters into stations worked
   void ProcessShowMe1x1(map<string, Station*>& stationMap);

   // Add a letter from a showme station
   void AddShowmeLetter(char c);

   // Once all the stations are processed, set the m_showme boolean
   void SetShowmeStatus();

   // Count the number of valid qso's qith 1x1 stations
   void Calculate1x1Count();

   // Setup the static station maps
   static void SetupStaticData();

   // Setup the static station category map
   static void SetupStaticStationCategoryMap();

   // Setup the static power category map
   static void SetupStaticPowerCategoryMap();
   static void SetupStaticPowerCategoryMapInverse();

   // Setup the static operator category map
   static void SetupStaticOperatorCategoryMap();
   static void SetupStaticOperatorCategoryMapInverse();

   // Setup the static station mode category map
   static void SetupStaticStationModeCategoryMap();

   // Determine the counties activated by mobile stations
   void DetermineMobileCountiesActivated();

   int NumberOfMobileCountiesActivated() const;

   // Get the Station Category as a string
   string GetStationCatString();

   // Get the Station Mode as a string
   string GetStationModeString();

public:
   // Convert the token to a StationCat
   static StationCat ParseStationCategory(const string& token);

   // Convert the token to a PowerCat
   static PowerCat ParsePowerCategory(const string& token);

   // Convert the token to a OperatorCat
   static OperatorCat ParseOperatorCategory(const string& token);

   // Convert the token to a StationModeCat
   static StationModeCat ParseStationModeCategory(const string& token);

};

