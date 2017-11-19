

#include "stdafx.h"
#include "Ai4wwTests.h"
#include "StringUtils.h"
#include "Station.h"
#include "TextFile.h"
#include "TestError.h"
#include "Contest.h"

Ai4wwTests::Ai4wwTests()
	:
	TestBase("Ai4wwTests"),
   m_station(nullptr)
{
}

Ai4wwTests::~Ai4wwTests()
{
   if (m_station)
      delete m_station;
}

bool Ai4wwTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   if (!TestBase::Setup())
      return false;

//   printf("Ai4wwTests::Setup\n");
   string filename = TestDataFolder() + string("ai4ww.log");
   TextFile textfile;
   bool status = textfile.Read(filename);
   if (!status)
   {
      SetSetupStatus(false);
      string msg = string("Ai4wwTests::Setup Error -> Failed to open file: ") + filename;
      TestError *error = new TestError(this, msg);
      AddTestError(error);
      return false;
   }

   textfile.GetLines(m_lines);

   Contest *contest = nullptr;
   m_station = new Station(contest);
   m_station->SetContestState("Missouri");
   m_station->SetContestStateAbbrev("mo");

   vector<QsoTokenType*> tokenTypes;
   m_station->ParseStringsCreateQsos(m_lines, tokenTypes);

   return true;
}

void Ai4wwTests::Teardown()
{
	TestBase::Teardown();
}

bool Ai4wwTests::RunAllTests()
{
	bool status = true;

	status = TestLocation() && status;
   status = TestInState()  && status;
   status = TestCallsign() && status;
   status = TestNumberOfQsos() && status;

	return status;
}

bool Ai4wwTests::TestLocation()
{
   if (!StartTest("TestLocation")) return false;

   string location = "sfl";
   AssertEqual(location, m_station->StationLocation());

	return true;
}

bool Ai4wwTests::TestInState()
{
   if (!StartTest("TestInState")) return false;

   AssertEqual(false, m_station->InState());

   return true;
}

bool Ai4wwTests::TestCallsign()
{
   if (!StartTest("TestCallsign")) return false;

   string call = "ai4ww";
   AssertEqual(call, m_station->StationCallsignLower());

   return true;
}

bool Ai4wwTests::TestNumberOfQsos()
{
   if (!StartTest("TestNumberOfQsos")) return false;

   int count = m_station->NumberOfQsos();
   AssertEqual(1, count);

   return true;
}
