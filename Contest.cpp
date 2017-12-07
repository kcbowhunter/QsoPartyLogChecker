

#include "stdafx.h"
#include "Contest.h"
#include "Station.h"
#include "ContestConfig.h"
#include "StringUtils.h"
#include "QsoTokenType.h"
#include "FreqToken.h"
#include "ModeToken.h"
#include "NameToken.h"
#include "DateToken.h"
#include "TimeToken.h"
#include "CallsignToken.h"
#include "RstToken.h"
#include "SerialNumberToken.h"
#include "LocationToken.h"
#include "ReportWriter.h"
#include "AllLocations.h"
#include "TextFile.h"
#include "CategoryMgr.h"
#include "ClubMgr.h"
#include "HtmlReporter.h"
#include "DxccCountryManager.h"
#include "MultipliersMgr.h"
#include "CustomReportManager.h"

#include "boost/algorithm/string.hpp"
using namespace boost;

Contest::Contest()
   :
   m_allLocations(nullptr),
   m_stationMap(),
   m_displayTimes(false),
   m_categoryMgr(nullptr),
   m_categoryMgr2(nullptr),
   m_totalQsos(0),
   m_totalValidQsos(0),
   m_missingLogsFolder(),
   m_htmlFolder(),
   m_resultsFolder(),
   m_logReportsFolder(),
   m_cwPoints(3),
   m_phonePoints(2),
   m_digitalPoints(3),
   m_cwAndDigitalAreTheSameMode(false),
   m_inStateMultsCounties(true),
   m_bonusStations(),
   m_bonusStationPoints(0),
   m_cabrilloBonusPoints(0),
   m_dxccCountryManager(nullptr),
   m_instateDxccMults(false),
   m_contestConfig(nullptr),
   m_multipliersType(eMultUnknown),
   m_bonusStationMultipliers(false),
   m_powerMultipliers(false),
   m_powerMultiplierQRP(1.0),
   m_powerMultiplierLow(1.0),
   m_powerMultiplierHigh(1.0),
   m_inStateMultipliers()
   {
   m_categoryMgr = new CategoryMgr();

   m_inStateClubs = new ClubMgr();
   m_outStateClubs = new ClubMgr();
   }

Contest::~Contest()
{
   double start = omp_get_wtime();

   bool parallel = true;

   double start1 = omp_get_wtime();
   if (m_stations.empty())
   { }
   else if (parallel)
   {
      Station **pStations = &m_stations[0];
      Station *station = nullptr;
      string logfilename;
      int stationCount = (int)m_stations.size();
      int i = 0;
      // delete all the log files in parallel
#pragma omp parallel for default(none) shared(stationCount, pStations) private(station, i)
      for (i = 0; i < stationCount; ++i)
      {
         station = pStations[i];
         delete station;
      }
   }
   else
   {
      auto iter = m_stations.begin();
      auto end = m_stations.end();
      for (; iter != end; ++iter)
      {
         Station *station = *iter;
         delete station;
      }
   }
   m_stations.clear();

   double finish1 = omp_get_wtime();
   double time = (finish1 - start1)*1000.0;
   if (m_displayTimes)
   {
      printf("Contest dtor: Time to delete stations: %7.2f ms\n", time);
   }

   // ***************************************************************
   // Delete the missing stations
   start1 = omp_get_wtime();
   vector<Station*> missingStationsVector(m_missingStationsMap.size());
   auto missingIter = m_missingStationsMap.begin();
   auto missingIterEnd = m_missingStationsMap.end();
   int i = 0;
   for (; missingIter != missingIterEnd; ++missingIter)
   {
      Station *s = (*missingIter).second;
//      delete s;
      missingStationsVector[i++] = s;
   }

   Station **pStations = missingStationsVector.empty() ? nullptr : &missingStationsVector[0];
   Station *station = nullptr;

   auto missingStationsCount = missingStationsVector.size();
#pragma omp parallel for default(none) shared(missingStationsCount, pStations) private(i, station)
   for (i = 0; i < (int)missingStationsCount; ++i)
   {
      station = pStations[i];
      delete station;
   }
   m_missingStationsMap.clear();
   missingStationsVector.clear();
   finish1 = omp_get_wtime();
   time = (finish1 - start1)*1000.0;
   if (m_displayTimes)
   {
      printf("Contest dtor: Time to delete missing stations: %7.2f ms\n", time);
   }

   if (m_allLocations != nullptr)
   {
      delete m_allLocations;
      m_allLocations = nullptr;
   }

   if (m_categoryMgr != nullptr)
   {
      delete m_categoryMgr;
      m_categoryMgr = nullptr;
   }

   if (m_categoryMgr2 != nullptr)
   {
      delete m_categoryMgr2;
      m_categoryMgr2 = nullptr;
   }

   double finish = omp_get_wtime();
   double t1 = (finish - start) * 1000.0;
   if (m_displayTimes)
   {
      printf("Contest dtor, elapsed time=%7.2f ms\n", t1);
   }

   // Delete the club managers
   delete m_inStateClubs;
   delete m_outStateClubs;

   // Do not delete the DxccCountryManager, it is deleted by the ContestConfig object
   m_dxccCountryManager = nullptr;
   m_contestConfig = nullptr;
}

bool Contest::ProcessConfigData(ContestConfig *config)
{
	m_contestConfig = config;
    bool status    = true;
	m_title       = config->GetTitle();
	m_titleAbbrev = config->GetAbbrev();
	m_state       = config->GetState();
	m_stateAbbrev = config->GetStateAbbrev();
	StringUtils::ToLower(m_stateAbbrev);

	// Multipliers are per contest, per mode, per band
	m_multipliersType = config->GetMultipliersType();

	// Bonus stations are score multipliers
	m_bonusStationMultipliers = config->GetBonusStationMultipliers();

   config->GetCountyAbbrevs(m_countyAbbrevs);
   config->GetCountyMap(m_countyMap);

   if (config->GetBonusCountyPoints() > 0)
   {
	   set<string> bonusCounties = config->GetBonusCounties();
	   status = CheckBonusCounties(m_countyAbbrevs, bonusCounties);
   }

   m_dxccCountryManager = config->GetDxccCountryManager();
   m_instateDxccMults   = config->InstateDxccMults();

   // Create the qso token types
   ProcessQsoTokenTypeData(config);

   if (m_tokenTypes.empty())
   {
      //   AddTokenTypesForMOQP();
      AddTokenTypesForKSQP();
   }

   m_resultsFolder = config->GetFoldersSectionValue("results");
   StringUtils::AddTrailingSlashIfNeeded(m_resultsFolder);

   m_logReportsFolder = config->GetFoldersSectionValue("logreports");
   StringUtils::AddTrailingSlashIfNeeded(m_logReportsFolder);

   m_missingLogsFolder = config->GetFoldersSectionValue("missinglogs");
   StringUtils::AddTrailingSlashIfNeeded(m_missingLogsFolder);

   m_htmlFolder = config->GetFoldersSectionValue("html");
   StringUtils::AddTrailingSlashIfNeeded(m_htmlFolder);

   m_inStateMultsCounties = config->GetInStateMultsCounties();

   m_allLocations = new AllLocations();
   m_allLocations->Setup(config);

   m_bonusStations = config->GetBonusStations();
   m_bonusStationPoints = config->GetBonusStationPoints();
   m_cabrilloBonusPoints = config->GetCabrilloBonusPoints();

   m_powerMultipliers = config->HasPowerMultipliers();
   m_powerMultiplierQRP = config->GetPowerMultiplierQRP();
   m_powerMultiplierLow = config->GetPowerMultiplierLow();
   m_powerMultiplierHigh = config->GetPowerMultiplierHigh();


   // Process Categories (usually Plaques)
   if (status)
   {
      const list<list<string>>& categoryData = config->GetCategoryData();
      status = m_categoryMgr->CreateCategories(categoryData);
      if (!status)
      {
         printf("Error Creating Categories\n");
         const string& catFileName = config->GetCategoriesFileName();
         printf("   Please check categories file %s\n\n", catFileName.c_str());
      }
   }

   // Process Categories2 (usually Certificates)
   if (status)
   {
      const list<list<string>>& categoryData = config->GetCategory2Data();
      if (!categoryData.empty())
      {
         if (m_categoryMgr2 == nullptr)
         {
            m_categoryMgr2 = new CategoryMgr();
         }

         status = m_categoryMgr2->CreateCategories(categoryData);
         if (!status)
         {
            printf("Error Creating Secondary Categories\n");
            const string& catFileName = config->GetCategories2FileName();
            printf("   Please check categories file %s\n\n", catFileName.c_str());
         }
      }
   }

   // Scoring
   if (config->GetCwPoints() > 0)
   {
      m_cwPoints = config->GetCwPoints();
   }

   if (config->GetPhonePoints() > 0)
   {
      m_phonePoints = config->GetPhonePoints();
   }

   if (config->GetDigitalPoints() > 0)
   {
      m_digitalPoints = config->GetDigitalPoints();
   }
   
   // Setup InState Multipliers
   if (status)
   {
	   status = SetupInStateMultipliers();
   }

   return status;
}

// Create Qso Token Types
void Contest::ProcessQsoTokenTypeData(ContestConfig *config)
{
   list<string> tokenTypes;
   config->GetQsoData(tokenTypes);
   if (tokenTypes.empty())
      return;

   string temp;
   for (string str : tokenTypes)
   {
      if (str.empty()) continue;
      temp = str;

      temp = StringUtils::RemoveComment(temp, '#');
      trim(temp);
      if (temp.empty()) continue;

      if (temp == "freq")
      {
         m_tokenTypes.push_back(new FreqToken());
      }
      else if (temp == "mode")
      {
         m_tokenTypes.push_back(new ModeToken());
      }
      else if (temp == "date")
      {
         m_tokenTypes.push_back(new DateToken());
      }
      else if (temp == "time")
      {
         m_tokenTypes.push_back(new TimeToken());
      }
      else if (temp == "callsign")
      {
         m_tokenTypes.push_back(new CallsignToken());
      }
      else if (temp == "rst")
      {
         m_tokenTypes.push_back(new RstToken());
      }
      else if (temp == "serial" || temp == "serialnumber")
      {
         m_tokenTypes.push_back(new SerialNumberToken());
      }
      else if (temp == "location")
      {
         m_tokenTypes.push_back(new LocationToken());
      }
      else if (temp == "name")
      {
         m_tokenTypes.push_back(new NameToken());
      }
      else
      {
         printf("Error:: Contest::ProcessQsoTokenTypeData Unknown token: %s\n", temp.c_str());
      }
   }
}

void Contest::AddTokenTypesForMOQP()
{
   m_tokenTypes.push_back(new FreqToken());
   m_tokenTypes.push_back(new ModeToken());
   m_tokenTypes.push_back(new DateToken());
   m_tokenTypes.push_back(new TimeToken());

   m_tokenTypes.push_back(new CallsignToken());
   m_tokenTypes.push_back(new RstToken());
   m_tokenTypes.push_back(new SerialNumberToken());
   m_tokenTypes.push_back(new LocationToken());

   m_tokenTypes.push_back(new CallsignToken());
   m_tokenTypes.push_back(new RstToken());
   m_tokenTypes.push_back(new SerialNumberToken());
   m_tokenTypes.push_back(new LocationToken());
}

void Contest::AddTokenTypesForKSQP()
{
   m_tokenTypes.push_back(new FreqToken());
   m_tokenTypes.push_back(new ModeToken());
   m_tokenTypes.push_back(new DateToken());
   m_tokenTypes.push_back(new TimeToken());

   m_tokenTypes.push_back(new CallsignToken());
   m_tokenTypes.push_back(new RstToken());
   m_tokenTypes.push_back(new LocationToken());

   m_tokenTypes.push_back(new CallsignToken());
   m_tokenTypes.push_back(new RstToken());
   m_tokenTypes.push_back(new LocationToken());
}

// Clone the token types defined for the contest to the vector
void Contest::CloneTokenTypes(vector<QsoTokenType*>& tokenTypes)
{
   tokenTypes.clear();
   tokenTypes.resize(m_tokenTypes.size());

   int i = 0;
   for (QsoTokenType* tt : m_tokenTypes)
   {
      QsoTokenType *clone = tt->Clone();
      tokenTypes[i++] = clone;
   }
}


// all county abbrevs are lower case
void Contest::GetCountyAbbrevs(set<string>& countyAbbrevs)
{
   countyAbbrevs.clear();
   for (string abbrev : m_countyAbbrevs)
   {
      countyAbbrevs.insert(abbrev);
   }
}

// This method processes all the log files
bool Contest::ProcessLogs(vector<string>& logFileNames)
{
	if (logFileNames.empty())
		return true;

	int i = 0;
	// create the stations
	m_stations.resize(logFileNames.size());
	for (string str : logFileNames)
	{
		Station *station = new Station(this);
		m_stations[i++] = station;
	}

	int stationCount = (int)logFileNames.size();
	Station **pStations = &m_stations[0];
	Station *station = nullptr;
	string logfilename;
	// read all the log files in parallel
	double start = omp_get_wtime();
#pragma omp parallel for default(none) shared(stationCount, pStations, logFileNames) private(station, logfilename, i)
	for (i = 0; i < stationCount; ++i)
	{
		//      printf("omp tid=%2d proc=%2d\n", omp_get_thread_num(), GetCurrentProcessorNumber());
		station = pStations[i];
		logfilename = logFileNames[i];
		//      printf("Processing log file: %s\n", logfilename.c_str());
		station->ReadLogFileCreateQsos(logfilename);
		station->DetermineInstateCountyCount();
	}

	double finish = omp_get_wtime();

	double time = (finish - start)*1000.0;
	if (m_displayTimes)
	{
		printf("Elapsed time to read and parse files is %7.4f ms\n", time);
	}

	printf("BuildStationMap\n");
	BuildStationMap();

	// Look in the other stations log file and validate the qso
	if (m_contestConfig->GetValidateQsos())
	{
		printf("Validate Qso's\n");
		ValidateQsos();
	}
	else
	{
		printf("Skipping Validate Qso's in Submitted Logs Step\n");
	}

	printf("Check for Duplicate QSO's\n");
	CheckForDuplicateQsos();

	// If one station logs a callsign incorrectly, that station can get credit for the qso
	// and the station that logged the qso correctly can lose credict for the qso.
	// Find and fix these problems
	printf("Fix Qso's With Logging Errors\n");
	FixQsosWithLoggingErrors();

	// Once all Errors are found, finally count multipliers and qso's
	FindMultipliersAndCountQsoPoints();

	// Find the number of valid qso's each station had with a 1x1 callsign
	// CalculateStation1x1Count();
	CallStationMemberFnInParallel(&Station::Calculate1x1Count);

	CallStationMemberFnInParallel(&Station::DetermineMobileCountiesActivated);

	// Bonus Stations can add score points or be score multipliers
	// If bonus station points are zero, then the bonus stations are
	// score multipliers
	if (m_bonusStationPoints > 0)
	{
		for (string bonusStation : m_bonusStations)
		{
			StringUtils::ToLower(bonusStation);

			Station *s = GetStation(bonusStation);
			if (s != nullptr)
			{
				s->SetBonusStation(true);
			}
			else
			{
				printf("Error: Bonus Station %s not found\n", bonusStation.c_str());
			}

			CalculateBonusPoints(bonusStation, m_bonusStationPoints);
		}
	}

	// Calculate the county bonus points for each station
	ContestConfig *contestConfig = GetContestConfig();
	int countyBonusPoints = contestConfig->GetBonusCountyPoints();
	if (countyBonusPoints > 0)
	{
		const set<string>& bonusCounties = contestConfig->GetBonusCounties();
		for (Station *s : m_stations)
		{
			s->CalculateBonusCountyPoints(bonusCounties, countyBonusPoints);
		}
	}


	bool useCategoryAbbrevs = true;
	bool assignCategoryToStation = true;
	bool matchFirstCategory = false;
	m_categoryMgr->DetermineStationCategories(m_stations, useCategoryAbbrevs, assignCategoryToStation, matchFirstCategory);

	if (m_categoryMgr2 != nullptr)
	{
		m_categoryMgr2->DetermineStationCategories(m_stations, useCategoryAbbrevs, assignCategoryToStation, matchFirstCategory);
	}

	// Determine categories for custom reports
	CustomReportManager *customReportMgr = m_contestConfig->GetCustomReportManager();
	customReportMgr->DetermineStationCategories(m_stations);

	bool writeOneLineSummary = false;
	if (writeOneLineSummary)
	{
		for (i = 0; i < stationCount; ++i)
		{
			station = pStations[i];
			station->WriteOneLineSummary();
		}
	}

	// Write Station Log Reports
	start = omp_get_wtime();
#pragma omp parallel for default(none) shared(stationCount, pStations, logFileNames) private(station, logfilename, i)
	for (i = 0; i < stationCount; ++i)
	{
		station = pStations[i];
		logfilename = m_logReportsFolder + station->StationCallsign();
		logfilename = logfilename + ".txt";
		station->WriteLogReport(logfilename);
	}

	finish = omp_get_wtime();
	time = (finish - start)*1000.0;
	if (m_displayTimes)
	{
		printf("Elapsed time to generate station log reports is %7.4f ms\n", time);
	}

	if (boost::iequals(m_stateAbbrev, "MO"))
	{
		FindShowMeStations();
	}


	CalculateTotalQsos();
	printf("There were %d total Qso's\n", m_totalQsos);
	printf("There were %d valid Qso's\n", m_totalValidQsos);

	printf("Generate Reports...\n");
	GenerateReports();

	if (m_contestConfig->GenerateMissingLogs())
	{
		printf("BuildMissingStations...\n");
		double startMissing = omp_get_wtime();
		BuildMissingStations();
		double endMissing = omp_get_wtime();
		double time = (endMissing - startMissing) * 1000.0;
		printf("\nTime to Build Missing Station Logs: %7.2f ms\n", time);
	}
	else
		printf("Skipping Missing Station Logs Generation Step\n");

   printf("GenerateClubReports...\n");
   GenerateClubReports();

   // Generate HTML Files
   bool genHtml = false;
   if (genHtml)
   {
      start = omp_get_wtime();
      HtmlReporter htmlReporter;
      htmlReporter.GenerateHtmlReports(m_stationMap, m_missingStationsMap, m_htmlFolder);
      finish = omp_get_wtime();
      time = (finish - start)*1000.0;
      if (m_displayTimes)
      {
         printf("Elapsed time to generate station HTML files is %7.4f ms\n", time);
      }
   }

   printf("Contest::ProcessLogs exit\n");

	return true;
}

// NOTE : this method must be called after all the stations have been scored
void Contest::GenerateClubReports()
{
   // First add all the stations with clubs to a club manager for instate and out of state clubs
   for (Station *s : m_stations)
   {
      const string clubName = s->GetClub();
      if (clubName.empty())
         continue;

//      string callsign = s->StationCallsign();
//      printf("Station: %10s  Club: %s\n", callsign.c_str(), clubName.c_str());

      if (s->InState())
      {
         m_inStateClubs->AddStation(s);
      }
      else
      {
         m_outStateClubs->AddStation(s);
      }
   }

   // Score the Clubs
   m_inStateClubs->CalculateScores();
   m_outStateClubs->CalculateScores();

   string fileName = m_resultsFolder +"Score-InStateClubs.txt";
   m_inStateClubs->WriteReport(fileName, "In State Clubs");

   fileName = m_resultsFolder + "Score-OutOfStateClubs.txt";
   m_outStateClubs->WriteReport(fileName, "Out of State Clubs");
}

void Contest::GenerateReports()
{
   double start = omp_get_wtime();

   vector<Station*> inStateStations;
   vector<Station*> outStateStations;
   inStateStations.reserve(m_stations.size());
   outStateStations.reserve(m_stations.size());

   for (Station *s : m_stations)
   {
      if (s->InState())
         inStateStations.push_back(s);
      else
         outStateStations.push_back(s);
   }

//   printf("Contest::GenerateReports InOutStateStations\n");
   // update the contest callsign/station map
   string filename = m_resultsFolder + "InStateStations.txt";
   ReportWriter::InOutStateStations(true, filename, m_stations);

//   printf("Contest::GenerateReports InOutStateStations\n");
   filename = m_resultsFolder + "OutOfStateStations.txt";
   ReportWriter::InOutStateStations(false, filename, m_stations);

//   printf("Contest::GenerateReports DetermineInStateCountyCount\n");
   DetermineInstateCountyCount();
   filename = m_resultsFolder + "CountyCount.txt";
   ReportWriter::WriteCountyCount(m_InstateCountyCount, m_countyMap, filename);

//   printf("Contest::GenerateReports ScoreAllStations\n");
   filename = m_resultsFolder + "Score-AllStations.txt";
   string title = "Sorted score for all stations in the contest";
   ReportWriter::WriteStationsByScore(filename, m_stations, title);

//   printf("Contest::GenerateReports ScoreUSAStations\n");
   filename = m_resultsFolder + "Score-USAStations.txt";
   title = "Sorted score for USA stations in the contest";
   ReportWriter::WriteStationsByScoreAndCountry("usa", filename, m_stations, title);

//   printf("Contest::GenerateReports ScoreCanadaStations\n");
   filename = m_resultsFolder + "Score-CanadaStations.txt";
   title = "Sorted score for Canada stations in the contest";
   ReportWriter::WriteStationsByScoreAndCountry("canada", filename, m_stations, title);

//   printf("Contest::GenerateReports ScoreDXStations\n");
   filename = m_resultsFolder + "Score-DXStations.txt";
   title = "Sorted score for DX stations in the contest";
   ReportWriter::WriteStationsByScoreAndCountry("dx", filename, m_stations, title);

//   printf("Contest::GenerateReports StationNoCountry\n");
   string emptystring;
   filename = m_resultsFolder + "Score-NoCountryStations.txt";
   title = "Sorted score for stations with unknown countries in the contest";
   ReportWriter::WriteStationsByScoreAndCountry(emptystring, filename, m_stations, title);

//   printf("Contest::GenerateReports InStateStations\n");
   filename = m_resultsFolder + "Score-InStateStations.txt";
   title = "Sorted Score for InState Stations";
   ReportWriter::WriteStationsByScoreAndInState(true, filename, m_stations, title);

   filename = m_resultsFolder + "Score-OutOfStateStations.txt";
   title = "Sorted Score for Out of State Stations";
   ReportWriter::WriteStationsByScoreAndInState(false, filename, m_stations, title);

//   printf("Contest::GenerateReports StateSummary\n");
   filename = m_resultsFolder + "Score-StateSummary.txt";
   ReportWriter::WriteStateSummaryReport(filename, m_stations, m_allLocations);

//   ReportWriter::WriteMobileStationsByScore(m_resultsFolder, m_stations);
   filename = m_resultsFolder + "Score-Categories.txt";
   ReportWriter::WriteCategorySummary(this, filename, m_categoryMgr, 1);

//   printf("Contest::GenerateReports KSQP-Categories\n");
   filename = m_resultsFolder + "KSQP-Categories.txt";
   ReportWriter::WriteCategorySummary(this, filename, m_categoryMgr, 2);

   filename = m_resultsFolder + "Score-Categories3.txt";
   ReportWriter::WriteCategorySummaryInOrder(this, filename, m_categoryMgr);

   if (m_categoryMgr2 != nullptr)
   {
      filename = m_resultsFolder + "Score-Categories2.txt";
      ReportWriter::WriteCategorySummaryInOrder(this, filename, m_categoryMgr2);
   }

   filename = m_resultsFolder + "Soapbox.txt";
   ReportWriter::WriteSoapBoxComments(filename, m_stations);

   filename = m_resultsFolder + "MOQPMasterResults.txt";
   ReportWriter::WriteMOQPMasterResults(filename, m_stations);

//   printf("Contest::GenerateReports 1x1StationCounts\n");
   filename = m_resultsFolder + "1x1StationCounts.txt";
   ReportWriter::Write1x1StationCounts(filename, m_stations);

   title = "In State Digital Results - Sorted by Number of Digital Qso's";
   filename = m_resultsFolder + "Score-InState-Digital.txt";
   ReportWriter::WriteDigitalResults(filename, inStateStations, title);

   title = "Out of State Digital Results - Sorted by Number of Digital Qso's";
   filename = m_resultsFolder + "Score-OutState-Digital.txt";
   ReportWriter::WriteDigitalResults(filename, outStateStations, title);

   // 'Pure' digital results consider only digital qso's and multipliers from digital qso's
   filename = m_resultsFolder + "Score-PureDigitalResults.txt";
   ReportWriter::WritePureDigitalResults(filename, inStateStations, outStateStations);

   title = "Valid Counties Worked Results - All Stations";
   filename = m_resultsFolder + "Score-CountiesWorked-AllStations.txt";
   ReportWriter::WriteCountiesWorkedResults(filename, m_stations, title);

   title = "Valid Counties Worked Results - InState Stations";
   filename = m_resultsFolder + "Score-CountiesWorked-InStateStations.txt";
   ReportWriter::WriteCountiesWorkedResults(filename, inStateStations, title);

   title = "Valid Counties Worked Results - Out of State Stations";
   filename = m_resultsFolder + "Score-CountiesWorked-OutStateStations.txt";
   ReportWriter::WriteCountiesWorkedResults(filename, outStateStations, title);

   title = "Valid VHF Qso's - In State Stations";
   filename = m_resultsFolder + "Score-VHFQsos-InStateStations.txt";
   ReportWriter::WriteVhfResults(filename, inStateStations, title);

   title = "Valid VHF Qso's - Out of State Stations";
   filename = m_resultsFolder + "Score-VHFQsos-OutStateStations.txt";
   ReportWriter::WriteVhfResults(filename, outStateStations, title);

   title = "Stations Sorted by Invalid Qso Count";
   filename = m_resultsFolder + "Stations-SortedByInvalidQsoCount.txt";
   ReportWriter::WriteStationsSortedByInvalidQsos(filename, m_stations, title);

   title = "Stations Sorted by Percentage of Invalid Qso Count";
   filename = m_resultsFolder + "Stations-SortedByInvalidQsoCountPercentage.txt";
   ReportWriter::WriteStationsSortedByInvalidQsosByPercentage(filename, m_stations, title);

   // Generate Custom Reports
   CustomReportManager *customReportManager = m_contestConfig->GetCustomReportManager();
   customReportManager->GenerateReportsForAllStations(m_resultsFolder, m_stations);
   customReportManager->GenerateCustomReports(m_resultsFolder, m_stations);

//   printf("Contest::GenerateReports After 1x1StationCounts\n");
   double finish = omp_get_wtime();
   double time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to generate reports: %7.2f ms\n", time);
   }
}

void Contest::ValidateQsos()
{
   size_t stationCount = m_stations.size();
   Station **pStations = &m_stations[0];
   Station *station = nullptr;
   double start = omp_get_wtime();
   size_t i = 0;
   string callsign;

// ***** THIS LOOP MUST BE RUN ON A SINGLE THREAD, IT IS NOT DESIGNED TO BE RUN IN PARALLEL *****
   // NOTE: station->ValidateQsos does not check for duplicates
   for (i = 0; i < stationCount; ++i)
   {
      station = pStations[i];

      callsign = station->StationCallsign();
      const char* call = callsign.empty() ? "<MissingCallsign>" : callsign.c_str();

//      printf("Validate qso's for station: %s\n", call);

      station->ValidateQsos();
   }


   double finish = omp_get_wtime();
   double time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Elapsed time to validate qso's is %7.4f ms\n", time);
   }
}

void Contest::CheckForDuplicateQsos()
{
	size_t stationCount = m_stations.size();
	Station **pStations = &m_stations[0];
	Station *station = nullptr;
	double start = omp_get_wtime();
	size_t i = 0;
	string callsign;

	// Check for duplicates (valid qso's only)
	for (i = 0; i < stationCount; ++i)
	{
		station = pStations[i];
		station->CheckForDuplicateQsos();
	}
}

// Find Multipliers and Count Qso Points
void Contest::FindMultipliersAndCountQsoPoints()
{
   size_t stationCount = m_stations.size();
   Station **pStations = &m_stations[0];
   Station *station = nullptr;
   int i = 0;

   // Find ignored qso's
   for (i = 0; i < (int)stationCount; ++i)
   {
      station = pStations[i];
      station->FindIgnoredQsos();
   }

   // Determine Multipliers (valid qso's only)
   for (i = 0; i < (int)stationCount; ++i)
   {
      station = pStations[i];
      station->FindMultipliers();
   }

   // Count qso points for valid qso's
   for (i = 0; i < (int)stationCount; ++i)
   {
      station = pStations[i];
      station->CountQsoPoints();
      station->DetermineValidAndInvalidCountiesWorked();
      station->CountValidVhfQsos();
   }
}

void Contest::BuildStationMap()
{
   for (Station *station : m_stations)
   {
      string callsign = station->StationCallsignLower();
      if (callsign.empty())
      {
         const string& logfilename = station->LogFileName();
         printf("Error: Missing CALLSIGN in Cabrillo File for log file '%s'\n", logfilename.c_str());
      }
      else
      {
         auto iter = m_stationMap.find(callsign);
         if (iter == m_stationMap.end())
         {
            m_stationMap[callsign] = station;
         }
         else
         {
            printf("Warning: Multiple logs submitted for station: %s\n", callsign.c_str());
            Station *s = (*iter).second;
            s->AddChildStation(station);
         }
      }
   }
}

Station *Contest::GetStation(const string& callsign) const
{
   string call(callsign);

   StringUtils::ToLower(call);

   auto iter = m_stationMap.find(call);

   return iter == m_stationMap.end() ? nullptr : (*iter).second;
}

void Contest::DetermineInstateCountyCount()
{
   // Initialize all counties to a count of 0 qso's
   m_InstateCountyCount.clear();
   for (string county : m_countyAbbrevs)
   {
      m_InstateCountyCount[county] = 0;
   }

   for (Station *station : m_stations)
   {
      station->AddInstateCounties(m_InstateCountyCount);
   }
}

void Contest::BuildMissingStations()
{
   double startTotal = omp_get_wtime();
   // Callsigns of missing stations
   set<string> missingCallsigns;

   double start = omp_get_wtime();
   // Build up a set of callsigns
   for (Station* s : m_stations)
   {
      s->GetMissingStationCallsigns(missingCallsigns, m_stationMap);
   }
   double finish = omp_get_wtime();
   double time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to Get Missing Station Callsigns: %7.2fms\n", time);
   }

   // Create the missing stations
   start = omp_get_wtime();
   for (string callsign : missingCallsigns)
   {
      Station *s = new Station(this);
      s->SetStationCallsign(callsign);
      s->ReserveQsos(20);

      StringUtils::ToLower(callsign);
      m_missingStationsMap[callsign] = s;
   }
   finish = omp_get_wtime();
   time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to create %zu missing stations: %7.2f ms\n", missingCallsigns.size(), time);
   }

   // Copy the qso's from the submitted logs to the missing (non submitted) logs
   start = omp_get_wtime();
   int qsoCount = 0;
   for (Station* s : m_stations)
   {
//      printf("Add missing qso's for station %s\n", s->Callsign().c_str());
      qsoCount += s->AddMissingStationQsos(m_missingStationsMap);
   }
   finish = omp_get_wtime();
   time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to copy %d qso's to the missing stations: %7.2f ms\n", qsoCount, time);
   }

   int i = 0;
   int stationCount = (int)m_missingStationsMap.size();
   vector<Station*> stations(stationCount);
   for (auto iter : m_missingStationsMap)
   {
      Station* s = iter.second;
      stations[i++] = s;
   }

   Station **pStations = stations.empty() ? nullptr : &stations[0];
   Station *station = nullptr;

   // Sort the Missing log files qso's by serial number
   start = omp_get_wtime();
//   for (auto iter : m_missingStations)
//#pragma omp parallel for default(none) shared(stationCount, pStations) private(station, i)
   for (i = 0; i < stationCount; ++i)
   {
//      Station* s = iter.second;
      station = pStations[i];
//      station->SortQsosBySerialNumber();
      station->SortQsosByTime();
   }
   finish = omp_get_wtime();
   time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to sort the missing stations qso's by serial number: %7.2f ms\n", time);
   }

   string logfilename;
   start = omp_get_wtime();
//#pragma omp parallel for default(none) shared(stationCount, pStations) private(station, logfilename, i)
   for (i = 0; i < stationCount; ++i)
   {
      station = pStations[i];
      logfilename = m_missingLogsFolder + station->StationCallsign();
      logfilename = logfilename + ".txt";
      station->WriteLogReport(logfilename);
   }

   finish = omp_get_wtime();
   time = (finish - start)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to generate missing station log reports is %7.2f ms\n", time);
   }

   double finishTotal = omp_get_wtime();
   time = (finishTotal - startTotal) * 1000.0;
   if (m_displayTimes)
   {
      printf("Total time to process missing logs: %7.2f ms\n", time);
   }
}

void Contest::CalculateBonusPoints(const string& bonusStation, const int bonusStationPoints)
{
	if (bonusStationPoints > 0)
	{
		for (Station *s : m_stations)
		{
			s->CalculateBonusPoints(bonusStation, bonusStationPoints, m_cabrilloBonusPoints);
		}
	}
}

// If one station logs a callsign incorrectly, that station can get credit for the qso
// and the station that logged the qso correctly can lose credict for the qso.
// Find and fix these problems
void Contest::FixQsosWithLoggingErrors()
{
   int qsoCount = 0;
   double startTime = omp_get_wtime();
   for (Station *s : m_stations)
   {
      qsoCount += s->FixQsosWithLoggingErrors();
   }

   double finishTime = omp_get_wtime();
   double time = (finishTime - startTime)*1000.0;
   if (m_displayTimes)
   {
      printf("Time to Fix %d Qsos With Logging Errors: %7.2f ms\n", qsoCount, time);
   }
}


// Find the stations in the contest that spell 'SHOWME' using the 1x1 stations
// Do this after the bonus station calc so that stations can use the bonus points to indicate that W0MA was worked
void Contest::FindShowMeStations()
{
   list<string> callsigns;
   callsigns.push_back("n0s");
   callsigns.push_back("n0h");
   callsigns.push_back("n0o");
   callsigns.push_back("n0w");
   callsigns.push_back("n0m");
   callsigns.push_back("n0e");

   callsigns.push_back("k0s");
   callsigns.push_back("k0h");
   callsigns.push_back("k0o");
   callsigns.push_back("k0w");
   callsigns.push_back("k0m");
   callsigns.push_back("k0e");

   callsigns.push_back("w0s");
   callsigns.push_back("w0h");
   callsigns.push_back("w0o");
   callsigns.push_back("w0w");
   callsigns.push_back("w0m");
   callsigns.push_back("w0e");

   for (string callsign : callsigns)
   {
      auto iter = m_stationMap.find(callsign);
      if (iter == m_stationMap.end())
      {
         printf("Error: unable to find 1x1 station %s\n", callsign.c_str());
         continue;
      }

      Station *s = (*iter).second;
      char c = callsign.at(2);

      s->ProcessShowMe1x1(m_stationMap);
   }

   for (Station *s : m_stations)
   {
      s->SetShowmeStatus();
   }

   TextFile file;

   int i = 0;
   char buffer[80];
   for (Station *s : m_stations)
   {
      if (s->ShowMe())
      {
//         file.AddLine(s->StationCallsign());
         const string opName = s->GetOperatorName();
         const char* pName = opName.empty() || opName.length() < 1 ? "Missing Name" : opName.c_str();

         sprintf_s(buffer, 80, "%3d) %15s, %s", ++i, s->StationCallsign().c_str(), pName);
         file.AddLine(buffer);

         if ((i + 5) % 5 == 0)
         {
            file.AddLine(" ");
         }
      }
   }

   string filename = m_resultsFolder + "ShowMe.txt";
   file.Write(filename);
}

// Find the number of valid qso's each station had with a 1x1 callsign
void Contest::CalculateStation1x1Count()
{
   int i = 0;
   Station **pStations = &m_stations[0];
   int stationCount = (int)m_stations.size();
   Station* station = nullptr;

#pragma omp parallel for default(none) shared(stationCount, pStations) private(station, i)
   for (i = 0; i < stationCount; ++i)
   {
      station = pStations[i];
      station->Calculate1x1Count();
   }
}

// Find the member function pStnMemberFunc for each Station in parallel
// This function takes no arguments
void Contest::CallStationMemberFnInParallel(PStationFunc pStnMemberFunc)
{
   int i = 0;
   Station **pStations = &m_stations[0];
   int stationCount = (int)m_stations.size();
   Station* station = nullptr;

//   PStationFunc func = &Station::Calculate1x1Count;

#pragma omp parallel for default(none) shared(stationCount, pStations, pStnMemberFunc) private(station, i)
   for (i = 0; i < stationCount; ++i)
   {
      station = pStations[i];
//      station->Calculate1x1Count();
//      station->*stnMemberFunc();
//      (station->*func)();
      (station->*pStnMemberFunc)();
   }
}


// Calculate the total Qso's based on instate logs
void Contest::CalculateTotalQsos()
{
   m_totalQsos = 0;
   m_totalValidQsos = 0;
   for (Station *s : m_stations)
   {
      m_totalQsos += s->GetNumberOfTotalQsos();
      m_totalValidQsos += s->GetNumberOfValidQsos();
   }
}


// Setup In State Multipliers
// (Out of state multipliers are Counties)
// Note that this method does not setup dxcc multipliers
bool Contest::SetupInStateMultipliers()
{
	string location;
	// In State multipliers are counties, states, provinces and 'dx'
	if (GetInStateMultsCounties())
	{
		GetCountyAbbrevs(m_inStateMultipliers);
	}

	// Does the contest allow dxcc countries as multipliers?
	// If not, most contests allow one 'DX' multiplier
	if (InStateDxccMults() == false)
	{
		m_inStateMultipliers.insert("dx");
	}

	// todo - make this data driven
	//      m_allMultipliers.insert("ks");

	const AllLocations& allLocations = GetAllLocations();
	const auto& stateAbbrevs = allLocations.GetStateAbbrevs();

	auto iter = stateAbbrevs.begin();
	auto iterEnd = stateAbbrevs.end();
	for (; iter != iterEnd; ++iter)
	{
		location = (*iter).first;
		StringUtils::ToLower(location);
		m_inStateMultipliers.insert(location);
	}

	const auto& canadaAbbrevs = allLocations.GetCanadaAbbrevs();
	iter = canadaAbbrevs.begin();
	iterEnd = canadaAbbrevs.end();
	for (; iter != iterEnd; ++iter)
	{
		location = (*iter).first;
		StringUtils::ToLower(location);
		m_inStateMultipliers.insert(location);
	}

	return true;
}

// Get the In State Multipliers for this Contest
bool Contest::GetInStateMultipliers(set<string>& inStateMultipliers) const
{
	if (m_inStateMultipliers.empty())
		return false;

	inStateMultipliers.clear();

	inStateMultipliers = m_inStateMultipliers;

	return true;
}

int Contest::GetInStateWorksOutOfStatePointsScaler() const 
	{ 
	return m_contestConfig->GetInStateWorksOutOfStatePointsScaler(); 
	}


bool Contest::GetBonusStationPointsPerBandPerMode() const 
{ 
	return m_contestConfig->GetBonusStationPointsPerBandPerMode(); 
}

// Check that the bonus counties supplied in the main config file are actually county abbreviations provided 
// in the contest counties file
bool Contest::CheckBonusCounties(const set<string>& countyAbbrevs, const set<string>& bonusCounties)
{
	bool status = true;
	int errorCount = 0;

	for (const string& bonusCounty : bonusCounties)
	{
		auto iter = countyAbbrevs.find(bonusCounty);
		if (iter == countyAbbrevs.end())
		{
			string temp(bonusCounty);
			StringUtils::ToUpper(temp);
			printf("Error: Bonus County %s is not a valid county abbreviation\n", temp.c_str());
			++errorCount;
			status = false;
		}
	}

	return status;
}
