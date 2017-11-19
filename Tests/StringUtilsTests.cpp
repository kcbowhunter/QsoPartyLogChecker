

#include "stdafx.h"
#include "StringUtilsTests.h"
#include "StringUtils.h"

StringUtilsTests::StringUtilsTests()
	:
	TestBase("StringUtilsTests")
{
}

StringUtilsTests::~StringUtilsTests()
{
}

bool StringUtilsTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

	return TestBase::Setup();
}

void StringUtilsTests::Teardown()
{
	TestBase::Teardown();
}

bool StringUtilsTests::RunAllTests()
{
	bool status = true;

	status = status && TestToLower();
	status = status && TestRemoveComment01();
	status = status && TestRemoveComment02();
	status = status && TestRemoveComment03();

   status = status && TestSplit01();
   status = status && TestSplit02();
   status = status && TestSplit03();
   status = status && TestSplit04();
   status = status && TestSplit05();
   status = status && TestSplit06();

   status = status && TestGetFirstDigit01();
   status = status && TestGetFirstDigit02();
   status = status && TestGetFirstDigit03();
   status = status && TestGetFirstDigit04();
   status = status && TestGetFirstDigit05();

   status = status && TestPadRight();

	return status;
}

bool StringUtilsTests::TestToLower()
{
   if (!StartTest("TestToLower")) return false;

	string fred("Fred");
	StringUtils::ToLower(fred);

	AssertEqual(fred, "fred");

	return true;
}

bool StringUtilsTests::TestRemoveComment01()
{
   if (!StartTest("TestRemoveComments01")) return false;

	string text("hi mom");
	string answer("hi mom");

	string result = StringUtils::RemoveComment(text, '#');
	AssertEqual(result, "hi mom");

	return true;
}

bool StringUtilsTests::TestRemoveComment02()
{
   if (!StartTest("TestRemoveComments02")) return false;

	string text("#");
	string answer("");

	string result = StringUtils::RemoveComment(text, '#');
	AssertEqual(result, "");

	return true;
}


bool StringUtilsTests::TestRemoveComment03()
{
   if (!StartTest("TestRemoveComments03")) return false;

	string text("hi mom # comment");
	string answer("hi mom # ");

	string result = StringUtils::RemoveComment(text, '#');
	AssertEqual(result, "hi mom ");

	return true;
}

bool StringUtilsTests::TestSplit01()
{
   if (!StartTest("TestSplit01")) return false;

   list<string> tokens;
   string line("a");

   StringUtils::Split(tokens, line);

   AssertEqual((int)tokens.size(), 1);

   string a("a");
   auto iter = tokens.begin();
   string first = *iter;
   AssertEqual(first, a);

   return true;
}

bool StringUtilsTests::TestSplit02()
{
   if (!StartTest("TestSplit02")) return false;

   list<string> tokens;
   string line(" a");

   StringUtils::Split(tokens, line);

   AssertEqual((int)tokens.size(), 1);

   string a("a");
   auto iter = tokens.begin();
   string first = *iter;
   AssertEqual(first, a);

   return true;
}

bool StringUtilsTests::TestSplit03()
{
   if (!StartTest("TestSplit03")) return false;

   list<string> tokens;
   string line("a ");

   StringUtils::Split(tokens, line);

   AssertEqual((int)tokens.size(), 1);

   string a("a");
   auto iter = tokens.begin();
   string first = *iter;
   AssertEqual(first, a);


   return true;
}

bool StringUtilsTests::TestSplit04()
{
   if (!StartTest("TestSplit04")) return false;

   list<string> tokens;
   string line(" a ");

   StringUtils::Split(tokens, line);

   AssertEqual((int)tokens.size(), 1);

   string a("a");
   auto iter = tokens.begin();
   string first = *iter;
   AssertEqual(first, a);


   return true;
}

bool StringUtilsTests::TestSplit05()
{
   if (!StartTest("TestSplit05")) return false;

   list<string> tokens;
   string line("a b");

   StringUtils::Split(tokens, line);

   AssertEqual((int)tokens.size(), 2);

   string a("a");
   auto iter = tokens.begin();
   string first = *iter;
   AssertEqual(first, a);

   ++iter;
   string second = *iter;
   AssertEqual(second, "b");

   return true;
}

bool StringUtilsTests::TestSplit06()
{
   if (!StartTest("TestSplit06")) return false;

   list<string> tokens;
   string line("QSO:  3541 CW 2014-04-06 0109 AI4WW         599 0001 FL  N0H           599 0616 GAS ");

   StringUtils::Split(tokens, line);

   AssertEqual((int)tokens.size(), 13);

   string qso("QSO:");
   auto iter = tokens.begin();
   string first = *iter;
   AssertEqual(first, qso);

   ++iter;
   string second = *iter;
   AssertEqual(second, "3541");

   ++iter;
   second = *iter;
   AssertEqual(second, "CW");

   ++iter;
   second = *iter;
   AssertEqual(second, "2014-04-06");

   ++iter;
   second = *iter;
   AssertEqual(second, "0109");

   ++iter;
   second = *iter;
   AssertEqual(second, "AI4WW");

   ++iter;
   second = *iter;
   AssertEqual(second, "599");

   ++iter;
   second = *iter;
   AssertEqual(second, "0001");

   ++iter;
   second = *iter;
   AssertEqual(second, "FL");

   ++iter;
   second = *iter;
   AssertEqual(second, "N0H");

   ++iter;
   second = *iter;
   AssertEqual(second, "599");

   ++iter;
   second = *iter;
   AssertEqual(second, "0616");

   ++iter;
   second = *iter;
   AssertEqual(second, "GAS");

   return true;
}

bool StringUtilsTests::TestGetFirstDigit01()
{
   if (!StartTest("TestGetFirstDigit01")) return false;

   string s1;
   int value = StringUtils::GetFirstDigit(s1);
   AssertEqual(-1, value);

   return true;
}

bool StringUtilsTests::TestGetFirstDigit02()
{
   if (!StartTest("TestGetFirstDigit02")) return false;
   string s1("ad0dx");

   int value = StringUtils::GetFirstDigit(s1);
   AssertEqual(0, value);

   return true;
}

bool StringUtilsTests::TestGetFirstDigit03()
{
   if (!StartTest("TestGetFirstDigit03")) return false;
   string s1("ad0dx/1");

   int value = StringUtils::GetFirstDigit(s1);
   AssertEqual(0, value);

   return true;
}

bool StringUtilsTests::TestGetFirstDigit04()
{
   if (!StartTest("TestGetFirstDigit04")) return false;
   string s1("w1aw");

   int value = StringUtils::GetFirstDigit(s1);
   AssertEqual(1, value);

   return true;
}

bool StringUtilsTests::TestGetFirstDigit05()
{
   if (!StartTest("TestGetFirstDigit05")) return false;
   string s1("hi mom!");

   int value = StringUtils::GetFirstDigit(s1);
   AssertEqual(-1, value);

   return true;
}

bool StringUtilsTests::TestPadRight()
{
   if (!StartTest("TestPadRight")) return false;
   string s1("7240");

   StringUtils::PadRight(s1, ' ', 5);
   AssertEqual("7240 ", s1);

   return true;
}

