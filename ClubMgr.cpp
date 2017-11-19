
#include "stdafx.h"

#include "ClubMgr.h"
#include "Club.h"
#include "Station.h"
#include "StringUtils.h"
#include "TextFile.h"

ClubMgr::ClubMgr()
{

}

ClubMgr::~ClubMgr()
{
   for (auto& pair : m_clubMap)
   {
      Club* club = pair.second;
      delete club;
   }

   m_clubMap.clear();
}

// Add a station to a club
// If the club exists, add the station to the club
// If the club does not exist, create the club and add the station
void ClubMgr::AddStation(Station *s)
{
   // if there is no club then we have nothing to do
   const string clubName = s->GetClub();
   if (clubName.empty())
      return;

   string clubNameLower = s->GetClubLower();

   Club *club = FindClub(clubNameLower);
   if (club == nullptr)
   {
      club = new Club();
      club->SetName(clubName);
      m_clubMap[clubNameLower] = club;
   }

   club->AddStation(s);
}

Club *ClubMgr::FindClub(const string& clubName)
{
   Club *club = nullptr;
   string clubNameLower = clubName;
   StringUtils::ToLower(clubNameLower);

   auto iter = m_clubMap.find(clubNameLower);
   if (iter != m_clubMap.end())
   {
      club = (*iter).second;
   }

   return club;
}

// Calculate the club scores for each club
void ClubMgr::CalculateScores()
{
   for (auto pair : m_clubMap)
   {
      Club *club = pair.second;
      club->CalcScore();
   }
}

bool SortClubs(const Club* c1, const Club* c2)
{
   return c1->Score() > c2->Score();
}

// Sort the clubs and write the report
void ClubMgr::WriteReport(const string& fileName, const string& title)
{
   // Sort the clubs by scores
   vector<Club*> clubs(m_clubMap.size());
   int i = 0;
   for (auto pair : m_clubMap)
   {
      clubs[i++] = pair.second;
   }

   sort(clubs.begin(), clubs.end(), SortClubs);

   TextFile file;
   file.AddLine(title);

   file.AddLine(" ");

   char buffer[80];
   i = 0;
   for (Club* c : clubs)
   {
      string clubName = c->GetName();
      sprintf_s(buffer, 80, "Club (%2d) %10d :", ++i, c->Score());
      string line = string(buffer) + clubName;
      file.AddLine(line);
      c->WriteScores(file);
      file.AddLine(" ");
   }

   file.Write(fileName);
}



