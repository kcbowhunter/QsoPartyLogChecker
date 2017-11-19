

#include "stdafx.h"

#include "QsoTests.h"

#include "Qso.h"
#include "Station.h"
#include "QsoTokenType.h"
#include "FreqToken.h"
#include "Freq.h"
#include "ModeToken.h"
#include "Mode.h"
#include "DateToken.h"
#include "Date.h"
#include "TimeToken.h"
#include "QsoTime.h"

#include "CallsignToken.h"
#include "RstToken.h"
#include "SerialNumberToken.h"
#include "LocationToken.h"

#include "Callsign.h"
#include "Rst.h"
#include "SerialNumber.h"
#include "Location.h"

QsoTests::QsoTests()
:
TestBase("QsoTests")
{
}

QsoTests::~QsoTests()
{
}


bool QsoTests::Setup()
{
   if (SetupComplete())
      return SetupStatus();

   return TestBase::Setup();
}

void QsoTests::Teardown()
{
   TestBase::Teardown();
}

bool QsoTests::RunAllTests()
{
   bool status = true;

   status = TestParseFrequency() && status;
   status = TestParseFreqModeDateTime() && status;
   status = TestParseQso01() && status;
   status = TestCopyAndFlip() && status;
   status = TestGetCreatedText() && status;

   return status;
}

bool QsoTests::TestParseFrequency()
{
   if (!StartTest("TestParseFrequency")) return false;

   Station *station = nullptr;

   vector<QsoTokenType*> tokenTypes(1);
   FreqToken *freqToken = new FreqToken();
   tokenTypes[0] = freqToken;

//   Qso *qso = new Qso(station);
   Qso *qso = new Qso();
   qso->SetStation(station);

   string line("3541");

   qso->Process(line, tokenTypes);

   const Freq& freq = qso->GetFreq();
   const string freqStr = freq.GetValue();

   AssertEqual("3541", freqStr);

   return true;
}

bool QsoTests::TestParseFreqModeDateTime()
{
   if (!StartTest("TestParseFreqModeDateTime")) return false;

   Station *station = nullptr;

   vector<QsoTokenType*> tokenTypes(4);
   FreqToken *freqToken = new FreqToken();
   ModeToken *modeToken = new ModeToken();
   DateToken *dateToken = new DateToken();
   TimeToken *timeToken = new TimeToken();

   tokenTypes[0] = freqToken;
   tokenTypes[1] = modeToken;
   tokenTypes[2] = dateToken;
   tokenTypes[3] = timeToken;

//   Qso *qso = new Qso(station);
   Qso *qso = new Qso();
   qso->SetStation(station);

   string line("3541  CW  2014-04-06  0109");

   qso->Process(line, tokenTypes);

   const Freq& freq = qso->GetFreq();
   const string freqStr = freq.GetValue();
   AssertEqual("3541", freqStr);

   const Mode& mode = qso->GetMode();
   const string modeStr = mode.GetValue();
   AssertEqual("cw", modeStr);

   const Date& date = qso->GetDate();
   const string dateStr = date.GetValue();
   AssertEqual("2014-04-06", dateStr);

   const QsoTime& qsoTime = qso->GetTime();
   const string timeStr = qsoTime.GetValue();
   AssertEqual("0109", timeStr);

   return true;
}

bool QsoTests::TestParseQso01()
{
   if (!StartTest("TestParseQso01")) return false;

   Station *station = nullptr;

   vector<QsoTokenType*> tokenTypes(12);
   FreqToken *freqToken = new FreqToken();
   ModeToken *modeToken = new ModeToken();
   DateToken *dateToken = new DateToken();
   TimeToken *timeToken = new TimeToken();

   CallsignToken *myCallsignToken = new CallsignToken();
   RstToken *myRstToken = new RstToken();
   SerialNumberToken *mySerialNumberToken = new SerialNumberToken();
   LocationToken *myLocationToken = new LocationToken();

   CallsignToken *theirCallsignToken = new CallsignToken();
   RstToken      *theirRstToken = new RstToken();
   SerialNumberToken *theirSerialNumberToken = new SerialNumberToken();
   LocationToken *theirLocationToken = new LocationToken();

   tokenTypes[0] = freqToken;
   tokenTypes[1] = modeToken;
   tokenTypes[2] = dateToken;
   tokenTypes[3] = timeToken;

   tokenTypes[4] = myCallsignToken;
   tokenTypes[5] = myRstToken;
   tokenTypes[6] = mySerialNumberToken;
   tokenTypes[7] = myLocationToken;

   tokenTypes[8] = theirCallsignToken;
   tokenTypes[9] = theirRstToken;
   tokenTypes[10] = theirSerialNumberToken;
   tokenTypes[11] = theirLocationToken;

//   Qso *qso = new Qso(station);
   Qso *qso = new Qso();
   qso->SetStation(station);

   string line("3541  CW  2014-04-06  0109 AI4WW   599   0001  FL    N0H 579  0616 GAS");

   qso->Process(line, tokenTypes);

   const Freq& freq = qso->GetFreq();
   const string freqStr = freq.GetValue();
   AssertEqual("3541", freqStr);

   const Mode& mode = qso->GetMode();
   const string modeStr = mode.GetValue();
   AssertEqual("cw", modeStr);

   const Date& date = qso->GetDate();
   const string dateStr = date.GetValue();
   AssertEqual("2014-04-06", dateStr);

   const QsoTime& qsoTime = qso->GetTime();
   const string timeStr = qsoTime.GetValue();
   AssertEqual("0109", timeStr);

   const Callsign& myCallsign = qso->GetMyCallsign();
   const string myCallsignString = myCallsign.GetValue();
   AssertEqual("ai4ww", myCallsignString);

   const Rst& myRst = qso->GetMyRst();
   AssertEqual("599", myRst.GetValue());

   const SerialNumber& mySerial = qso->GetMySerialNumber();
   AssertEqual("0001", mySerial.GetValue());

   const Location *myLocation = qso->GetMyLocation();
   AssertEqual("fl", myLocation->GetValue());

   const Callsign& theirCallsign = qso->GetTheirCallsign();
   const string theirCallsignString = theirCallsign.GetValue();
   AssertEqual("n0h", theirCallsignString);

   const Rst& theirRst = qso->GetTheirRst();
   AssertEqual("579", theirRst.GetValue());

   const SerialNumber& theirSerial = qso->GetTheirSerialNumber();
   AssertEqual("0616", theirSerial.GetValue());

   const Location *theirLocation = qso->GetTheirLocation();
   AssertEqual("gas", theirLocation->GetValue());

   return true;
}

// test when copying to create the missing logs / qso's
// i.e. this
//    14007  CW  2014-04-05  1402 KC0M   579   0002  TAN   WA6KHK 589  0001 CA
// becomes
//    14007  CW  2014-04-05  1402 WA6KHK 589  0001 CA      KC0M   579   0002  TAN   
bool QsoTests::TestCopyAndFlip()
{
   if (!StartTest("TestCopyAndFlip")) return false;

   Qso *qso = new Qso();
   string line("14007  CW  2014-04-05  1402 KC0M   579   0002  TAN   WA6KHK 589  0001 CA");

   vector<QsoTokenType*> tokenTypes;
   CreateTokenTypesMOQP2014(tokenTypes);

   qso->Process(line, tokenTypes);

   Qso *newQso = new Qso();

//   string line("14007  CW  2014-04-05  1402 KC0M   579   0002  TAN   WA6KHK 589  0001 CA");

   newQso->CopyAndFlip(qso);

   string freq = newQso->GetFreq().GetValue();
   AssertEqual("14007", freq);

   HamBand band = e20m;
   AssertTrue(band == newQso->GetFreq().GetBand());

   string mode = newQso->GetMode().GetValue();
   AssertEqual("cw", mode);

   QsoMode qmode = eModeCw;
   AssertTrue(qmode == newQso->GetMode().GetMode());

   string date = "2014-04-05";
   AssertEqual(date, newQso->GetDate().GetValue());

   string time = "1402";
   AssertEqual(time, newQso->GetTime().GetValue());

   // Flip the source and target qso data
   //   string line("14007  CW  2014-04-05  1402 KC0M   579   0002  TAN   WA6KHK 589  0001 CA");
   string myCall = "wa6khk";
   AssertEqual(myCall, newQso->GetMyCallsign().GetCallsign());

   string theirCall = "kc0m";
   AssertEqual(theirCall, newQso->GetTheirCallsign().GetCallsign());

   string myRst = "589";
   AssertEqual(myRst, newQso->GetMyRst().GetValue());

   string theirRst = "579";
   AssertEqual(theirRst, newQso->GetTheirRst().GetValue());

   string mySerial = "0001";
   AssertEqual(mySerial, newQso->GetMySerialNumber().GetValue());

   string theirSerial = "0002";
   AssertEqual(theirSerial, newQso->GetTheirSerialNumber().GetValue());

   string myLocation = "ca";
   AssertEqual(myLocation, newQso->GetMyLocation()->GetValue());

   string theirLocation = "tan";
   AssertEqual(theirLocation, newQso->GetTheirLocation()->GetValue());

   string qsoText = newQso->GetCreatedText(tokenTypes);

   delete qso;
   delete newQso;

   return true;
}

// Create the ascii text for the Cabrillo file for this qso test
bool QsoTests::TestGetCreatedText()
{
   if (!StartTest("TestGetCreatedText")) return false;

   Qso *qso = new Qso();
   string line("14007  CW  2014-04-05  1402 KC0M   579   0002  TAN   WA6KHK 589  0001 CA");

   vector<QsoTokenType*> tokenTypes;
   CreateTokenTypesMOQP2014(tokenTypes);

   qso->Process(line, tokenTypes);

   string cabFile = qso->GetCreatedText(tokenTypes);

   delete qso;
   return false;
}

bool QsoTests::CreateTokenTypesMOQP2014(vector<QsoTokenType*>& tokenTypes)
{
   tokenTypes.resize(12);
   FreqToken *freqToken = new FreqToken();
   ModeToken *modeToken = new ModeToken();
   DateToken *dateToken = new DateToken();
   TimeToken *timeToken = new TimeToken();

   CallsignToken     *myCallsignToken = new CallsignToken();
   RstToken          *myRstToken = new RstToken();
   SerialNumberToken *mySerialNumberToken = new SerialNumberToken();
   LocationToken     *myLocationToken = new LocationToken();

   CallsignToken     *theirCallsignToken = new CallsignToken();
   RstToken          *theirRstToken = new RstToken();
   SerialNumberToken *theirSerialNumberToken = new SerialNumberToken();
   LocationToken     *theirLocationToken = new LocationToken();

   tokenTypes[0] = freqToken;
   tokenTypes[1] = modeToken;
   tokenTypes[2] = dateToken;
   tokenTypes[3] = timeToken;

   tokenTypes[4] = myCallsignToken;
   tokenTypes[5] = myRstToken;
   tokenTypes[6] = mySerialNumberToken;
   tokenTypes[7] = myLocationToken;

   tokenTypes[8] = theirCallsignToken;
   tokenTypes[9] = theirRstToken;
   tokenTypes[10] = theirSerialNumberToken;
   tokenTypes[11] = theirLocationToken;

   return true;
}