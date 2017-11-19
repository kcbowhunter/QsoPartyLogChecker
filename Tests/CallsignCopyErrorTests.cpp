

#include "stdafx.h"

#include "CallsignCopyErrorTests.h"
#include "Contest.h"
#include "Station.h"
#include "Qso.h"

CallsignCopyErrorTests::CallsignCopyErrorTests()
:
MiniContest("CallsignCopyErrorLogs", "KSQP\\ksqp.txt", "CallsignCopyError")
{
}

CallsignCopyErrorTests::~CallsignCopyErrorTests()
{
}

bool CallsignCopyErrorTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   return MiniContest::Setup();
}

void CallsignCopyErrorTests::Teardown()
{
   TestBase::Teardown();
}

bool CallsignCopyErrorTests::RunAllTests()
{
   bool status = true;

   status = Test01() && status;

   return status;
}

bool CallsignCopyErrorTests::Test01()
{
   if (!StartTest("CallsignCopyError-Test01")) return false;

   Station *k0s = m_contest->GetStation("k0s");
   Station *n6mu = m_contest->GetStation("n6mu");

   Qso *k0 = k0s->GetQso(0);
   Qso *n0 = n6mu->GetQso(0);

   AssertTrue(k0->OtherStationMissing());
   AssertFalse(n0->OtherStationMissing());

   AssertFalse(k0->ValidQso());
   AssertTrue(n0->ValidQso());

   return true;
}

