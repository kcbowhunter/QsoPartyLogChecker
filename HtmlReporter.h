#pragma once

class Station;


class HtmlReporter
{
public:
   HtmlReporter();
   virtual ~HtmlReporter();

   bool GenerateHtmlReports(map<string, Station*> &stationMap,
                            map<string, Station*> &missingStationsMap,
                            const string& htmlFolder);

private:
   void CreateStationVector(std::vector<Station*>& stationVec, map<string, Station*>& stationMap);
   void CreateStationHtmlFiles(const string& htmlFolder, vector<Station*>& allStationsVec);

   static void CreateStationHtmlFile(Station* pStation, const string& htmlFolder);


};

