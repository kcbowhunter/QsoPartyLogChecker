
#pragma once

#include "MiniContest.h"

class N6MU01 : public MiniContest
{
   public:
      N6MU01();
      virtual ~N6MU01();

      virtual bool Setup();
      virtual void Teardown();
      virtual bool RunAllTests();

      bool Test01();

};