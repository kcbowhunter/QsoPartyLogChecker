#include "stdafx.h"
#include "HtmlReporter.h"
#include "HtmlUtils.h"
#include "Station.h"
#include "TextFile.h"

HtmlReporter::HtmlReporter()
{
}


HtmlReporter::~HtmlReporter()
{
}

// Generate the HTRL reports
bool HtmlReporter::GenerateHtmlReports(map<string, Station*> &stationMap,
                                       map<string, Station*> &missingStationsMap,
                                       const string& htmlFolder)
{
   // Create vectors of pointers to stations from the maps because OPENMP only iterates over vectors
   std::vector<Station*> stationVec;
   std::vector<Station*> missingStationVec;

   CreateStationVector(stationVec, stationMap);
   CreateStationVector(missingStationVec, missingStationsMap);

   // Create a vector of pointers to all stations
   std::vector<Station*> allStationsVec;
   allStationsVec.reserve(stationVec.size() + missingStationVec.size());
   allStationsVec.insert(allStationsVec.end(), stationVec.begin(), stationVec.end());
   allStationsVec.insert(allStationsVec.end(), missingStationVec.begin(), missingStationVec.end());

   // Create html files for each station
   CreateStationHtmlFiles(htmlFolder, allStationsVec);

   return true;
}

// Size the vector and fill the vector of stations from the station map
void HtmlReporter::CreateStationVector(std::vector<Station*>& stationVec, map<string, Station*>& stationMap)
{
   stationVec.resize(stationMap.size());

   auto iter = stationMap.begin();
   int i = 0;
   for (; iter != stationMap.end(); ++iter, ++i)
   {
      stationVec[i] = (*iter).second;
   }
}

void HtmlReporter::CreateStationHtmlFiles(const string& htmlFolder, vector<Station*>& allStationsVec)
{
   Station** pStations = &allStationsVec[0];
   int stationCount = (int)allStationsVec.size();
   int i = 0;
   Station* pStation = nullptr;
   for (; i < stationCount; ++i)
   {
      pStation = pStations[i];
      HtmlReporter::CreateStationHtmlFile(pStation, htmlFolder);
   }

}

void HtmlReporter::CreateStationHtmlFile(Station* pStation, const string& htmlFolder)
{
   vector<string> lines;

   HtmlUtils::AddDocType(lines);
   HtmlUtils::AddStartTag(lines, "html");
   HtmlUtils::AddStartTag(lines, "head");

   const string& callsign = pStation->StationCallsign();
   HtmlUtils::AddTagWithContent(lines, "Title", callsign);

   HtmlUtils::AddEndTag(lines, "head");

//   HtmlUtils::AddStartTag(lines, "body");
   HtmlUtils::AddStartTag(lines, "body", "bgcolor", "#00BFFF");

   string content = string("Amateur Radio Station: ") + callsign;
   HtmlUtils::AddTagWithContent(lines, "h1", content.c_str());
   HtmlUtils::AddEndTag(lines, "body");

   HtmlUtils::AddEndTag(lines, "html");

   TextFile htmlFile;
   htmlFile.AddLines(lines);

   string filename = htmlFolder + callsign + ".html";
   htmlFile.Write(filename);
}

