
#pragma once

#include "QsoItem.h"
class Qso;
class DxccCountry;

class Location : public QsoItem
{
   public:
      Location(Qso* qso);
      virtual ~Location();

      // Return true if location is valid
      // Return false if location is invalid and set error string
      bool Validate(const set<string>& validLocations);

	  void         SetDxccCountry(DxccCountry *dxcc) { m_dxccCountry = dxcc; }
	  DxccCountry *GetDxccCountry() const { return m_dxccCountry; }

   private:
      Location();
	  DxccCountry *m_dxccCountry;
};
