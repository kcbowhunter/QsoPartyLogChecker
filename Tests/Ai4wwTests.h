
#pragma once
#include "stdafx.h"
#include "TestBase.h"
class Station;

class Ai4wwTests : public TestBase
{
	public:
		Ai4wwTests();
		virtual ~Ai4wwTests();

		virtual bool Setup();
		virtual void Teardown();
		virtual bool RunAllTests();

      bool TestLocation();
      bool TestInState();
      bool TestCallsign();
      bool TestNumberOfQsos();

   private:
      Station *m_station;

      // lines from the cabrillo file
      vector<string> m_lines;
};

