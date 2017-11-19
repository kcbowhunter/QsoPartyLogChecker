

#include "stdafx.h"
#include "TestBase.h"
#include "TestError.h"

TestError::TestError(TestBase *testBase, const string& msg)
   :
   m_testBase(testBase),
   m_errorMsg(msg)
{
	m_testName = testBase->GetCurrentTest();
}

string TestError::ToString()
{
	string msg = string("Error in test ") + m_testName + string(" : ") + m_errorMsg;
	return msg;
}



