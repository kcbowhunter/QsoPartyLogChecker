
#pragma once

#include "MiniContest.h"

class CallsignCopyErrorTests : public MiniContest
{
   public:
      CallsignCopyErrorTests();
      virtual ~CallsignCopyErrorTests();

      virtual bool Setup();
      virtual void Teardown();
      virtual bool RunAllTests();

      bool Test01();

};