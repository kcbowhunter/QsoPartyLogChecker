
#include "stdafx.h"
#include "MiniContest.h"
#include "ContestConfig.h"
#include "Contest.h"
#include "TextFile.h"

MiniContest::MiniContest(const string& logsFileFolder, const string& configFileName, const string& testName)
   :
   TestBase(testName),
   m_contestConfig(nullptr),
   m_contest(nullptr),
   m_logsFileFolder(logsFileFolder),
   m_testRoot("c:\\learn\\qsopartylogchecker\\testroot"),
   m_configFileName(configFileName)
   {
      m_contestConfig = new ContestConfig();
      string logsFolder = m_testRoot + string("\\") + logsFileFolder;
      m_contestConfig->SetFoldersSectionValue("logs", logsFolder);
   }

MiniContest::~MiniContest()
   {
      delete m_contestConfig;
      m_contestConfig = nullptr;
   }

bool MiniContest::Setup()
   {
      if (SetupComplete())
         return SetupStatus();

      bool status = TestBase::Setup();

      if (status)
      {
         status = ProcessConfigFile();
         if (status)
         {
            m_contest = new Contest();
            // NOTE: this does not process the log files
            m_contest->ProcessConfigData(m_contestConfig);

            vector<string> logFileNames;
            m_contestConfig->GetLogFileNames(logFileNames);
            printf("Processing %zd logs\n", logFileNames.size());
            printf("Begin Processing Logs\n");
            m_contest->ProcessLogs(logFileNames);
         }
      }

      return status;
   }

void MiniContest::Teardown()
   {
      TestBase::Teardown();
   }

bool MiniContest::ProcessConfigFile()
{
   TextFile configFile;
   string configFileName = m_testRoot + string("\\") + m_configFileName;
   bool status = configFile.Read(configFileName);

   if (!status)
   {
      printf("Error: Unable to process config file %s from MiniContest::ProcessConfigFile \n", m_configFileName.c_str());
      string errorString = configFile.ErrorString();
      if (!errorString.empty())
      {
         printf("   File Error: %s\n", errorString.c_str());
      }
      return false;
   }

   vector<string> text;
   configFile.GetLines(text);

   status = m_contestConfig->Process(text);
   if (!status)
   {
      printf("Error processing config file %s\n", configFileName.c_str());
      return false;
   }

   return true;
}