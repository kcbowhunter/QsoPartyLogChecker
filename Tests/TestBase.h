
#pragma once

class TestError;

//
// Base class for all tests
// Each class derived from TestBase can provide multiple tests
//
class TestBase
{
public:
	virtual ~TestBase();

	virtual bool Setup();
	virtual void Teardown();
	virtual bool RunAllTests()=0;

	bool SetupComplete() const { return m_setupComplete; }
	bool TeardownComplete() const { return m_teardown; }
	int  ErrorCount() const { return m_errors; }

	string GetCurrentTest() const { return m_currentTest; }
//	string GetTestError() const { return m_testError; }
   string GetTestClassName() const { return m_testClassName; }

	void WriteSummary();

   bool SetupStatus() const { return m_setupStatus; }

protected:
	TestBase(const string& testClass);
	bool StartTest(const string& currentTest);

	void AssertEqual(const string& s1, const string& s2);
   void AssertEqual(const int& i1, const int& i2);
   void AssertEqual(const bool b1, const bool b2);

   void AssertFalse(const bool b1);
   void AssertTrue(const bool b1);

   const string TestDataFolder() const { return m_testDataFolder; }

   // Derived classes can set their own setup status
   void SetSetupStatus(bool b) { m_setupStatus = b; }

   // Add Error from derived class
   void AddTestError(TestError *error);

   // Return number of asserts executed
   int GetAssertCount();

private:
	bool m_setupComplete;  // setup was executed
   bool m_setupStatus;    // setup status (errors from Setup)
	bool m_teardown;       // teardown was executed
	int  m_errors;         // error count
	int  m_asserts;        // assert count
	int  m_testCount;      // number of tests executed
	string m_testClassName;    // name of the test class
	string m_currentTest;  // name of the current test
//	string m_testError;    // name of the test error
	list<TestError*> m_testErrors;

   string m_testDataFolder;
};

