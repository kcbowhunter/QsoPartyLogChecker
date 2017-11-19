// QsoPartyLogChecker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TextFile.h"
#include "StringUtilsTests.h"
#include "TestRunner.h"
#include "ContestConfig.h"
#include "Contest.h"
#include "Station.h"
#include "Ai4wwTests.h"
#include "QsoTests.h"
#include "N6MU01.h"
#include "CallsignTests.h"
#include "StationTests.h"
#include "CategoryTests.h"
#include "CallsignCopyErrorTests.h"
#include "ContestConfigTests.h"
#include "Win32Utils.h"
#include "CheckDxccCountry.h"
#include "StringUtils.h"
#include "boost/algorithm/string.hpp"
using namespace boost;


void RunOneTest();
void RunAllTests();
void ShowNumberOfThreads();

int _tmain(int argc, _TCHAR* argv[])
{
   const string Version("1.2.0 Build 2017-Nov-28-1");
   printf("Hello QsoPartyLogChecker!\n");
   printf("   Version %s\n", Version.c_str());
   printf("   Comments, questions, bug reports to Ron ad0dx@yahoo.com\n");

   string currentFolder = Win32Utils::GetCurrentWorkingDirectory();
   printf("   Current working directory is %s\n", currentFolder.c_str());

   // Setup the static data early to avoid having to use a critical section around the static data structures
   Station::SetupStaticData();

	//////
//   RunOneTest();
//   return -1;
	//////

//	RunAllTests();
//   return -2;

	if (argc == 1)
	{
		printf("Usage: QsoPartyLogChecker ConfigFile.txt [file.log] OR [DxccCheck=callsign] OR [MissingLogs=True/False]\n");
		return 1;
	}

	TextFile configFile;
	string configFileName = argv[1];
	bool status = configFile.Read(configFileName);

	if (!status)
	{
		printf("Error: Unable to read config file %s\n", argv[1]);
		string errorString = configFile.ErrorString();
		if (!errorString.empty())
		{
			printf("   File Error: %s\n", errorString.c_str());
		}
		return 2;
	}

	vector<string> text;
	configFile.GetLines(text);

	ContestConfig config;
	status = config.Process(text);
	if (!status)
	{
		printf("Error processing config file %s\n", configFileName.c_str());
		return 3;
	}

   ShowNumberOfThreads();

	const string title = config.GetTitle();
	if (!title.empty())
      printf("\nWelcome to the %s!\n\n", title.c_str());

	TextFile *pLogFile = nullptr;
	if (argc == 3)
	{
//		string filename(argv[2]);
//		pLogFile = new TextFile();
//		pLogFile->Read(filename);
//		string errorMsg = pLogFile->ErrorString();
//		if (!errorMsg.empty())
//		{
//			printf("Error opening log file %s: %s\n", filename.c_str(), errorMsg.c_str());
//			return 4;
//		}
		// process dxcccheck=callsign to display the country for the given callsign
		string arg3(argv[2]);
		string dxcccheck = arg3.substr(0, 9);
		string key;
		string value;
		string errorMsg;

		StringUtils::GetKeyValuePair(arg3, key, value, errorMsg);
		if (! errorMsg.empty())
		{
			printf("unknown argument: %s", arg3.c_str());
			return -6;
		}
		if (boost::iequals(dxcccheck, "dxcccheck"))
		{
			auto *dxccCountryManager = config.GetDxccCountryManager();
			CheckDxccCountry(arg3, dxccCountryManager);
			return 0;
		}
		else if (boost::iequals(key, "MissingLogs"))
		{
			bool ml = false;
			StringUtils::ParseBoolean(ml, value);
			if (ml)
				printf("Changing GenerateMissingLogs to True\n");
			else
				printf("Changing GenerateMissingLogs to False\n");

			config.SetGenerateMissingLogs(ml);
		}
		else
		{
			printf("unknown argument: %s", arg3.c_str());
			return -5;
		}
	}

   double startTime = omp_get_wtime();
	Contest *pContest = new Contest();

	// NOTE: this does not process the log files
	status = pContest->ProcessConfigData(&config);
   if (!status)
   {
      printf("Error Processing Contest Configuration File %s\n", configFileName.c_str());
      printf("   Please fix configuration errors and run again\n");
      printf("   Note that the errors cound be in files referenced from the Contest Config File\n");
      return 5;
   }

	if (pLogFile != nullptr)
	{
		vector<string> data;
		pLogFile->GetLines(data);
		Station station(pContest);
        vector<QsoTokenType*> tokenTypes;
        station.ParseStringsCreateQsos(data, tokenTypes);
		station.WriteOneLineSummary();
	}
	else
	{
		vector<string> logFileNames;
		config.GetLogFileNames(logFileNames);
        printf("Processing %zd logs\n", logFileNames.size());
        printf("Begin Processing Logs\n");
		pContest->ProcessLogs(logFileNames);
        printf("End Processing Logs\n");
	}

   printf("\n");
//   printf("Begin Delete Contest\n");
	delete pContest;
//   printf("End   Delete Contest\n");

//   printf("Qso dtor time %7.3f ms\n", Qso::m_dtorTime*1000.0);

   const string& resultsFolder = config.GetResultsFolder();
   if (!resultsFolder.empty())
   {
      printf("\nContest Results are in folder %s\n", resultsFolder.c_str());
   }

   const string& logReportsFolder = config.GetLogReportsFolder();
   if (!logReportsFolder.empty())
   {
      printf("Log Reports     are in folder %s\n", logReportsFolder.c_str());
   }

   double endTime = omp_get_wtime();
   double time = (endTime - startTime) * 1000.0;
   printf("\nTotal time for QsoPartyLogChecker: %7.2f ms\n", time);


   return 0;
}

////////////////////////////////////////////////////////////////
void RunOneTest()
{
//	StringUtilsTests test;
//   test.TestPadRight();
//	test.TestToLower();
//	test.TestRemoveComment01();
//   test.TestSplit06();
//	test.RunAllTests();

//   Ai4wwTests test;
//   test.TestLocation();

   QsoTests test;
//   test.TestParseFrequency();
//   test.TestParseFreqModeDateTime();
//   test.TestParseQso01();
   test.TestCopyAndFlip();
//   test.TestGetCreatedText();

//   N6MU01 test;
//   test.Test01();

//   CallsignTests test;
//   test.Test01();

//   StationTests test;
//   test.TestStationModeCat();

//   CategoryTests test;
//   test.TestMobileInStateSingleOperatorLowPower();

//   CallsignCopyErrorTests test;
//   test.Test01();

//   ContestConfigTests test;
//   test.CheckRequiredFoldersTest_LogFolderDoesNotExist();

	test.WriteSummary();
}

///////////////////////////////////////////////
void RunAllTests()
{
	TestRunner testRunner;
	testRunner.RunTestSuite();
}

/////////////////////////////////////////////////
void ShowNumberOfThreads()
{
   int maxthreads = 0;
#pragma omp parallel
   {
      if (omp_get_thread_num() == 0)
      {
         maxthreads = omp_get_num_threads();
         printf("   This program will use %d threads\n\n", maxthreads);
      }
   }
}