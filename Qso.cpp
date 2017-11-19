
#include "stdafx.h"
#include "Qso.h"
#include "StringUtils.h"
#include "Station.h"

#include "QsoTime.h"

#include "Callsign.h"
#include "Rst.h"
#include "SerialNumber.h"
#include "Location.h"

#include "QsoTokenType.h"
#include "QsoError.h"
#include "QsoInfo.h"

#include "AllLocations.h"
#include "Contest.h"
#include "DxccCountryManager.h"
#include "DxccCountry.h"

double Qso::m_dtorTime = 0.0;

Qso::Qso()
   :
   m_station(nullptr),
   m_otherStation(nullptr),
   m_number(-1),
   m_freq(this),
   m_mode(this),
   m_date(this),
   m_time(this),
   m_myCall(this),
   m_myRst(this),
   m_myName(this),
   m_mySerialNumber(this),
   m_myLocation(new Location(this)),
   m_theirCall(this),
   m_theirRst(this),
   m_theirName(this),
   m_theirSerialNumber(this),
   m_theirLocation(new Location(this)),
   m_refQsoNumber(-1),
   m_dupeNumber(-1),
   m_alive(true),
   m_ignore(false),
   m_otherStationMissing(true)
{
   m_freqMap["freq"] = &m_freq;
   m_freqMap["mode"] = &m_mode;
   m_freqMap["date"] = &m_date;
   m_freqMap["time"] = &m_time;

   m_myQsoMap["callsign"]     = &m_myCall;
   m_myQsoMap["rst"]          = &m_myRst;
   m_myQsoMap["name"]         = &m_myName;
   m_myQsoMap["location"]     = m_myLocation;
   m_myQsoMap["serialnumber"] = &m_mySerialNumber;

   m_theirQsoMap["callsign"]     = &m_theirCall;
   m_theirQsoMap["rst"]          = &m_theirRst;
   m_theirQsoMap["name"]         = &m_theirName;
   m_theirQsoMap["location"]     = m_theirLocation;
   m_theirQsoMap["serialnumber"] = &m_theirSerialNumber;
}

// Copy constructor
Qso::Qso(const Qso& qso)
:
m_station(qso.m_station),
m_otherStation(qso.m_otherStation),
m_orig(),
m_origLow(),
m_number(-1),
m_dupeNumber(-1),
m_createdText(),
m_refQsoNumber(-1),
m_tokens(),
m_qsoErrors(),
m_qsoInfos(),
m_freqMap(),
m_myQsoMap(),
m_theirQsoMap(),
m_freq(qso.m_freq),
m_mode(qso.m_mode),
m_date(qso.m_date),
m_time(qso.m_time),
m_myCall(qso.m_myCall),
m_myRst(qso.m_myRst),
m_myName(qso.m_myName),
m_mySerialNumber(qso.m_mySerialNumber),
m_myLocation(new Location(*qso.m_myLocation)),
m_theirCall(qso.m_theirCall),
m_theirRst(qso.m_theirRst),
m_theirName(qso.m_theirName),
m_theirSerialNumber(qso.m_theirSerialNumber),
m_theirLocation(new Location(*qso.m_theirLocation)),
m_alive(true)
{
}

// C++ Assignment operator
Qso& Qso::operator=(const Qso& qso)
{
   if (this == &qso)
      return *this;

   m_station = qso.m_station;
   m_otherStation = qso.m_otherStation;
   m_orig = qso.m_orig;
   m_origLow = qso.m_origLow;
   m_number = -1;
   m_dupeNumber = -1;
   m_refQsoNumber = -1;
   m_tokens.clear();
   m_qsoErrors.clear();
   m_qsoInfos.clear();
   m_freqMap.clear();
   m_myQsoMap.clear();
   m_theirQsoMap.clear();
   m_freq.Copy(qso.m_freq);
   m_mode.Copy(qso.m_mode);
   m_date.Copy(qso.m_date);
   m_time.Copy(qso.m_time);

   m_myCall.Copy(qso.m_myCall);
   m_myRst.Copy(qso.m_myRst);
   m_myName.Copy(qso.m_myName);
   m_mySerialNumber.Copy(qso.m_mySerialNumber);
   m_myLocation = new Location(*qso.m_myLocation);

   m_theirCall.Copy(qso.m_theirCall);
   m_theirRst.Copy(qso.m_theirRst);
   m_theirName.Copy(qso.m_theirName);
   m_theirSerialNumber.Copy(qso.m_theirSerialNumber);
   m_theirLocation = new Location(*qso.m_theirLocation);

   return *this;
}


Qso::~Qso()
{
   double start = omp_get_wtime();

   if (m_myLocation != nullptr) delete m_myLocation;

   if (m_theirLocation != nullptr) delete m_theirLocation;

   for (QsoError *qsoError : m_qsoErrors)
   {
      delete qsoError;
   }
   m_qsoErrors.clear();

   for (QsoInfo* qsoInfo : m_qsoInfos)
   {
      delete qsoInfo;
   }
   m_qsoInfos.clear();

   double finish = omp_get_wtime();
//   m_dtorTime += finish - start;

   m_alive = false;
}

// Parse and process the tokens in the string 'line' using the token order in tokenTypes
// Each token in tokenTypes corresponds to a token in 'line'
// The tokens must be in the order of 3 groups:
//  a) freq, mode, date, time
//  b) my callsign, my rst, my serial number, my location
//  c) their callsign, their rst, their serial number, their location
void Qso::Process(const string& line, vector<QsoTokenType*>& tokenTypes)
{
	m_orig = line;
   m_origLow = line;
   StringUtils::ToLower(m_origLow);

   StringUtils::Split(m_tokens, m_origLow);

   bool status = true;
   int errorCount = 0;
   int tokenCount = 0;
   // Process freq, mode, date, time
   auto iter = m_tokens.begin();

   Process2(iter, tokenTypes, m_freqMap, tokenCount, errorCount);

   Process2(iter, tokenTypes, m_myQsoMap, tokenCount, errorCount);

   Process2(iter, tokenTypes, m_theirQsoMap, tokenCount, errorCount);

   AllLocations *allLocations = m_station == nullptr ? nullptr : m_station->GetAllLocations();
   if (m_station != nullptr && allLocations != nullptr)
   {
      if (m_station->InState())
      {
         bool InstateDxccMults = m_station->InStateDxccMults();
		 DxccCountryManager *dxccCountryMgr = InstateDxccMults ? m_station->GetDxccCountryManager() : nullptr;
         const set<string>& countyAbbrevs = m_station->GetCountyAbbrevs();
         CheckInStateLocation(m_myLocation, countyAbbrevs);
		 CheckAllLocations(m_theirLocation, allLocations, countyAbbrevs, true, dxccCountryMgr);
      }
      else
      {
         const set<string>& countyAbbrevs = m_station->GetCountyAbbrevs();
         CheckInStateLocation(m_theirLocation, countyAbbrevs);

		 DxccCountryManager *dxccCountryMgr = m_station->GetDxccCountryManager();
         CheckAllLocations(m_myLocation, allLocations, countyAbbrevs, false, dxccCountryMgr);
      }
   }
}

bool Qso::CheckAllLocations(Location *location, AllLocations *allLocations, const set<string>& countyAbbrevs, bool checkCounties, DxccCountryManager *dxccCountryManager)
{
   string loc = location->GetValue();

   if (loc == "dx")
      return true;

   // Kansas Qso Party Special Code KSQP
   if (m_station->InState())
   {
      if (loc == "ks")
         return true;
   }

   if (checkCounties)
   {
      auto iter = countyAbbrevs.find(loc);
      if (iter != countyAbbrevs.end())
         return true;
   }

   bool status = allLocations->IsValidLocation(loc);

   // If the location is still not valid, is it an arrl section?
   if (!status)
   {
      string spc = allLocations->GetStateOrProvinceFromSection(loc);
      if (!spc.empty())
      {
         // Update the location logged as an arrl section with the corresponding state or province
         StringUtils::ToLower(spc);
         location->SetValue(spc);
         return true;
      }
   }

   if (!status && dxccCountryManager != nullptr)
   {
	   DxccCountry *dxcc = dxccCountryManager->FindCountry(loc);
	   if (dxcc != nullptr)
	   {
		   location->SetDxccCountry(dxcc);
		   status = true;
	   }
   }

   if (!status)
   {
      char buffer[80];
      string call = m_station->StationCallsign();
	  string badLocation(loc);
	  StringUtils::ToUpper(badLocation);
      const char* locText = badLocation.empty() ? "(Nil)" : badLocation.c_str();

      sprintf_s(buffer, 80, "%10s Error: Bad Location %s in qso %d", call.c_str(), locText, m_number);
      printf("%s\n", buffer);
      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      m_qsoErrors.push_back(qsoerror);
   }

   return status;
}

bool Qso::CheckInStateLocation(Location *location, const set<string>& locationAbbrevs)
{
   bool status = location->Validate(locationAbbrevs);
   if (!status)
   {
      char buffer[80];
      string call = m_station->StationCallsign();
      string error = location->GetErrorString();
      sprintf_s(buffer, 80, "%10s Error: %s in qso %d", call.c_str(), error.c_str(), m_number);
      printf("%s\n", buffer);
      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      m_qsoErrors.push_back(qsoerror);
   }
   return true;
}

void Qso::Process2(list<string>::iterator& iter, 
                   vector<QsoTokenType*>& tokenTypes, 
                   map<string, QsoItem*>& qsoItemMap, 
                   int& tokenCount, int& errorCount)
{
   set<string> tokenTypesProcessed;

   for (; iter != m_tokens.end(); ++iter)
   {
      string token = (*iter);
      if (tokenCount >= (int)tokenTypes.size())
      {
         ++errorCount;  // extra token found
         continue;
      }

      QsoTokenType *qsoTokenType = tokenTypes[tokenCount++];
      string tokenTypeString = qsoTokenType->TokenType();

      // don't process the same tokenType twice
      auto setIter = tokenTypesProcessed.find(tokenTypeString);
      if (setIter == tokenTypesProcessed.end())
      {
         tokenTypesProcessed.insert(tokenTypeString);
      }
      else
      {
         --tokenCount;
         break;
      }

      auto mapiter = qsoItemMap.find(tokenTypeString);
      if (mapiter == qsoItemMap.end())
      {
         ++errorCount;  // token type not found
         --tokenCount;  // token not processed
         break;
      }

      QsoItem *qsoItem = (*mapiter).second;
      bool status = qsoItem->ProcessToken(token, this);
      if (!status)
      {
         ++errorCount;  // error processing token
         continue;
      }
   }
}

void Qso::AddQsoError(QsoError *error)
{
	const string& callsignLower = m_station->StationCallsignLower();

//	if (callsignLower == "k1ro")
//	{
//		const char* errorMsg = error->m_error.empty() ? "<null>" : error->m_error.c_str();
//		printf("AddQsoError %s, %s\n", callsignLower.c_str(), errorMsg);
//	}

    m_qsoErrors.push_back(error);
}

bool Qso::GetQsoErrors(list<string>& errors)
{
   errors.clear();
   if (m_qsoErrors.empty())
      return false;

   for (QsoError *error : m_qsoErrors)
   {
      errors.push_back(error->m_error);
   }

   return true;
}

// Remove all the qso error objects
void Qso::ClearQsoErrors()
{
   for (QsoError *error : m_qsoErrors)
   {
      delete error;
   }

   m_qsoErrors.clear();
}


void Qso::AddQsoInfo(QsoInfo* info)
{
   m_qsoInfos.push_back(info);
}

bool Qso::GetQsoInfos(list<string>& infos)
{
   infos.clear();
   for (QsoInfo* info : m_qsoInfos)
   {
      infos.push_back(info->m_msg);
   }

   return true;
}

// Validate this qso
bool Qso::Validate(Contest *contest)
{
   if (!ValidQso())
      return false;

   bool checkSerialNumber = false;

   string myCallsign = m_station->StationCallsignLower();
//   printf("Validating Qso: [%10s] %s\n", myCallsign.c_str(), m_orig.c_str());

   string theirCallsign = m_theirCall.GetCallsign();

   m_otherStation = contest->GetStation(theirCallsign);

   // If the other station did not submit a logfile, return true
   if (m_otherStation == nullptr)
   {
      m_station->AddMissingStation(theirCallsign);
      m_otherStationMissing = true;
      return true;
   }

   m_otherStationMissing = false;

   HamBand band = m_freq.GetBand();
   QsoMode mode = m_mode.GetMode();
   string  myCall = m_myCall.GetCallsign();
   string myLocation = m_myLocation->GetValue();
   string theirLocation = m_theirLocation->GetValue();
   Qso *qso = m_otherStation->FindAvailableQso(band, mode, myCallsign, theirLocation, myLocation);

   if (qso == nullptr)
   {
      QsoError *qsoerror = new QsoError(eQsoNotFoundInLog);

      string mycallsign = m_station->StationCallsign();

      char buffer[80];
	  string theirCall(theirCallsign);
	  StringUtils::ToUpper(theirCall);
      sprintf_s(buffer, 80, "Station %s,  qso[%4d] with station %s not found in log", mycallsign.c_str(), m_number, theirCall.c_str());
//      printf("%s\n", buffer);

      qsoerror->m_error = string(buffer);
      this->AddQsoError(qsoerror);
      return false;
   }
   else if (checkSerialNumber)
   {
      // Check the serial number 
      int sn1 = m_theirSerialNumber.GetSerialNumber();
      int sn2 = qso->GetMySerialNumber().GetSerialNumber();
      if (sn1 != sn2)
      {
         string callsign = qso->GetMyCallsign().GetCallsign();
         char buffer[80];
         sprintf_s(buffer, 80, "Bad Serial Number: Logged %d but station %s sent %d", sn1, callsign.c_str(), sn2);
         QsoError *error = new QsoError();
         error->m_error = string(buffer);
         this->AddQsoError(error);
         return false;
      }
   }

   SetRefQsoNumber(qso->GetNumber());
   qso->SetRefQsoNumber(GetNumber());

   return true;
}

bool Qso::Match(HamBand band, QsoMode mode, const string& callsign, const string& myLocation, const string& theirLocation)
{
   if (m_freq.GetBand() != band)
      return false;

   if (m_mode.GetMode() != mode)
      return false;

   string theirCallsign = m_theirCall.GetCallsign();

   if (theirCallsign != callsign)
      return false;

   string theirLoc = m_theirLocation->GetValue();
   if (theirLocation != theirLoc)
      return false;

   string myLoc = m_myLocation->GetValue();
   if (myLoc != myLocation)
      return false;

   return true;
}

// Return true if the qso arg is a duplicate
bool Qso::IsDuplicateQso(Qso* qso, bool cwAndDigitalAreTheSameMode)
{
   if (m_freq.GetBand() != qso->GetFreq().GetBand())
      return false;

   // If cw and digital are treated as the same mode, then one mode must be phone and the other
   // cw or digital for the duplicate qso test to fail at this point
   if (cwAndDigitalAreTheSameMode)
   {
      QsoMode mode1  = m_mode.GetMode();
      bool    cwdig1 = mode1 == eModeCw || mode1 == eModeDigital;
      QsoMode mode2  = qso->GetMode().GetMode();
      bool    cwdig2 = mode2 == eModeCw || mode2 == eModeDigital;

      if (cwdig1 && cwdig2 || mode1 == mode2)
      {
         ;  // both qso's are cw or digital or both are the same qso could be a duplicate
      }
      else
      {
         return false;  // the modes are different and they are not both cw or digital
      }
   }
   else if (m_mode.GetMode() != qso->GetMode().GetMode())
   {
      return false;
   }


   const string theirCalla = m_theirCall.GetCallsign();
   const string theirCallb = qso->GetTheirCallsign().GetCallsign();
   if (theirCalla != theirCallb)
      return false;

   const string myLoca = m_myLocation->GetValue();
   const string myLocb = qso->GetMyLocation()->GetValue();
   if (myLoca != myLocb)
      return false;

   const string theirLoca = m_theirLocation->GetValue();
   const string theirLocb = qso->GetTheirLocation()->GetValue();
   if (theirLoca != theirLocb)
      return false;

   return true;
}

// Copy the freq/mode/date/time data and also copy the call/rst/sn/location data, flipping the source and target
// This is used when creating missing log files
bool Qso::CopyAndFlip(Qso* source)
{
   if (source == nullptr)
      return false;

//   string line("14007  CW  2014-04-05  1402 KC0M   599   0002  TAN   WA6KHK 599  0001 CA");

   m_freq.Copy(source->m_freq);
   m_mode.Copy(source->m_mode);
   m_date.Copy(source->m_date);
   m_time.Copy(source->m_time);

   // Flip
   m_myCall.Copy(source->m_theirCall);
   m_theirCall.Copy(source->m_myCall);

   m_myRst.Copy(source->m_theirRst);
   m_theirRst.Copy(source->m_myRst);

   m_myName.Copy(source->m_theirName);
   m_theirName.Copy(source->m_myName);

   m_mySerialNumber.Copy(source->m_theirSerialNumber);
   m_theirSerialNumber.Copy(source->m_mySerialNumber);

   m_myLocation->Copy(source->m_theirLocation);
   m_theirLocation->Copy(source->m_myLocation);

   return true;
}

// Get the created text for missing qso's
string Qso::GetCreatedText(vector<QsoTokenType*>& qsoTokenTypes)
{
   if (!m_createdText.empty())
      return m_createdText;

//   char buffer[120];
   string qsoString;

   int tokenCount = (int)qsoTokenTypes.size();
   int tokensPerQso = (tokenCount - 4) / 2;

   //   string line("14007  CW  2014-04-05  1402 KC0M   599   0002  TAN   WA6KHK 599  0001 CA");
   string freqString = m_freq.GetValue();
   const char* freq = freqString.empty() ? "freq" : freqString.c_str();

   string modeString = m_mode.GetValue();
   StringUtils::ToUpper(modeString);
   const char* mode = modeString.empty() ? "??" : modeString.c_str();

   string dateString = m_date.GetValue();
   const char* date = dateString.empty() ? "date" : dateString.c_str();

   string timeString = m_time.GetValue();
   const char* time = timeString.empty() ? "time" : timeString.c_str();

   string sFreq = string(freq);
   StringUtils::PadRight(sFreq, ' ', 5);
   qsoString = sFreq + string(" ") + string(mode) + string(" ") + string(date) + string(" ") + string(time) + string(" ");

   map<string, string> qsoData;
   bool status = GetMyQsoData(qsoData);

   int minStringSize = 0;
   int i = 4;
   int k = 4 + tokensPerQso;
   for (; i < k; ++i)
   {
      string tokenType = qsoTokenTypes[i]->TokenType();
      string tokenValue = qsoData[tokenType];
      StringUtils::ToUpper(tokenValue);
      minStringSize = qsoTokenTypes[i]->GetMinSize();
      if (minStringSize > 0)
      {
         StringUtils::PadRight(tokenValue, ' ', minStringSize);
      }
      qsoString = qsoString + tokenValue + string(" ");
   }

   status = GetTheirQsoData(qsoData);
   i = k;
   k = k + tokensPerQso;
   for (; i < k; ++i)
   {
      string tokenType = qsoTokenTypes[i]->TokenType();
      string tokenValue = qsoData[tokenType];
      StringUtils::ToUpper(tokenValue);
      minStringSize = qsoTokenTypes[i]->GetMinSize();
      if (minStringSize > 0)
      {
         StringUtils::PadRight(tokenValue, ' ', minStringSize);
      }
      qsoString = qsoString + tokenValue + string(" ");
   }

   m_createdText = qsoString;

   // Missouri Qso Party with serial number
   //sprintf_s(buffer, 120, " %5s %2s %10s %4s %10s %3s %4s %4s %10s %3s %4s %4s",
   //   freq, mode, date, time,
   //   myCall, myRst, mySerial, myLoc,
   //   theirCall, theirRst, theirSerial, theirLoc);

   // Kansas Qso Party
   //sprintf_s(buffer, 120, "%5s %2s %10s %4s %-10s %-3s %4s %-10s %-3s %4s",
   //   freq, mode, date, time,
   //   myCall, myRst, myLoc,
   //   theirCall, theirRst, theirLoc);

   //m_createdText = string(buffer);

   return m_createdText;
}

// extract 'my' qso data to the map
bool Qso::GetMyQsoData(map<string, string>& qsoData)
{
   qsoData.clear();

   // my qso data
   string myCallString = m_myCall.GetCallsign();
   StringUtils::ToUpper(myCallString);
   const char* myCall = myCallString.empty() ? "call" : myCallString.c_str();
   qsoData["callsign"] = string(myCall);

   string myRstString = m_myRst.GetValue();
   const char* myRst = myRstString.empty() ? "rst" : myRstString.c_str();
   qsoData["rst"] = string(myRst);

   string myNameString = m_myName.GetValue();
   const char* myName = myNameString.empty() ? "name" : myNameString.c_str();
   qsoData["name"] = string(myName);

   string mySerialString = m_mySerialNumber.GetValue();
   const char* mySerial = mySerialString.empty() ? "0000" : mySerialString.c_str();
   qsoData["serialnumber"] = string(mySerial);

   string myLocationString = m_myLocation->GetValue();
   StringUtils::ToUpper(myLocationString);
   const char* myLoc = myLocationString.empty() ? "loc" : myLocationString.c_str();
   qsoData["location"] = string(myLoc);

   return true;
}

// extract 'their' qso data to the map
bool Qso::GetTheirQsoData(map<string, string>& qsoData)
{
   qsoData.clear();

   // their qso data
   string theirCallString = m_theirCall.GetCallsign();
   StringUtils::ToUpper(theirCallString);
   const char* theirCall = theirCallString.empty() ? "call" : theirCallString.c_str();
   qsoData["callsign"] = string(theirCall);

   string theirRstString = m_theirRst.GetValue();
   const char* theirRst = theirRstString.empty() ? "rst" : theirRstString.c_str();
   qsoData["rst"] = string(theirRst);

   string theirNameString = m_theirName.GetValue();
   const char* theirName = theirNameString.empty() ? "name" : theirNameString.c_str();
   qsoData["name"] = string(theirName);

   string theirSerialString = m_theirSerialNumber.GetValue();
   const char* theirSerial = theirSerialString.empty() ? "0000" : theirSerialString.c_str();
   qsoData["serialnumber"] = string(theirSerial);

   string theirLocationString = m_theirLocation->GetValue();
   StringUtils::ToUpper(theirLocationString);
   const char* theirLoc = theirLocationString.empty() ? "loc" : theirLocationString.c_str();
   qsoData["location"] = string(theirLoc);

   return true;
}


string Qso::GetHamBandString(const HamBand band)
{
   string str("?");

   switch (band)
   {
   case e160m: return string("160");
   case e80m: return string("80");
   case e40m: return string("40");
   case e20m: return string("20");
   case e15m: return string("15");
   case e10m: return string("10");
   case e2m:  return string("2");
   case e440: return string("440");
   default: return string("?");
   }

   return str;
}

// Return the 'qso' minute since Jan 1 2000
int Qso::GetQsoTime() const
{
   return m_date.m_minutes + m_time.m_minutes;
}

// Return the nth qso error
QsoError *Qso::GetQsoError(const int num)
{
   if (num < 0)
   {
      return nullptr;
   }

   if (num >= (int)m_qsoErrors.size())
   {
      return nullptr;
   }

   int count = 0;
   for (QsoError *e : m_qsoErrors)
   {
      if (count++ == num)
      {
         return e;
      }
   }

   return nullptr;
}
