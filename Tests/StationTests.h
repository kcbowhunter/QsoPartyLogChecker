
#pragma once
#include "stdafx.h"
#include "TestBase.h"

class StationTests : public TestBase
{
	public:
		StationTests();
		virtual ~StationTests();

		virtual bool Setup();
		virtual void Teardown();
		virtual bool RunAllTests();

      // Test individual category enums used to define a Category
      bool TestStationCat();
      bool TestPowerCat();
      bool TestOperatorCat();
      bool TestStationModeCat();

   private:
};

