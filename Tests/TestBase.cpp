
#include "stdafx.h"
#include "TestBase.h"
#include "TestError.h"

TestBase::TestBase(const string& testClassName)
	:
	m_setupComplete(false),
	m_teardown(false),
	m_errors(0),
	m_testClassName(testClassName),
	m_testCount(0),
   m_setupStatus(false)
{
   m_testDataFolder = "c:\\learn\\qsopartylogchecker\\testdata\\";
}

TestBase::~TestBase()
{
	auto iter = m_testErrors.begin();
	auto end = m_testErrors.end();
	for (; iter != end; ++iter)
	{
		TestError *t = *iter;
		delete t;
	}
	m_testErrors.clear();
}

bool TestBase::Setup()
{
	if (m_setupComplete)
		return m_setupStatus;

	m_setupComplete = true;
	m_errors = 0;
	m_asserts = 0;
	m_testCount = 0;
   m_setupStatus = true;
	printf("Setup Test Class: %s\n", m_testClassName.c_str());

   return m_setupStatus;
}

void TestBase::Teardown()
{
	if (m_teardown)
		return;

	m_teardown = true;
}

bool TestBase::StartTest(const string& currentTest) 
{ 
	bool status = Setup();  // Setup is virtual so derived classes can do test specific setup
   if (!status)
   {
      string msg = string("TestBase::StartTest Failed for test: ") + currentTest;
      printf("%s\n", msg.c_str());
      return false;
   }

	++m_testCount;
	m_currentTest = currentTest;
	printf("Running Test %s\n", m_currentTest.c_str());
   return true;
}

void TestBase::AssertFalse(const bool b1)
{
   ++m_asserts;
   if (b1)
   {
      ++m_errors;
      string msg = string("Assert False failed: ");
      TestError *error = new TestError(this, msg);
      m_testErrors.push_back(error);
   }
}

void TestBase::AssertTrue(const bool b1)
{
   ++m_asserts;
   if (!b1)
   {
      ++m_errors;
      string msg = string("Assert True failed: ");
      TestError *error = new TestError(this, msg);
      m_testErrors.push_back(error);
   }
}

void TestBase::AssertEqual(const string& s1, const string& s2)
{
	++m_asserts;
	if (s1 != s2)
	{
		++m_errors;
		string msg = string("AssertEqual failed: ") + s1 + string(" not equal to ") + s2;
		TestError *error = new TestError(this, msg);
		m_testErrors.push_back(error);
	}
}

void TestBase::AssertEqual(const int& i1, const int& i2)
{
   ++m_asserts;
   if (i1 != i2)
   {
      ++m_errors;
      char buffer[80];
      sprintf_s(buffer, 80, "AssertEqual failed, int %d not equal to int %d", i1, i2);
      string msg = string(buffer);
      TestError *error = new TestError(this, msg);
      m_testErrors.push_back(error);
   }
}

void TestBase::AssertEqual(const bool b1, const bool b2)
{
   ++m_asserts;
   if (b1 != b2)
   {
      string sb1 = b1 ? string("True") : string("False");
      string sb2 = b2 ? string("True") : string("False");
      ++m_errors;
      string msg = string("AssertEqual failed: bool ") + sb1 + string(" not equal to bool ") + sb2;
      TestError *error = new TestError(this, msg);
      m_testErrors.push_back(error);
   }
}

void TestBase::WriteSummary()
{
	if (m_testErrors.empty())
	{
		printf("No Errors in %-15s [%d tests run, %d asserts checked]\n", m_testClassName.c_str(), m_testCount, m_asserts);
		return;
	}

	int count = (int)m_testErrors.size();
	printf("Test Class %s : %d Errors Found\n", m_testClassName.c_str(), count);
	
	auto iter = m_testErrors.begin();
	auto end = m_testErrors.end();
	for (; iter != end; ++iter)
	{
		TestError *t = *iter;
		string msg = t->ToString();
		printf("%s\n", msg.c_str());
	}

}

// Add Error from derived class
void TestBase::AddTestError(TestError *error)
{
   m_testErrors.push_back(error);
}
