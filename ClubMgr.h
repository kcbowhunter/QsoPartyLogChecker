
#pragma once

class Club;
class Station;

// The ClubMgr manages clubs and sorts the clubs by scores
class ClubMgr
{
public:
   ClubMgr();
   virtual ~ClubMgr();

   // Add a station to a club
   // If the club exists, add the station to the club
   // If the club does not exist, create the club and add the station
   void AddStation(Station *s);

   // Find a club given the club name
   Club *FindClub(const string& clubName);

   // Calculate the club scores for each club
   void CalculateScores();

   // Sort the clubs and write the report
   void WriteReport(const string& fileName, const string& title);

private:
   // map of lower case club name, Club*
   map<string, Club*> m_clubMap;
};