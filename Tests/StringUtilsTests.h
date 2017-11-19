
#pragma once
#include "stdafx.h"
#include "TestBase.h"

class StringUtilsTests : public TestBase
{
	public:
		StringUtilsTests();
		virtual ~StringUtilsTests();

		virtual bool Setup();
		virtual void Teardown();
		virtual bool RunAllTests();

		bool TestToLower();
		bool TestRemoveComment01();
		bool TestRemoveComment02();
		bool TestRemoveComment03();

      bool TestSplit01();
      bool TestSplit02();
      bool TestSplit03();
      bool TestSplit04();
      bool TestSplit05();
      bool TestSplit06();

      bool TestGetFirstDigit01();
      bool TestGetFirstDigit02();
      bool TestGetFirstDigit03();
      bool TestGetFirstDigit04();
      bool TestGetFirstDigit05();

      bool TestPadRight();
};

