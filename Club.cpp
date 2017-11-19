
#include "stdafx.h"

#include "Club.h"
#include "Station.h"
#include "StringUtils.h"
#include "TextFile.h"

// A ham radio club
// Ham radio clubs have stations that sum their scores for the club store
Club::Club()
:
m_name(),
m_score(-1),
m_stationMap()
{
}

// Clubs do not own stations
Club::~Club()
{
   m_stationMap.clear();
}

// Calculate the club score
void Club::CalcScore()
{
   m_score = 0;
   for (auto pair : m_stationMap)
   {
      Station *s = pair.second;
      m_score += s->Score();
   }
}

// Add this station to the club;
bool Club::AddStation(Station *s)
{
   bool status = true;
   string callsignLower = s->StationCallsign();
   StringUtils::ToLower(callsignLower);
   auto iter = m_stationMap.find(callsignLower);

   if (iter == m_stationMap.end())
   {
      m_stationMap[callsignLower] = s;
      status = true;
   }
   else
   {
      const char* pName = s->StationCallsign().c_str();
      printf("Error -> Cub::AddStation, station %s already exists in the club\n", pName);
      status = false;
   }

   return false;
}

// Write the scores for each station to the file
void Club::WriteScores(TextFile& file)
{
   int i = 0;
   char buffer[80];
   for (auto pair : m_stationMap)
   {
      Station *s = pair.second;
      const char* pCall = s->StationCallsign().c_str();
      sprintf_s(buffer, 80, "  Station %2d) %10d %s", ++i, s->Score(), pCall);
      file.AddLine(buffer);
   }
}
