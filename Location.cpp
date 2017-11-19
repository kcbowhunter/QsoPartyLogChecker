
#include "stdafx.h"
#include "Location.h"
#include "Qso.h"

Location::Location(Qso *qso)
   :
   QsoItem(qso),
   m_dxccCountry(nullptr)
   {
   }

Location::~Location()
{
	m_dxccCountry = nullptr;
}

// Return true if location is valid
// Return false if location is invalid and set error string
bool Location::Validate(const set<string>& validLocations)
{
   if (m_value.empty())
   {
      m_errorString = "Location is missing";
      return false;
   }

   auto iter = validLocations.find(m_value);
   if (iter == validLocations.end())
   {
      m_errorString = m_value + string(" is not a valid location");
      return false;
   }

   return true;
}
