
#pragma once

class Station;
class TextFile;

class Club
{
public:
   Club();
   virtual ~Club();

   void SetName(const string& name) { m_name = name; }
   const string& GetName() const { return m_name; }

   int Score() const { return m_score; }

   // Calculate the club score
   void CalcScore();

   // Add this station to the club;
   bool AddStation(Station *s);

   // Write the scores for each station to the file
   void WriteScores(TextFile& file);

private:
   int m_score;
   string m_name;

   // map of lower case callsign, Station*
   map<string, Station*> m_stationMap;
};