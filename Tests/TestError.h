

#pragma once

class TestBase;

// Holds the data for one error encountered during a test
// The tests are run in parallel, so the error messages cannot be written to the output 
// while the tests are running because the text from different tests would be intermingled.
// This small class captures the information for a test error so that the error can be written 
// out at a later time
class TestError
{
public:
	TestError(TestBase *testBase, const string& msg);
	virtual ~TestError() {};

	string ToString();

private:
	TestError(){};
	TestBase* m_testBase;
	string m_testName;
	string m_errorMsg;
};
