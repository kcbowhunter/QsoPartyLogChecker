

#include "stdafx.h"
#include "CategoryTests.h"
#include "StringUtils.h"
#include "Category.h"
#include "Station.h"
#include "Contest.h"
#include "CategoryMgr.h"

CategoryTests::CategoryTests()
	:
	TestBase("CategoryTests")

{
}

CategoryTests::~CategoryTests()
{
}

bool CategoryTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   if (!TestBase::Setup())
      return false;

   return true;
}

void CategoryTests::Teardown()
{
	TestBase::Teardown();
}

bool CategoryTests::RunAllTests()
{
	bool status = true;

	status = TestCanadaCategory() && status;
   status = TestDXCategory() && status;
   status = TestInstate() && status;
   status = TestMobileInStateSingleOperatorLowPower() && status;

	return status;
}

bool CategoryTests::TestCanadaCategory()
{
   if (!StartTest("TestCanadaCategory")) return false;

   // Setup the category
   CategoryMgr *catmgr = nullptr;
   Category cat(catmgr);
   string data1("country=Canada");
   string data2("title=O Canada");
   list<string> categoryData;
   categoryData.push_back(data1);
   categoryData.push_back(data2);

   bool status = cat.ReadData(categoryData);

   AssertTrue(status);

   AssertEqual(cat.m_country, "canada");
   AssertEqual(cat.m_title, "O Canada");
   AssertTrue(cat.m_instate == eFalseBool);

   // Setup the station
   Contest *contest = nullptr;
   Station ve3iir(contest);
   ve3iir.SetCountry("canada");

   // The station should match the category
   status = cat.Match(&ve3iir);
   AssertTrue(status);

   return true;
}

bool CategoryTests::TestDXCategory()
{
   if (!StartTest("TestDXCategory")) return false;

   // Setup the category
   CategoryMgr *catmgr = nullptr;
   Category cat(catmgr);
   string data1("country=DX");
   string data2("title=DX Category");
   list<string> categoryData;
   categoryData.push_back(data1);
   categoryData.push_back(data2);

   bool status = cat.ReadData(categoryData);

   AssertTrue(status);

   AssertEqual(cat.m_country, "dx");
   AssertEqual(cat.m_title, "DX Category");
//   AssertFalse(cat.m_instate);

   // Setup the station
   Contest *contest = nullptr;
   Station om2vl(contest);
   om2vl.SetCountry("dx");

   // The station should match the category
   status = cat.Match(&om2vl);
   AssertTrue(status);

   return true;
}

bool CategoryTests::TestInstate()
{
   if (!StartTest("TestInstate")) return false;

   CategoryMgr *catmgr = nullptr;
   Category cat(catmgr);

   string key("Instate");
   string value("true");
   bool status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertTrue(cat.m_instate);

   value = "Yes";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertTrue(cat.m_instate);

   value = "TRUE";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertTrue(cat.m_instate);

   value = "YES";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertTrue(cat.m_instate);

   value = "false";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertFalse(cat.m_instate);

   value = "FALSE";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertFalse(cat.m_instate);

   value = "no";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertFalse(cat.m_instate);

   value = "NO";
   status = cat.AssignData(key, value);
   AssertTrue(status);
//   AssertFalse(cat.m_instate);

   value = "";
   status = cat.AssignData(key, value);
   AssertFalse(status);

   value.clear();
   status = cat.AssignData(key, value);
   AssertFalse(status);

   value = "badvalue";
   status = cat.AssignData(key, value);
   AssertFalse(status);

   return true;
}

bool CategoryTests::TestMobileInStateSingleOperatorLowPower()
{
   if (!StartTest("TestMobileInStateSingleOperatorLowPower")) return false;

   // Setup The Category
   CategoryMgr *catmgr = nullptr;
   Category cat(catmgr);
   string data1("station=mobile");
   string data2("title=Mobile InState SingleOp LowPower");
   string data3("operator=single");
   string data4("instate=true");
   string data5("power=low");

   list<string> categoryData;
   categoryData.push_back(data1);
   categoryData.push_back(data2);
   categoryData.push_back(data3);
   categoryData.push_back(data4);
   categoryData.push_back(data5);

   bool status = cat.ReadData(categoryData);

   AssertTrue(status);

   // USA is the default country
   AssertEqual(cat.m_country, "usa");
   AssertEqual(cat.m_title, "Mobile InState SingleOp LowPower");
//   AssertTrue(cat.m_instate);
   AssertTrue(cat.m_powerCat == eLowPowerCat);
   AssertTrue(cat.m_stationOperatorCat == eSingleOperatorCat);

   // Setup the station
   Contest *contest = nullptr;
   Station ad0dx(contest);
   ad0dx.SetInState(true);
   ad0dx.SetStationCat(eMobileStationCat);
   ad0dx.SetPowerCat(eLowPowerCat);
   ad0dx.SetOperatorCat(eSingleOperatorCat);

   // The station should match the category
   status = cat.Match(&ad0dx);
   AssertTrue(status);

   return true;
}
