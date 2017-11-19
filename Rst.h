
#pragma once

#include "QsoItem.h"
class Qso;

class Rst : public QsoItem
{
   public:
      Rst(Qso* qso);
      virtual ~Rst();

   private:
      Rst();
};

