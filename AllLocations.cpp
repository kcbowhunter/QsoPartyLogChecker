
#include "stdafx.h"
#include "AllLocations.h"
#include "StringUtils.h"

AllLocations::AllLocations()
{

}

AllLocations::AllLocations(const AllLocations& x)
{
   m_stateAbbrevMap = x.m_stateAbbrevMap;
   m_canadaAbbrevMap = x.m_canadaAbbrevMap;
   m_arrlSectionsMap = x.m_arrlSectionsMap;
}

AllLocations::~AllLocations()
{

}

bool AllLocations::Setup(ContestConfig *config)
{
   const map<string, string>& stateAbbrevMap = config->GetStateAbbrevMap();
   const map<string, string>& provinceAbbrevMap = config->GetCanadaAbbrevMap();
   const map<string, string>& arrlSectionMap = config->GetArrlSectionsMap();

   m_stateAbbrevMap = stateAbbrevMap;
   m_canadaAbbrevMap = provinceAbbrevMap;
   m_arrlSectionsMap = arrlSectionMap;

   return true;
}

// Return true if this is a state abbreviation, ie "MO"
bool AllLocations::IsStateAbbrev(const string& abbrev)
{
   string ab(abbrev);
   StringUtils::ToLower(ab);

   auto iter = m_stateAbbrevMap.find(ab);

   return  iter == m_stateAbbrevMap.end() ? false : true;
}

bool AllLocations::IsProvinceAbbrev(const string& abbrev)
{
   string ab(abbrev);
   StringUtils::ToLower(ab);

   auto iter = m_canadaAbbrevMap.find(ab);

   return  iter == m_canadaAbbrevMap.end() ? false : true;
}

// given a section name, return the state or province, ie sfl -> fl
string AllLocations::GetStateOrProvinceFromSection(const string& abbrev)
{
   string ab(abbrev);
   StringUtils::ToLower(ab);

   auto iter = m_arrlSectionsMap.find(ab);

   if (iter == m_arrlSectionsMap.end())
      return string();

   string x = (*iter).second;

   return x;
}

bool AllLocations::IsValidLocation(const string& loc)
{
   if (IsStateAbbrev(loc))
      return true;

   if (IsProvinceAbbrev(loc))
      return true;

   return false;
}