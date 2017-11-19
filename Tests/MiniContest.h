
#pragma once

#include "TestBase.h"

class ContestConfig;
class Contest;

class MiniContest : public TestBase
{
   public:
      MiniContest(const string& logsFileFolder, const string& configFileName, const string& testName);
      virtual ~MiniContest();

      virtual bool Setup();
      virtual void Teardown();

   protected:
      ContestConfig *m_contestConfig;
      Contest *m_contest;

      string m_testRoot;
      string m_logsFileFolder;
      string m_configFileName;

   private:
      bool ProcessConfigFile();
};
