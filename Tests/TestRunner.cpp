
#include "stdafx.h"
#include "TestRunner.h"
#include "TestBase.h"

#include "StringUtilsTests.h"
#include "Ai4wwTests.h"
#include "QsoTests.h"
#include "N6MU01.h"
#include "CallsignTests.h"
#include "StationTests.h"
#include "CategoryTests.h"
#include "ContestConfigTests.h"

TestRunner::TestRunner()
{
	m_tests.push_back(new StringUtilsTests());
   m_tests.push_back(new Ai4wwTests());
   m_tests.push_back(new QsoTests());
   m_tests.push_back(new N6MU01());
   m_tests.push_back(new CallsignTests());
   m_tests.push_back(new StationTests());
   m_tests.push_back(new CategoryTests());
   m_tests.push_back(new ContestConfigTests());
}

TestRunner::~TestRunner()
{
	auto iter = m_tests.begin();
	auto enditer = m_tests.end();

	TestBase *t = nullptr;
	for (; iter != enditer; ++iter)
	{
		t = *iter;
		delete t;
	}

	m_tests.clear();
}

// Run all the tests in the registered test objects in parallel using OPENMP
bool TestRunner::RunTestSuite()
{
	int count = 0;
	int size = (int)m_tests.size();
	vector<TestBase*> tests(size);
	for (TestBase* t : m_tests)
	{
		tests[count++] = t;
	}

   TestBase **ppTests = &tests[0];
	count = 0;

   TestBase *testBase = nullptr;
   string testClassName;
   string currentTest;
   int i = 0;
//#pragma omp parallel for default(none) shared(size, ppTests) private(i, testBase, testClassName)
	for (i = 0; i < size; ++i)
	{
      try{
         testBase = ppTests[i];
         testClassName = testBase->GetTestClassName();
         testBase->RunAllTests();
      }
      catch (std::exception& e)
      {
         string currentTest = testBase->GetCurrentTest();
         const char* testName = currentTest.empty() ? "Missing" : currentTest.c_str();
         printf("std::Exception: Test class %s Current Test %s\n", testClassName.c_str(), testName);
         printf("   std::Exception --> %s", e.what());
      }
      catch (...)
      {
         string currentTest = testBase->GetCurrentTest();
         const char* testName = currentTest.empty() ? "Missing" : currentTest.c_str();
         printf("std::Exception: Test class %s Current Test %s\n", testClassName.c_str(), testName);
      }
	}

	for (TestBase* t : m_tests)
	{
		t->WriteSummary();
	}

	return true;
}


