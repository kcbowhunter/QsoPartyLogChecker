

#include "stdafx.h"
#include "CallsignTests.h"
#include "StringUtils.h"
#include "Callsign.h"
#include "Qso.h"

CallsignTests::CallsignTests()
	:
	TestBase("CallsignTests")

{
}

CallsignTests::~CallsignTests()
{
}

bool CallsignTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   if (!TestBase::Setup())
      return false;

   return true;
}

void CallsignTests::Teardown()
{
	TestBase::Teardown();
}

bool CallsignTests::RunAllTests()
{
	bool status = true;

	status = TestN0H_BAR() && status;
   status = TestVE9AA() && status;
   status = TestOM2VL() && status;

	return status;
}

bool CallsignTests::TestN0H_BAR()
{
   if (!StartTest("TestN0H_BAR")) return false;

   Qso *qso = nullptr;
   Callsign callsign(qso);

   string buffer("N0O/BAR");

   bool status = callsign.ProcessToken(buffer, qso);

   string prefix = callsign.GetPrefix();
   string call = callsign.GetCallsign();
   string suffix = callsign.GetSuffix();

   AssertTrue(prefix.empty());
   AssertEqual("N0O", call);
   AssertEqual("BAR", suffix);

   AssertTrue(callsign.IsUSA());
   AssertFalse(callsign.IsCanada());
   AssertFalse(callsign.IsDx());

   return true;
}

bool CallsignTests::TestVE9AA()
{
   if (!StartTest("TestVE9AA")) return false;

   Qso *qso = nullptr;
   Callsign callsign(qso);

   string buffer("VE9AA");

   bool status = callsign.ProcessToken(buffer, qso);

   string prefix = callsign.GetPrefix();
   string call = callsign.GetCallsign();
   string suffix = callsign.GetSuffix();

   AssertTrue(prefix.empty());
   AssertEqual("VE9AA", call);
   AssertTrue(suffix.empty());

   AssertFalse(callsign.IsUSA());
   AssertTrue(callsign.IsCanada());
   AssertFalse(callsign.IsDx());

   return true;
}

bool CallsignTests::TestOM2VL()
{
   if (!StartTest("TestOM2VL")) return false;

   Qso *qso = nullptr;
   Callsign callsign(qso);

   string buffer("OM2VL");

   bool status = callsign.ProcessToken(buffer, qso);

   string prefix = callsign.GetPrefix();
   string call = callsign.GetCallsign();
   string suffix = callsign.GetSuffix();

   AssertTrue(prefix.empty());
   AssertEqual("OM2VL", call);
   AssertTrue(suffix.empty());

   AssertFalse(callsign.IsUSA());
   AssertFalse(callsign.IsCanada());
   AssertTrue(callsign.IsDx());

   return true;
}
