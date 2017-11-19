
#pragma once

#include "ContestConfig.h"

// Manages State / Province / Country / Section Information
class AllLocations
{
   public:
      AllLocations();
      AllLocations(const AllLocations& x);
      virtual ~AllLocations();

      bool Setup(ContestConfig *config);

      // Return true if this is a state abbreviation, ie "MO"
      bool IsStateAbbrev(const string& abbrev);

      // Return true if this is a province abbreviation, ie "NS"
      bool IsProvinceAbbrev(const string& abbrev);

      // given a section name, return the state or province, ie sfl -> fl
      string GetStateOrProvinceFromSection(const string& abbrev);

      // Return true if loc is a valid location
      bool IsValidLocation(const string& loc);

      const map<string, string>& GetStateAbbrevs() const { return m_stateAbbrevMap; }
      const map<string, string>& GetCanadaAbbrevs() const { return m_canadaAbbrevMap; }

   private:
      // state abbreviation map, ie MO Missouri, one key,value pair per line
      map<string, string> m_stateAbbrevMap;

      // Canada abbreviation map, ie "BC", "British Columbia", one key,value pair per line
      map<string, string> m_canadaAbbrevMap;

      // Map of arrl section abbrev to state name, ie. EB, CA
      // This is used to map from arrl sections to provinces and states because most qso parties use states and provinces as multipliers
      map<string, string> m_arrlSectionsMap;

};

