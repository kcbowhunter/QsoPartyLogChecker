
#pragma once

class Station;

class Location;

class QsoItem;
class QsoTokenType;

class QsoError;
class QsoInfo;
class AllLocations;
class Contest;
class DxccCountryManager;

enum HamBand { eUnknownBand=-100, eAnyBand=-10, 
               e160m=0, e80m=1, e40m=2, e20m=3, e15m=4, e10m=5, e6m=6, e2m=7, e440=8, eHamBandSize=11};

enum QsoMode { eUnknownMode, eAnyMode, eModePhone, eModeCw, eModeDigital};

#include "Freq.h"
#include "Mode.h"
#include "Date.h"
#include "QsoTime.h"
#include "QsoName.h"

#include "Callsign.h"
#include "Rst.h"
#include "SerialNumber.h"

class Qso
{
public:
   Qso();
	virtual ~Qso();

   // Copy constructor
   Qso(const Qso& qso);

   // C++ Assignment operator
   Qso& operator=(const Qso& qso);

	string OriginalText() const { return m_orig; }

   void Process(const string& line, vector<QsoTokenType*>& tokenTypes);

   const Freq& GetFreq() const { return m_freq; }
   const Mode& GetMode() const { return m_mode; }
   const Date& GetDate() const { return m_date; }
   const QsoTime& GetTime() const { return m_time; }

   const Callsign&     GetMyCallsign() const     { return m_myCall; }
   const Rst&          GetMyRst() const          { return m_myRst; }
   const QsoName&      GetQsoName() const        { return m_myName; }
   const SerialNumber& GetMySerialNumber() const { return m_mySerialNumber; }
   const Location     *GetMyLocation() const     { return m_myLocation; }

   const Callsign&     GetTheirCallsign() const     { return m_theirCall; }
   const Rst&          GetTheirRst() const          { return m_theirRst; }
   const QsoName&      GetTheirName() const         { return m_theirName; }
   const SerialNumber& GetTheirSerialNumber() const { return m_theirSerialNumber; }
   const Location     *GetTheirLocation() const     { return m_theirLocation; }

   // Return the 'qso' minute since Jan 1 2000
   int GetQsoTime() const;

   void SetStation(Station *station) { m_station = station; }
   Station *GetStation() const { return m_station; }

   static double m_dtorTime;

   int  GetNumber() const { return m_number; }
   void SetNumber(int num) { m_number = num; }

   bool IsDuplicate() const { return m_dupeNumber > 0; }
   int GetDuplicateNumber() const { return m_dupeNumber; }
   void SetDuplicateNumber(const int x)  { m_dupeNumber = x; }

   bool GetQsoErrors(list<string>& errors);
   void AddQsoError(QsoError* error);

   bool GetQsoInfos(list<string>& infos);
   void AddQsoInfo(QsoInfo* info);

   // Remove all the qso error objects
   void ClearQsoErrors();

   bool ValidQso() const { return m_qsoErrors.empty(); }
   int  ErrorCount() const { return (int)m_qsoErrors.size(); }

   // Return the nth qso error
   QsoError *GetQsoError(const int num);

   // RefQsoNumber is the qso number for the station worked for the matching qso
   int GetRefQsoNumber() const { return m_refQsoNumber; }
   void SetRefQsoNumber(int x) { m_refQsoNumber = x; }

   // Validate this qso
   bool Validate(Contest *contest);

   // Return true if the qso arg is a duplicate
   bool IsDuplicateQso(Qso* qso, bool cwAndDigitalAreTheSameMode);

   bool Match(HamBand band, QsoMode mode, const string& callsign, const string& myLocation, const string& theirLocation);

   // Copy the freq/mode/date/time data and also copy the call/rst/sn/location data, flipping the source and target
   // This is used when creating missing log files
   bool CopyAndFlip(Qso* source);

   // Get the created text for missing qso's
   string GetCreatedText(vector<QsoTokenType*>& qsoTokenTypes);

   static string GetHamBandString(const HamBand band);

   bool OtherStationMissing() const { return m_otherStationMissing; }

   void SetIgnore(const bool b) { m_ignore = b; }
   bool IsIgnored() const { return m_ignore; }

private:
   Station *m_station;
   Station *m_otherStation;
   bool m_alive;  // true -> set true in ctor, false in dtor

   // if a station mode is ssb and they have a cw qso, the cw qso 
   // is ignored for scoring
   bool m_ignore; // ignore the qso for scoring

   string m_orig;  // original qso from cab file
   string m_origLow;  // lower case verson of m_orig
   int    m_number;  // qso number (order in log file): 1, 2, 3...

   // Created text for missing qso's
   string m_createdText;

   // Reference qso number of the station worked
   int m_refQsoNumber;

   // Duplicate Qso Number: -1 duplicate not checked, =0 no duplicate found, >0 duplicate qso number
   int m_dupeNumber;

   list<string> m_tokens;

   list<QsoError*> m_qsoErrors;
   list<QsoInfo*> m_qsoInfos;

   // frequency, mode, date, time QsoItem map
   map<string, QsoItem*> m_freqMap;

   // qso data map; callsign, rst, serial number, location
   map<string, QsoItem*> m_myQsoMap;
   map<string, QsoItem*> m_theirQsoMap;

   // true -> the station worked in the qso did not submit a log
   bool m_otherStationMissing;

//   string line("QSO:  3541 CW 2014-04-06 0109 AI4WW         599 0001 FL  N0H           599 0616 GAS ");
   Freq m_freq;
   Mode m_mode;
   Date m_date;
   QsoTime m_time;

   Callsign m_myCall;
   Rst      m_myRst;
   QsoName  m_myName;
   SerialNumber m_mySerialNumber;
   Location *m_myLocation;

   Callsign m_theirCall;
   Rst      m_theirRst;
   QsoName  m_theirName;
   SerialNumber m_theirSerialNumber;
   Location *m_theirLocation;

private:
   void Process2( list<string>::iterator& iter, 
                         vector<QsoTokenType*>& tokenTypes, 
                         map<string, QsoItem*>& qsoItemMap, 
                         int& tokenCount, int& errorCount);

   bool CheckInStateLocation(Location *location, const set<string>& locationAbbrevs);

   bool CheckAllLocations(Location *location, AllLocations *allLocations, const set<string>& countyAbbrevs, bool checkCounties, DxccCountryManager *dxccCountryMgr);

   // extract 'my' qso data to the map
   bool GetMyQsoData(map<string, string>& qsoData);

   // extract 'their' qso data to the map
   bool GetTheirQsoData(map<string, string>& qsoData);
};
