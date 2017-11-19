
#pragma once

#include "TestBase.h"
class QsoTokenType;

class QsoTests : public TestBase
{
   public:
      QsoTests();
      virtual ~QsoTests();

      virtual bool Setup();
      virtual void Teardown();
      virtual bool RunAllTests();

      bool TestParseFrequency();
      bool TestParseFreqModeDateTime();
      bool TestParseQso01();

      // test when copying to create the missing logs / qso's
      bool TestCopyAndFlip();

      // Create the ascii text for the Cabrillo file for this qso test
      bool TestGetCreatedText();

   private:
      bool CreateTokenTypesMOQP2014(vector<QsoTokenType*>& tokenTypes);

};