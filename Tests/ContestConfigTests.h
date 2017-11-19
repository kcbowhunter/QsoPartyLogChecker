
#pragma once
#include "stdafx.h"
#include "TestBase.h"
class Station;

class ContestConfigTests : public TestBase
{
	public:
		ContestConfigTests();
		virtual ~ContestConfigTests();

		virtual bool Setup();
		virtual void Teardown();
		virtual bool RunAllTests();

      bool CheckRequiredFoldersTest_NoLogsFolderSpecified();
      bool CheckRequiredFoldersTest_LogFolderDoesNotExist();

   private:
};

