
#pragma once
#include "stdafx.h"
#include "TestBase.h"
class Station;

class CallsignTests : public TestBase
{
	public:
		CallsignTests();
		virtual ~CallsignTests();

		virtual bool Setup();
		virtual void Teardown();
		virtual bool RunAllTests();

      bool TestN0H_BAR();
      bool TestVE9AA();
      bool TestOM2VL();

   private:
};

