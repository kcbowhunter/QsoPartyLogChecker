

#include "stdafx.h"
#include "StationTests.h"
#include "StringUtils.h"
#include "Station.h"

StationTests::StationTests()
	:
	TestBase("StationTests")

{
}

StationTests::~StationTests()
{
}

bool StationTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   if (!TestBase::Setup())
      return false;

   return true;
}

void StationTests::Teardown()
{
	TestBase::Teardown();
}

bool StationTests::RunAllTests()
{
	bool status = true;

	status = TestStationCat() && status;
   status = TestPowerCat() && status;
   status = TestOperatorCat() && status;
   status = TestStationModeCat() && status;

	return status;
}

bool StationTests::TestStationCat()
{
   if (!StartTest("TestStationCat")) return false;

   StationCat cat = Station::ParseStationCategory("unknown");
   AssertTrue(cat == eUnknownStationCat);

   cat = Station::ParseStationCategory("fred");
   AssertTrue(cat == eUnknownStationCat);

   cat = Station::ParseStationCategory("any");
   AssertTrue(cat == eAnyStationCat);

   cat = Station::ParseStationCategory("fixed");
   AssertTrue(cat == eFixedStationCat);

   cat = Station::ParseStationCategory("mobile");
   AssertTrue(cat == eMobileStationCat);

   cat = Station::ParseStationCategory("portable");
   AssertTrue(cat == ePortableStationCat);

   return true;
}

//enum PowerCat        { eUnknownPowerCat, eAnyPowerCat, eHighPowerCat, eLowPowerCat, eQrpPowerCat };
bool StationTests::TestPowerCat()
{
   if (!StartTest("TestPowerCat")) return false;

   PowerCat cat = Station::ParsePowerCategory("unknown");
   AssertTrue(cat == eUnknownStationCat);

   cat = Station::ParsePowerCategory("fred");
   AssertTrue(cat == eUnknownPowerCat);

   cat = Station::ParsePowerCategory("any");
   AssertTrue(cat == eAnyPowerCat);

   cat = Station::ParsePowerCategory("high");
   AssertTrue(cat == eHighPowerCat);

   cat = Station::ParsePowerCategory("low");
   AssertTrue(cat == eLowPowerCat);

   cat = Station::ParsePowerCategory("qrp");
   AssertTrue(cat == eQrpPowerCat);

   return true;
}

// enum OperatorCat     { eUnknownOperatorCat, eAnyOperatorCat, eSingleOperatorCat, eMultiOperatorCat };
bool StationTests::TestOperatorCat()
{
   if (!StartTest("TestOperatorCat")) return false;

   OperatorCat cat = Station::ParseOperatorCategory("unknown");
   AssertTrue(cat == eUnknownPowerCat);

   cat = Station::ParseOperatorCategory("fred");
   AssertTrue(cat == eUnknownPowerCat);

   cat = Station::ParseOperatorCategory("any");
   AssertTrue(cat == eAnyPowerCat);

   cat = Station::ParseOperatorCategory("single");
   AssertTrue(cat == eSingleOperatorCat);

   cat = Station::ParseOperatorCategory("multi");
   AssertTrue(cat == eMultiOperatorCat);

   return true;
}

// enum StationModeCat  { eUnknownModeCat, eAnyModeCat, eCwModeCat, eSsbModeCat, eMixedModeCat };
bool StationTests::TestStationModeCat()
{
   if (!StartTest("TestStationModeCat")) return false;

   StationModeCat cat = Station::ParseStationModeCategory("unknown");
   AssertTrue(cat == eUnknownStationModeCat);

   cat = Station::ParseStationModeCategory("fred");
   AssertTrue(cat == eUnknownStationModeCat);

   cat = Station::ParseStationModeCategory("any");
   AssertTrue(cat == eAnyModeCat);

   cat = Station::ParseStationModeCategory("cw");
   AssertTrue(cat == eCwModeCat);

   cat = Station::ParseStationModeCategory("ssb");
   AssertTrue(cat == eSsbModeCat);

   cat = Station::ParseStationModeCategory("mixed");
   AssertTrue(cat == eMixedModeCat);

   return true;
}