
#include "stdafx.h"
#include "ReportWriter.h"
#include "Station.h"
#include "TextFile.h"
#include "StringUtils.h"
#include "AllLocations.h"
#include "CategoryMgr.h"
#include "Category.h"
#include "Contest.h"


// Sorts Callsigns based on district first (0-9), then length of call, then alphabetical
bool CallsignLessThan(const string& call1, const string& call2)
{
   int digit1 = StringUtils::GetFirstDigit(call1);
   int digit2 = StringUtils::GetFirstDigit(call2);

   if (digit1 < digit2)
      return true;
   else if (digit1 > digit2)
      return false;

   size_t len1 = call1.length();
   size_t len2 = call2.length();
   if (len1 < len2)
      return true;
   else if (len1 > len2)
      return false;

   return call1 < call2;
}

// Sorts Callsigns based on district first (0-9), then length of call, then alphabetical
bool StationCallsignLessThan(const Station* s1, const Station* s2)
{
   const string& s1call = s1->StationCallsign();
   const string& s2call = s2->StationCallsign();

   return CallsignLessThan(s1call, s2call);
}

// Write Instate / Out of State summary file
void ReportWriter::InOutStateStations(bool instate, const string& filename, vector<Station*>& stations)
{
   TextFile file;
   if (instate)
   {
      file.AddLine("List of In State Stations");
   }
   else
   {
      file.AddLine("List of Out of State Stations");
   }

   vector<string> calls;
   calls.reserve(stations.size());
   string callsign;
   for (Station* station : stations)
   {
      if (station->InState() == instate)
      {
         callsign = station->StationCallsign();
         calls.push_back(callsign);
      }
   }

   sort(calls.begin(), calls.end(), CallsignLessThan);

   char buffer[80];
   int i = 1;
   for (string call : calls)
   {
      sprintf_s(buffer, 80, " %3d) %s", i++, call.c_str());

      string str(buffer);
      file.AddLine(str);
   }

   file.Write(filename);
}

bool SortCounties(pair<string, int>& p1, pair<string, int>& p2)
{
   return p1.second > p2.second;
}

// Write out the County Count 
void ReportWriter::WriteCountyCount(map<string, int>& countyCount, map<string, string>& countyMap, string fileName)
{
   TextFile file;

   string msg = "County Counts";
   file.AddLine(msg);

   vector<pair<string, int>> counties;
   counties.resize(countyCount.size());

   pair<string, int> countyPair;
   int count = 0;
   char buffer[80];
   auto iter = countyCount.begin();
   auto iterEnd = countyCount.end();
   for (; iter != iterEnd; ++iter)
   {
      string county = (*iter).first;
      int countyCount = (*iter).second;

      countyPair.first = county;
      countyPair.second = countyCount;
      counties[count++] = countyPair;
   }

   sort(counties.begin(), counties.end(), SortCounties);

   int qsoCount = 0;
   auto iter2 = counties.begin();
   auto iter2end = counties.end();
   auto mapIter = countyMap.begin();
   auto mapIterEnd = countyMap.end();
   string countyName;
   string countyAbbrevUpper;
   for (; iter2 != iter2end; ++iter2)
   {
      countyPair = *iter2;
      string abbrev = countyPair.first;
      mapIter = countyMap.find(abbrev);
      countyName = mapIter == mapIterEnd ? "Missing" : (*mapIter).second;

      countyAbbrevUpper = abbrev.c_str();
      StringUtils::ToUpper(countyAbbrevUpper);
      sprintf_s(buffer, 80, "%4s, %4d, %s", countyAbbrevUpper.c_str(), countyPair.second, countyName.c_str());
      file.AddLine(buffer);
      qsoCount += countyPair.second;
   }

   sprintf_s(buffer, 80, "Total Qso's %d", qsoCount);
   file.AddLine(buffer);

   file.Write(fileName);
}

bool SortScoreStations(const Station* s1, const Station* s2)
{
   return s1->Score() > s2->Score();
}

// Write the stations to the given file sorted by score
void ReportWriter::WriteStationsByScore(const string& filename, list<Station*>& stations, const string& title)
{
   // First copy the stations to a vector
   vector<Station*> stationVec(stations.size());

   int i = 0;
   for (Station *s : stations)
   {
      stationVec[i++] = s;
   }

   // This method sorts the vector of stations and writes the file
   ReportWriter::WriteStationsByScore(filename, stationVec, title);
}

// Write the stations to the given file, sorted by score
void ReportWriter::WriteStationsByScore(const string& filename, vector<Station*>& stations, const string& title)
{
   vector<Station*> stationVec(stations.size());

   sort(stations.begin(), stations.end(), SortScoreStations);

   TextFile file;

   file.AddLine(title);
   file.AddLine(" ");

   int i = 1;
   char buffer[80];
   for (Station *s : stations)
   {
      sprintf_s(buffer, 80, "%4d) %12s : %-10d", i++, s->StationCallsign().c_str(), s->Score());
      file.AddLine(buffer);
      if ((i-1) % 5 == 0 && i > 1)
      {
         file.AddLine(" ");
      }
   }

   file.AddLine(" ");
   
   sprintf_s(buffer, 80, "Total Number of Stations: %zd", stations.size());
   file.AddLine(buffer);

   file.Write(filename);
}

void ReportWriter::WriteStationsByScoreAndCountry(const string& country, const string& filename, vector<Station*>& stations, const string& title)
{
   list<Station*> stationList;

   for (Station* s : stations)
   {
      if (s->Country() == country)
      {
         stationList.push_back(s);
      }
   }

   ReportWriter::WriteStationsByScore(filename, stationList, title);
}

void ReportWriter::WriteStationsByScoreAndInState(bool instate, const string& filename, vector<Station*>& stations, const string& title)
{
   list<Station*> stationList;
   for (Station* s : stations)
   {
      if (s->InState() == instate && s->Country() == "usa")
      {
         stationList.push_back(s);
      }
   }

   ReportWriter::WriteStationsByScore(filename, stationList, title);
}

// Summary Report for each US State
void ReportWriter::WriteStateSummaryReport(const string& filename, vector<Station*>& stations, AllLocations* allLocations)
{
   // key = state abbrev, value = list of stations in that state
   map<string, list<Station*>> reportMap;

   // abbrev, state name map, i.e. mo, missouri
   map<string, string> states = allLocations->GetStateAbbrevs();

   int count = 0;
   // Build up the report map
   for (Station* s : stations)
   {
      if (s->OutState() && s->Country() == "usa")
      {
         string state = s->State();
         if (state.empty())
         {
            printf("Error: ReportWriter::WriteStateSummaryReport - station %s missing state\n", s->StationCallsign().c_str());
            continue;
         }

         StringUtils::ToLower(state);
         auto stateIter = states.find(state);
         if (stateIter == states.end())
         {
            printf("Error: ReportWriter::WriteStateSummaryReport - state %s unknown\n", state.c_str());
            continue;
         }

         auto mapIter = reportMap.find(state);
         if (mapIter == reportMap.end())
         {
            list<Station*> stations;
            stations.push_back(s);
            pair<string, list<Station*>> p(state, stations);
            reportMap.insert(p);
         }
         else
         {
            list<Station*>& stations = (*mapIter).second;
            stations.push_back(s);
         }
      }
   }

   const int SIZE = 80;
   char buffer[SIZE];
   TextFile file;
   file.AddLine("Scoring Summary By State");

   auto mapIter = reportMap.begin();
   auto mapIterEnd = reportMap.end();

   for (; mapIter != mapIterEnd; ++mapIter)
   {
      string abbrev = (*mapIter).first;
      StringUtils::ToLower(abbrev);

      list<Station*>& stations = (*mapIter).second;

      auto stateIter = states.find(abbrev);
      if (stateIter == states.end())
      {
         printf("Error: state not found\n");
         continue;
      }
      string name = (*stateIter).second;

      file.AddLine(name);
      if (stations.empty())
      {
         file.AddLine("*** no stations found ***\n");
         continue;
      }

      int i = 0;
      vector<Station*> stationVec(stations.size());
      for (Station* s : stations)
      {
         stationVec[i++] = s;
      }

      sort(stationVec.begin(), stationVec.end(), SortScoreStations);
      for (Station* s : stationVec)
      {
         ++count;
         sprintf_s(buffer, 80, "%15s : %-10d", s->StationCallsign().c_str(), s->Score());
         file.AddLine(buffer);
      }
      file.AddLine(" ");
   }

   file.AddLine(" ");
   sprintf_s(buffer, SIZE, "Total USA Stations for this report: %d", count);
   file.AddLine(buffer);


   file.Write(filename);
}

void ReportWriter::WriteMobileStationsByScore(const string& reportRoot, vector<Station*>& stations)
{
   list<Station*> mobiles;

   for (Station *s : stations)
   {
      if (s->InState() && s->Mobile())
      {
         mobiles.push_back(s);
         printf("   Instate Mobile: %s\n", s->StationCallsign().c_str());
         if (s->SingleOp())
            printf("      Single-Op\n");
         else
            printf("      Multi-Op\n");

         printf("      Score: %d\n", s->Score());
      }
   }
}

// Write the master summary for all categories
// iFlag = 1-> Generic Summary
// iFlag = 2 -> KSQP Style Summary
void ReportWriter::WriteCategorySummary(Contest *contest, const string& filename, CategoryMgr *categoryMgr, int iFlag)
{
   list<Category*> categories;
   categoryMgr->GetCategories(categories);

   TextFile file;
   string contestAbbrev = contest->GetAbbrev();
   string msg = string("Category Summary for ") + contestAbbrev;
   file.AddLine(msg);

   if (iFlag == 2)
   {
      char buffer[120];
      sprintf_s(buffer, 120, "1x1, call, award, qth, cw, ph, dig, points, mults, bonus, score");
      file.AddLine(buffer);
   }

   // do the instate fixed categories
   for (Category* cat : categories)
   {
      if (cat->m_instate &&
         cat->m_stationCat == eFixedStationCat)
      {
         WriteCategory(file, cat, iFlag);
      }
   }

   // do the instate mobile categories
   for (Category* cat : categories)
   {
      if (cat->m_instate &&
         cat->m_stationCat == eMobileStationCat)
      {
         WriteCategory(file, cat, iFlag);
      }
   }

   // do the instate rover categories
   for (Category* cat : categories)
   {
      if (cat->m_instate &&
         cat->m_stationCat == eRoverStationCat)
      {
         WriteCategory(file, cat, iFlag);
      }
   }

   // do the instate expedition categories
   for (Category* cat : categories)
   {
      if (cat->m_instate &&
         cat->m_stationCat == eExpeditionStationCat)
      {
         WriteCategory(file, cat, iFlag);
      }
   }

   // do the out of state USA categories
   for (Category* cat : categories)
   {
      if (!cat->m_instate && (cat->m_country == "usa" || cat->m_country == "usacan"))
      {
         WriteCategory(file, cat, iFlag);
      }
   }

   // canada
   for (Category* cat : categories)
   {
      if (cat->m_country == "canada")
      {
         WriteCategory(file, cat, iFlag);
         break;
      }
   }

   // dx
   for (Category* cat : categories)
   {
      if (cat->m_country == "dx")
      {
         WriteCategory(file, cat, iFlag);
         break;
      }
   }

   // checklog
   for (Category* cat : categories)
   {
      if (cat->m_stationOperatorCat == eCheckLogCat)
      {
         WriteCategory(file, cat, iFlag);
         break;
      }
   }

   file.Write(filename);
}

// Write the category data out in the order the categories appear in the categories file
void ReportWriter::WriteCategorySummaryInOrder(Contest *contest, const string& filename, CategoryMgr *categoryMgr)
{
   list<Category*> categories;
   categoryMgr->GetCategories(categories);

   TextFile file;
   string contestAbbrev = contest->GetAbbrev();
   string msg = string("Category Summary for ") + contestAbbrev;
   file.AddLine(msg);

   for (Category* cat : categories)
   {
         WriteCategory(file, cat, 1);
   }

   file.Write(filename);
}

// Write one category to the given text file (but do not write the file to disk)
void ReportWriter::WriteCategory(TextFile& file, Category* cat, int iFlag)
{
   file.AddLine(" ");
   file.AddLine(cat->m_title);

   if (cat->m_stations.empty())
      return;

   auto size = cat->m_stations.size();
   vector<Station*> stations(size);
   int i = 0;
   for (Station *s : cat->m_stations)
   {
      stations[i++] = s;
   }

   sort(stations.begin(), stations.end(), SortScoreStations);

   i = 0;
   char buffer[120];
   if (iFlag == 1)
   {
      for (Station *s : stations)
      {
         sprintf_s(buffer, 120, "   %4d) %10s Score = %7d", ++i, s->StationCallsign().c_str(), s->Score());
         file.AddLine(buffer);
      }
   }
   else if (iFlag == 2)
   {
      for (Station *s : stations)
      {
         // KSQP Style Summary
         string s1x1;
         string call = s->StationCallsign();

         if (call.length() == 3)
         {
            s1x1 = call;
            StringUtils::ToUpper(s1x1);
            call = s->Operators();
            if (call.length() > 20)
            {
               call = call.substr(0, 20);
            }
            StringUtils::ToUpper(call);
         }

         string award;
         string location;
         if (s->Country() == "usa")
         {
            location = s->State().empty() ? s->StationLocation() : s->State();
         }
         else if (s->Country() == "canada")
         {
            location = s->Province().empty() ? s->StationLocation() : s->Province();
         }
         else
         {
            location = "dx";
         }

         StringUtils::ToUpper(location);

         int cwqsos = s->GetValidCwQsos();
         int phqsos = s->GetValidPhoneQsos();
         int dgqsos = s->GetValidDigitalQsos();

         int qsopoints = s->QsoPoints();
         int mults = s->TotalMultipliers();
         int bonus = s->BonusPoints();
         int score = s->Score();

         if (score <= 0)
            continue;

         const char *p1x1 = s1x1.empty() ? "" : s1x1.c_str();
         const char *pcall = call.empty() ? "?" : call.c_str();
         const char *pLoc = location.empty() ? "?" : location.c_str();

//         sprintf_s(buffer, 120, "%3s, %10s,  , %3s, %4d, %4d, %4d, %4d, %3d, %3d, %7d",
         sprintf_s(buffer, 120, "%s,%s,,%s,%d,%d,%d,%d,%d,%d,%d",
            p1x1, pcall, pLoc,
            cwqsos, phqsos, dgqsos,
            qsopoints, mults, bonus, score);

         file.AddLine(buffer);
      }


   }

}

// Write the SoapBox Comments
void ReportWriter::WriteSoapBoxComments(const string& filename, vector<Station*>& stations)
{
   TextFile file;
   file.AddLine("SoapBox Comments");
   file.AddLine(" ");

   list<string> soapbox;
   for (Station *s : stations)
   {
      s->GetSoapbox(soapbox);
      if (!soapbox.empty())
      {
         file.AddLine(s->StationCallsign());
         for (const string& soap : soapbox)
         {
            file.AddLine(soap);
         }
         file.AddLine(" ");
      }
   }

   file.Write(filename);
}

// Sort the stations by callsign
void ReportWriter::SortStationsByCallsign(vector<Station*>& stationsSorted, vector<Station*>& unsorted)
{
   stationsSorted = unsorted;
   sort(stationsSorted.begin(), stationsSorted.end(), StationCallsignLessThan);
}

void ReportWriter::WriteMOQPMasterResults(const string& filename, vector<Station*>& stations)
{
   vector<Station*> sortedStations;
   SortStationsByCallsign(sortedStations, stations);

   TextFile file;
   for (Station *s : sortedStations)
   {
      string line = s->GetMOQPMasterResult();
      file.AddLine(line);
   }

   file.Write(filename);
}

bool StationSortByInvalidQsosByPercentage(const Station* s1, const Station* s2)
{
	double p1 = s1->GetInvalidQsoPercentage();
	double p2 = s2->GetInvalidQsoPercentage();

	return p1 > p2;
}


bool StationSortByInvalidQsos(const Station* s1, const Station* s2)
{
	int j1 = s1->GetNumberOfInvalidQsos();
	int j2 = s2->GetNumberOfInvalidQsos();

	return j1 > j2;
}

bool StationSortByValidCountiesWorked(const Station* s1, const Station* s2)
{
   int j1 = s1->GetValidCountiesWorked();
   int j2 = s2->GetValidCountiesWorked();

   return j1 > j2;
}

bool StationSortByDigitalQsos(const Station* s1, const Station* s2)
{
   int j1 = s1->GetValidDigitalQsos();
   int j2 = s2->GetValidDigitalQsos();

   return j1 > j2;
}

bool StationSortByPureDigitalScore(const Station* s1, const Station* s2)
{
	int j1 = s1->GetPureDigitalScore();
	int j2 = s2->GetPureDigitalScore();

	return j1 > j2;
}

bool StationSortByVhfWorked(const Station* s1, const Station* s2)
{
   int j1 = s1->GetValidVhfQsos();
   int j2 = s2->GetValidVhfQsos();

   return j1 > j2;
}

bool StationSortBy1x1Count(const Station* s1, const Station* s2)
{
   int j1 = s1->Get1x1CountUnique();
   int j2 = s2->Get1x1CountUnique();

   if (j1 == j2)
   {
      int l1 = (int)s1->StationCallsign().length();
      int l2 = (int)s2->StationCallsign().length();

      return l1 < l2;
   }

   return  j1 > j2;
}

// Write out a file with the count of the number of valid qso's with 1x1 stations for each station
void ReportWriter::Write1x1StationCounts(const string& filename, vector<Station*>& inputStations)
{
   vector<Station*> stations;

   stations = inputStations;

//   printf("ReportWriter::Write1x1StationCounts Begin\n");
   sort(stations.begin(), stations.end(), StationSortBy1x1Count);

//   printf("ReportWriter::Write1x1StationCounts After Sort\n");
   char buffer[80];
   TextFile file;
   for (Station *s : stations)
   {
      const string& sCallsign = s->StationCallsign();
      const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
      sprintf_s(buffer, 80, "%12s, %d", callsign, s->Get1x1CountUnique());
//      printf("%s\n", buffer);
      string text(buffer);
      file.AddLine(text);
   }

   file.Write(filename);
}

void ReportWriter::WriteCountiesWorkedResults(const string& filename, vector<Station*>& inputStations, const string& title)
{
   vector<Station*> stations;
   stations = inputStations;

   sort(stations.begin(), stations.end(), StationSortByValidCountiesWorked);

   char buffer[80];
   TextFile file;
   file.AddLine(title);
   file.AddLine(" ");
   int count = 0;
   for (Station *s : stations)
   {
      const string& sCallsign = s->StationCallsign();
      const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
      count = s->GetValidCountiesWorked();
      sprintf_s(buffer, 80, "%12s, %d", callsign, count);
      string text(buffer);
      file.AddLine(text);
   }

file.Write(filename);
}

// Write out the results for digital stations, ignoring stations with a 0 score
void ReportWriter::WriteDigitalResults(const string& filename, vector<Station*>& inputStations, const string& title)
{
   vector<Station*> stations;
   stations = inputStations;

   sort(stations.begin(), stations.end(), StationSortByDigitalQsos);

   char buffer[80];
   TextFile file;
   file.AddLine(title);
   file.AddLine(" ");
   int dig = 0;
   for (Station *s : stations)
   {
      const string& sCallsign = s->StationCallsign();
      const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
      dig = s->GetValidDigitalQsos();
      sprintf_s(buffer, 80, "%12s, %d", callsign, dig);
      //      printf("%s\n", buffer);
      string text(buffer);
      file.AddLine(text);

      if (dig == 0)
      {
         break;
      }
   }

   file.Write(filename);
}

// 'Pure' digital results considers only digital qso's and multipliers from digital qso's
void ReportWriter::WritePureDigitalResults(const string& filename, vector<Station*>& inStateStations, vector<Station*>& outStateStations)
{
	TextFile file;
	string title;
	title = "Pure Digital Qso's";
	file.AddLine(title);
	title = "Pure Digital Qso's consider only digital qso's and multipliers from digital qso's";
	file.AddLine(title);
	file.AddLine(" ");

	title = " ***** In State Digital Stations *****";
	WritePureDigitalResults2(title, inStateStations, file);
	file.AddLine(" ");

	title = " ***** Out of State Digital Stations *****";
	WritePureDigitalResults2(title, outStateStations, file);


	file.Write(filename);
}

void ReportWriter::WritePureDigitalResults2(const string& mainTitle, vector<Station*>& inputStations, TextFile& file)
{
	vector<Station*> stations;
	stations = inputStations;

	sort(stations.begin(), stations.end(), StationSortByPureDigitalScore);

	string title;
	file.AddLine(mainTitle);
	file.AddLine(" ");
	title = "callsign, pure digital score, digital qso's, digital multipliers";
	file.AddLine(title);
	file.AddLine(" ");

	char buffer[80];
	int qsos = 0;
	int mults = 0;
	int pureScore = 0;
	for (Station* s : stations)
	{
		qsos = s->GetValidDigitalQsos();
		pureScore = s->GetPureDigitalScore();
		mults = s->GetDigitalMultipliers();
		const string& sCallsign = s->StationCallsign();
		const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
		if (qsos > 0)
		{
			sprintf_s(buffer, 80, "%12s, %8d, %3d, %3d", callsign, pureScore, qsos, mults);
			file.AddLine(buffer);
		}
	}
}

void ReportWriter::WriteVhfResults(const string& filename, vector<Station*>& inputStations, const string& title)
{
   vector<Station*> stations;
   stations = inputStations;

   sort(stations.begin(), stations.end(), StationSortByVhfWorked);

   char buffer[80];
   TextFile file;
   file.AddLine(title);
   file.AddLine(" ");
   int dig = 0;
   for (Station *s : stations)
   {
      const string& sCallsign = s->StationCallsign();
      const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
      dig = s->GetValidVhfQsos();
      sprintf_s(buffer, 80, "%12s, %d", callsign, dig);
      //      printf("%s\n", buffer);
      string text(buffer);
      file.AddLine(text);

      if (dig == 0)
      {
         break;
      }
   }

   file.Write(filename);
}

// Sort the stations by the number of Invalid Qso's
void ReportWriter::WriteStationsSortedByInvalidQsos(const string& filename, vector<Station*>& inputStations, const string& title)
{
	vector<Station*> stations;
	stations = inputStations;

	sort(stations.begin(), stations.end(), StationSortByInvalidQsos);

	char buffer[80];
	TextFile file;
	file.AddLine(title);
	file.AddLine(" ");
	int count = 0;
	for (Station *s : stations)
	{
		const string& sCallsign = s->StationCallsign();
		const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
		count = s->GetNumberOfInvalidQsos();
		sprintf_s(buffer, 80, "%12s, %d", callsign, count);
		//      printf("%s\n", buffer);
		string text(buffer);
		file.AddLine(text);
	}

	file.Write(filename);
}

// Sort the stations by percentage of invalid qso's
void ReportWriter::WriteStationsSortedByInvalidQsosByPercentage(const string& filename, vector<Station*>& inputStations, const string& title)
{
	vector<Station*> stations;
	stations = inputStations;

	sort(stations.begin(), stations.end(), StationSortByInvalidQsosByPercentage);

	char buffer[80];
	TextFile file;
	file.AddLine(title);
	file.AddLine(" ");
	double percentage = 0.0;
	int count = 0;
	for (Station *s : stations)
	{
		const string& sCallsign = s->StationCallsign();
		const char* callsign = sCallsign.empty() ? "<callsign>" : sCallsign.c_str();
		percentage = s->GetInvalidQsoPercentage();
		count = s->GetNumberOfInvalidQsos();
		sprintf_s(buffer, 80, "%12s, %7.2f%%,   %d invalid q's", callsign, percentage, count);
		//      printf("%s\n", buffer);
		string text(buffer);
		file.AddLine(text);
	}

	file.Write(filename);
}

