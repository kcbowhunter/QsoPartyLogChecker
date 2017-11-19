
#pragma once

class TestBase;

class TestRunner
{
public:
	TestRunner();
	~TestRunner();

	// Run all the tests in the registered test objects in parallel using OPENMP
	bool RunTestSuite();

private:
	list<TestBase*> m_tests;

};

