
#pragma once
#include "stdafx.h"
#include "TestBase.h"
struct Category;

class CategoryTests : public TestBase
{
	public:
		CategoryTests();
		virtual ~CategoryTests();

		virtual bool Setup();
		virtual void Teardown();
		virtual bool RunAllTests();

      bool TestCanadaCategory();
      bool TestDXCategory();

      bool TestInstate();

      bool TestMobileInStateSingleOperatorLowPower();

   private:
};

