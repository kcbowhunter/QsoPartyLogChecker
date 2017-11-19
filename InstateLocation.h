
#pragma once

#include "Location.h"
class Qso;

class InstateLocation : public Location
{
   public:
      InstateLocation(Qso *qso);
      virtual ~InstateLocation();
};

