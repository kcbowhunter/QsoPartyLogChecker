

#include "stdafx.h"

#include "N6MU01.h"
#include "Contest.h"
#include "Station.h"
#include "Qso.h"

N6MU01::N6MU01()
:
MiniContest("N6MU01Logs", "MOQPConfigFiles\\moqp01.txt", "N6MU01")
{
}

N6MU01::~N6MU01()
{
}

bool N6MU01::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   return MiniContest::Setup();
}

void N6MU01::Teardown()
{
   TestBase::Teardown();
}

bool N6MU01::RunAllTests()
{
   bool status = true;

   status = Test01() && status;

   return status;
}

bool N6MU01::Test01()
{
   if (!StartTest("N6MU01-Test01")) return false;

   Station *w0h = m_contest->GetStation("w0h");
   Station *n6mu = m_contest->GetStation("n6mu");

   Qso *w0 = w0h->GetQso(0);
   Qso *w1 = w0h->GetQso(1);
   Qso *w2 = w0h->GetQso(2);

   Qso *n0 = n6mu->GetQso(0);
   Qso *n1 = n6mu->GetQso(1);
   Qso *n2 = n6mu->GetQso(2);

   AssertTrue(w0->ValidQso());
   AssertTrue(w1->ValidQso());
   // Duplicate
   AssertFalse(w2->ValidQso());

   AssertTrue(n0->ValidQso());
   AssertTrue(n1->ValidQso());
   // Duplicate
   AssertFalse(n2->ValidQso());

   AssertEqual(1, w0->GetNumber());
   AssertEqual(2, w1->GetNumber());
   AssertEqual(3, w2->GetNumber());

   AssertEqual(1, n0->GetNumber());
   AssertEqual(2, n1->GetNumber());
   AssertEqual(3, n2->GetNumber());

   AssertEqual(1, w0->GetRefQsoNumber());
   AssertEqual(2, w1->GetRefQsoNumber());
   AssertEqual(3, w2->GetRefQsoNumber());

   AssertEqual(1, n0->GetRefQsoNumber());
   AssertEqual(2, n1->GetRefQsoNumber());
   AssertEqual(3, n2->GetRefQsoNumber());

   AssertFalse(w0->IsDuplicate());
   AssertFalse(w1->IsDuplicate());
   AssertTrue(w2->IsDuplicate());
   AssertEqual(1, w2->GetDuplicateNumber());

   AssertFalse(n0->IsDuplicate());
   AssertFalse(n1->IsDuplicate());
   AssertTrue(n2->IsDuplicate());
   AssertEqual(1, n2->GetDuplicateNumber());

   return true;
}

