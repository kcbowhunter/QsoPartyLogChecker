
#include "stdafx.h"
#include "ContestConfigTests.h"
#include "StringUtils.h"
#include "ContestConfig.h"

ContestConfigTests::ContestConfigTests()
	:
	TestBase("ContestConfigTests")
{
}

ContestConfigTests::~ContestConfigTests()
{
}

bool ContestConfigTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   if (!TestBase::Setup())
      return false;

   return true;
}

void ContestConfigTests::Teardown()
{
	TestBase::Teardown();
}

bool ContestConfigTests::RunAllTests()
{
	bool status = true;

   status = CheckRequiredFoldersTest_NoLogsFolderSpecified() && status;
   status = CheckRequiredFoldersTest_LogFolderDoesNotExist() && status;

	return status;
}

bool ContestConfigTests::CheckRequiredFoldersTest_NoLogsFolderSpecified()
{
   if (!StartTest("CheckRequiredFoldersTest_NoLogsFolder")) return false;

   ContestConfig contestConfig;
   contestConfig.SetVerbose(false);  // don't dump errors to console
   bool status = contestConfig.CheckRequiredFolders();
   AssertFalse(status);
   ContestConfigError error = contestConfig.GetError();
   AssertTrue(error == eLogFolderNotProvided);

   return status == false;
}

bool ContestConfigTests::CheckRequiredFoldersTest_LogFolderDoesNotExist()
{
   if (!StartTest("CheckRequiredFoldersTest_LogFolderDoesNotExist")) return false;

   ContestConfig contestConfig;
   contestConfig.SetVerbose(false);  // don't dump errors to console
   contestConfig.AddFolderKeyValuePair("Logs", "c:\\DummyFolder\\LogsNotHere");

   bool status = contestConfig.CheckRequiredFolders();
   AssertFalse(status);
   ContestConfigError error = contestConfig.GetError();
   AssertTrue(error == eLogFolderDoesNotExist);

   return status == false;
}

