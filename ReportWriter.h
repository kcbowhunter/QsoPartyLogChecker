
#pragma once

class Station;
class AllLocations;
class CategoryMgr;
struct Category;
class TextFile;
class Contest;

class ReportWriter
{
public:
   // Write Instate / Out of State summary file
   static void InOutStateStations(bool instate, const string& filename, vector<Station*>& stations);

   // Write out the County Count 
   static void WriteCountyCount(map<string, int>& countyCount, map<string, string>& countyMap, string fileName);

   // Write out the given stations sorted by score
   static void ReportWriter::WriteStationsByScore(const string& filename, list<Station*>& stations, const string& title);

   // Write out the given stations sorted by score
   static void ReportWriter::WriteStationsByScore(const string& filename, vector<Station*>& stations, const string& title);

   // Write stations from the given country sorted by score
   static void WriteStationsByScoreAndCountry(const string& country, const string& filename, vector<Station*>& stations, const string& title);

   // Write InState or Out of Stations sorted by score
   static void WriteStationsByScoreAndInState(bool instate, const string& filename, vector<Station*>& stations, const string& title);

   // Summary Report for each US State
   static void WriteStateSummaryReport(const string& filename, vector<Station*>& stations, AllLocations* allLocations);

   static void WriteMobileStationsByScore(const string& reportRoot, vector<Station*>& stations);

   // Write the master summary for all categories
   // iFlag = 1-> Generic Summary
   // iFlag = 2 -> KSQP Style Summary
   static void WriteCategorySummary(Contest *contest, const string& filename, CategoryMgr *categoryMgr, int iFlag);

   // Write one category to the given text file (but do not write the file to disk)
   static void WriteCategory(TextFile& file, Category* cat, int iFlag);

   // Write the category data out in the order the categories appear in the categories file
   static void WriteCategorySummaryInOrder(Contest *contest, const string& filename, CategoryMgr *categoryMgr);

   // Write the SoapBox Comments
   static void WriteSoapBoxComments(const string& filename, vector<Station*>& stations);

   // Write out the master reports file for the MOQP
   static void WriteMOQPMasterResults(const string& filename, vector<Station*>& stations);

   // Sort the stations by callsign
   static void SortStationsByCallsign(vector<Station*>& stationsSorted, vector<Station*>& unsorted);

   // Write out a file with the count of the number of valid qso's with 1x1 stations for each station
   static void Write1x1StationCounts(const string& filename, vector<Station*>& m_stations);

   // Write out the results for digital stations, ignoring stations with a 0 score
   static void WriteDigitalResults(const string& filename, vector<Station*>& inputStations, const string& title);

   // 'Pure' digital results considers only digital qso's and multipliers from digital qso's
   static void WritePureDigitalResults(const string& filename, vector<Station*>& inStateStations, vector<Station*>& outStateStations);
   static void WritePureDigitalResults2(const string& mainTitle, vector<Station*>& stations, TextFile& file);

   static void WriteCountiesWorkedResults(const string& filename, vector<Station*>& inputStations, const string& title);

   static void WriteVhfResults(const string& filename, vector<Station*>& inputStations, const string& title);

   static void WriteStationsSortedByInvalidQsos(const string& filename, vector<Station*>& inputStations, const string& title);

   static void WriteStationsSortedByInvalidQsosByPercentage(const string& filename, vector<Station*>& inputStations, const string& title);

private:
   ReportWriter();
   ~ReportWriter();
};