
#include "stdafx.h"
#include "Station.h"
#include "Qso.h"
#include "StringUtils.h"
#include "boost/algorithm/string.hpp"
using namespace boost;

#include "Contest.h"
#include "ContestConfig.h"
#include "TextFile.h"
#include "Location.h"
#include "AllLocations.h"
#include "QsoError.h"
#include "QsoInfo.h"
#include "Mode.h"
#include "Callsign.h"
#include "SerialNumber.h"
#include "Category.h"
#include "Callsign.h"
#include "QsoTokenType.h"
#include "DxccCountryManager.h"
#include "DxccCountry.h"
#include "MultipliersMgr.h"
#include "StationResults.h"
#include "BonusStationsPerBandPerMode.h"

map<string, StationCat> Station::m_stationCategoryMap;
map<string, PowerCat>   Station::m_powerCategoryMap;
map<string, OperatorCat> Station::m_operatorCategoryMap;
map<string, StationModeCat> Station::m_stationModeCategoryMap;

Station::Station(Contest *contest)
	:
	m_instate(false),
	m_contest(contest),
   m_stationCat(eFixedStationCat),
   m_powerCat(eHighPowerCat),
   m_stationOperatorCat(eSingleOperatorCat),
   m_stationModeCat(eMixedModeCat),
   m_allLocations(nullptr),
   m_state(),
   m_province(),
   m_country("usa"),
   m_qsoPoints(0),
   m_bonusPoints(0),
   m_qsos(),
   m_showme(false),
   m_category(nullptr),
   m_operatorName(),
   m_club(),
   m_clubLower(),
   m_validPhoneQsos(0),
   m_badPhoneQsos(0),
   m_validCwQsos(0),
   m_badCwQsos(0),
   m_validDigitalQsos(0),
   m_badDigitalQsos(0),
   m_numberValidQsos(0),
   m_operators(),
   m_operatorsList(),
   m_logFileName(),
   m_categoryAbbrev(),
   m_1x1Count(0),
   m_1x1CountUnique(0),
   m_cwPoints(3),
   m_phonePoints(2),
   m_digitalPoints(3),
   m_validVhfQsos(0),
   m_ignoredModeQsos(0),
   m_workedBonusStation(false),
   m_multipliersMgr(nullptr),
   m_stationResults(nullptr),
   m_cabrilloFile(true),  // default to cabrillo file because this is most common
   m_bonusStationsPerBandPerMode(nullptr),
   m_pureDigitalScore(0),
   m_digitalMultipliers(0)

{
   if (contest != nullptr)
   {
      m_contestState = contest->GetState();
      m_contestStateAbbrev = contest->GetStateAbbrev();
      contest->GetCountyAbbrevs(m_countyAbbrevs);
      contest->CloneTokenTypes(m_qsoTokenTypes);

      m_allLocations = new AllLocations(contest->GetAllLocations());

      m_cwPoints = contest->GetCwPoints();
      m_phonePoints = contest->GetPhonePoints();
      m_digitalPoints = contest->GetDigitalPoints();
   }

   m_cwQsoCountByBand.resize(eHamBandSize);
   m_phQsoCountByBand.resize(eHamBandSize);
   m_ryQsoCountByBand.resize(eHamBandSize);
}

Station::~Station()
{
	m_qsos.clear();

   if (m_allLocations != nullptr)
   {
      delete m_allLocations;
      m_allLocations = nullptr;
   }

   if (!m_qsoTokenTypes.empty())
   {
      for (auto tt : m_qsoTokenTypes)
      {
         delete tt;
      }
      m_qsoTokenTypes.clear();
   }

   delete m_multipliersMgr;
   m_multipliersMgr = nullptr;

   if (m_bonusStationsPerBandPerMode != nullptr)
   {
	   delete m_bonusStationsPerBandPerMode;
	   m_bonusStationsPerBandPerMode = nullptr;
   }
}

void Station::WriteOneLineSummary()
{
	const char* instate = "In State";
	const char* outstate = "Out of State";

	const char* inoutstate = m_instate ? instate : outstate;

	printf("%10s : %s\n", m_callsign.c_str(), inoutstate);
}

bool Station::ReadLogFileCreateQsos(const string& logfilename)
{
	if (logfilename.empty())
	{
		printf("Error in Station::ReadLogFileCreateQsos - log file name is empty\n");
		return false;
	}

   m_logFileName = logfilename;

	TextFile file;
	bool status = file.Read(logfilename);
	if (!status)
	{
		printf("Error: failed to read log file %s\n", logfilename.c_str());
		return false;
	}

	vector<string> lines;
	file.GetLines(lines);

   status = ParseStringsCreateQsos(lines, m_qsoTokenTypes);

   bool spcstatus =SetupStateProvinceCountry();
   if (!spcstatus)
   {
      const char* text1 = m_callsign.empty() ? "callsign" : m_callsign.c_str();
      const char* text2 = m_stationLocation.empty() ? "location" : m_stationLocation.c_str();
      printf("Error: unable to determine state province country for %s with location %s\n", text1, text2);
   }

	return status;
}

// Process the lines from the log file
bool Station::ParseStringsCreateQsos(vector<string>& data, vector<QsoTokenType*>& tokenTypes)
{
	int size = (int)data.size();

   list<string> qsoLines;
	bool endoflog = false;
	int i = 0;
	for (; i < size && !endoflog; ++i)
	{
		string str = data[i];
		boost::trim(str);
      if (str.empty())
         continue;

		ParseStringCreateQso(str, endoflog, tokenTypes, qsoLines);
	}

   //		Qso *qso = new Qso(this);
   //		qso->Process(value, tokenTypes);
   m_qsos.resize(qsoLines.size());
   int count = 0;
   for (string line: qsoLines)
   {
      Qso& qso = m_qsos[count++];
      qso.SetStation(this);
      qso.SetNumber(count);
      qso.Process(line, tokenTypes);
   }

   // Setup the multipliers for this station
   bool status = SetupMultipliers();

	return status;
}

bool Station::SetupMultipliers()
{
   if (m_contest == nullptr)
      return false;

   bool status = false;
   MultipliersType multType = m_contest->GetMultipliersType();
   if (multType == eMultPerMode)
   {
	   // MultipliersMgr is only created when active
	   m_multipliersMgr = new MultipliersMgr();
	   status = m_multipliersMgr->SetupMultipliers(this, multType);
	   return status;
   }

   string location;
   if (m_instate)
   {
      // In State multipliers are counties, states, provinces and 'dx'
      if (m_contest->GetInStateMultsCounties())
      {
         m_contest->GetCountyAbbrevs(m_allMultipliers);
      }

	  if (InStateDxccMults() == false)
	  {
		  m_allMultipliers.insert("dx");
	  }

      // todo - make this data driven
//      m_allMultipliers.insert("ks");

      const AllLocations& allLocations = m_contest->GetAllLocations();
      const auto& stateAbbrevs = allLocations.GetStateAbbrevs();
      
      auto iter = stateAbbrevs.begin();
      auto iterEnd = stateAbbrevs.end();
      for (; iter != iterEnd; ++iter)
      {
         location = (*iter).first;
         StringUtils::ToLower(location);
         m_allMultipliers.insert(location);
      }

      const auto& canadaAbbrevs = allLocations.GetCanadaAbbrevs();
      iter = canadaAbbrevs.begin();
      iterEnd = canadaAbbrevs.end();
      for (; iter != iterEnd; ++iter)
      {
         location = (*iter).first;
         StringUtils::ToLower(location);
         m_allMultipliers.insert(location);
      }
   }
   else  // multipliers for out of state stations is instate counties
   {
      m_contest->GetCountyAbbrevs(m_allMultipliers);
   }

   return true;
}

bool Station::ParseStringCreateQso(string& str, bool& endoflog, vector<QsoTokenType*>& tokenTypes, list<string>& qsoLines)
{
	int pos = (int)str.find(':');
	if (pos == string::npos)
	{
      if (m_callsign.empty())
      {
         printf("Error in line %s, : not found\n", str.c_str());
      }
      else
      {
         printf("Error in line %s for station %s -> : not found\n", str.c_str(), m_callsign.c_str());
      }

		return false;
	}

	endoflog = false;
	string key = str.substr(0, pos);
	string value = str.substr(pos + 1, str.length() - pos);

	boost::trim(key);
	boost::trim(value);
   string valueLower = value;

	StringUtils::ToLower(key);
   StringUtils::ToLower(valueLower);

	int addresscount = 0;
	if (key.compare("qso") == 0)
	{
//		Qso *qso = new Qso(this);
//		qso->Process(value, tokenTypes);
      qsoLines.push_back(value);
	}
//	else if (key.compare("address") == 0)
//	{
//		++addresscount;
//	}
   else if (key.compare("name") == 0)
   {
      m_operatorName = value;
   }
   else if (key.compare("soapbox") == 0)
   {
      if (!value.empty())
      {
         m_soapbox.push_back(value);
      }
   }
   else if (key.compare("email") == 0)
   {
      if (!value.empty())
      {
         m_email = valueLower;
      }
   }
   else if (key.substr(0, 7) == "address")
   {
      if (!value.empty())
      {
         StringUtils::ReplaceCharacter(value, ',', ' ');
         if (m_address.empty())
            m_address = value;
         else
            m_address += " " + value;
      }
   }
   else if (key.compare("operators") == 0)
   {
      if (!value.empty())
      {
         StringUtils::ReplaceCharacter(value, ',', ' ');
         m_operators = value;
         StringUtils::Split(m_operatorsList, value);
      }
   }
   else if (key.compare("location") == 0)
   {
      // is the location an instate county?
      string lowerValue(value);
      StringUtils::ToLower(lowerValue);
      auto iter = m_countyAbbrevs.find(lowerValue);
      if (iter == m_countyAbbrevs.end())
      {
         // this is not an instate county
         m_stationLocation = lowerValue;
         m_instate = m_contestStateAbbrev == m_stationLocation;
      }
      else
      {
         // this is an instate county
         m_stationLocation = m_contestStateAbbrev;
         m_instate = true;
      }
   }
	else if (key.compare("arrl-section") == 0)
	{
		m_stationLocation = value;
		StringUtils::ToLower(m_stationLocation);
		m_instate = m_contestStateAbbrev == m_stationLocation;
	}
	else if (key.compare("end-of-log") == 0)
	{
		endoflog = true;
	}
   else if (key.compare("club") == 0)
   {
      if (!value.empty())
      {
         if (valueLower.compare("none") != 0)
         {
            m_club = value;
            m_clubLower = valueLower;
         }
      }
   }
	else if (key.compare("callsign") == 0)
	{
		m_callsign = value;
		m_callsignLower = value;
		StringUtils::ToLower(m_callsignLower);
	}
   else if (key.compare("category") == 0)
   {
      ProcessCategoryOperator(valueLower);
//      ProcessCategoryStation(valueLower);
      StationCat cat = ParseStationCategory(valueLower);
      if (cat != eUnknownStationCat)
         m_stationCat = cat;

      ProcessCategoryPower(valueLower);
      ProcessCategoryMode(valueLower);
   }
   else if (key.compare("category-ksqp") == 0 ||
            key.compare("category-abbrev") == 0)
   {
      m_categoryAbbrev = valueLower;
   }
   else if (key.compare("category-station") == 0)
   {
//      ProcessCategoryStation(valueLower);
      StationCat cat = ParseStationCategory(valueLower);
      if (cat != eUnknownStationCat)
         m_stationCat = cat;
   }
   else if (key.compare("category-operator") == 0)
   {
      ProcessCategoryOperator(valueLower);
   }
   else if (key.compare("category-power") == 0)
   {
      ProcessCategoryPower(valueLower);
   }
   else if (key.compare("category-mode") == 0)
   {
      ProcessCategoryMode(valueLower);
   }
   else if (key.compare("qplc-bonus_cabrillo") == 0)
   {
      StringUtils::ParseBoolean(m_cabrilloFile, value);
   }
   else if (key.compare("qplc-score") == 0 ||
	        key.compare("qplc-multipliers") == 0 ||
			key.compare("qplc-qsopoints") == 0 ||
			key.compare("qplc-bonuspoints") == 0) 
   {
   if (m_stationResults == nullptr)
	   m_stationResults = new StationResults();

       m_stationResults->ProcessKeyValue(key, value);
   }
	else if (!value.empty())
	{
		m_datamap[key] = value;
	}

	return true;
}

// Process cw, ssb, mixed mode
void Station::ProcessCategoryMode(const string& value)
{
   auto pos1 = value.find("mixed");
   if (pos1 != string::npos)
   {
      m_stationModeCat = eMixedModeCat;
      return;
   }

   pos1 = value.find("cw");
   if (pos1 != string::npos)
   {
      m_stationModeCat = eCwModeCat;
      return;
   }

   pos1 = value.find("ssb");
   if (pos1 != string::npos)
   {
      m_stationModeCat = eSsbModeCat;
      return;
   }

}

void Station::ProcessCategoryPower(const string& value)
{
   auto pos1 = value.find("low");
   auto pos2 = value.find("lp");
   if (pos1 != string::npos || pos2 != string::npos)
   {
      m_powerCat = eLowPowerCat;
      return;
   }

   pos1 = value.find("high");
   pos2 = value.find("hp");
   if (pos1 != string::npos || pos2 != string::npos)
   {
      m_powerCat = eHighPowerCat;
      return;
   }

   pos1 = value.find("qrp");
   if (pos1 != string::npos)
   {
      m_powerCat = eQrpPowerCat;
      return;
   }
}

void Station::ProcessCategoryStation(const string& value)
{
   auto pos1 = value.find("rover");
   auto pos2 = value.find("mobile");
   if (pos1 != string::npos)
   {
      m_stationCat = eRoverStationCat;
   }
   else if (pos2 != string::npos)
   {
      m_stationCat = eMobileStationCat;
   }
}

void Station::ProcessCategoryOperator(const string& value)
{
   auto pos1 = value.find("single");
   auto pos2 = value.find("multi");
   auto pos3 = value.find("checklog");
   if (pos1 != string::npos)
   {
      m_stationOperatorCat = eSingleOperatorCat;
   }
   else if (pos2 != string::npos)
   {
      m_stationOperatorCat = eMultiOperatorCat;
   }
   else if (pos3 != string::npos)
   {
      m_stationOperatorCat = eCheckLogCat;
   }
}

// Find the valid and invalid counties worked during the contest
void Station::DetermineValidAndInvalidCountiesWorked()
{
   const Location *location = nullptr;
   string county;
   for (Qso& qso : m_qsos)
   {
      if (qso.IsIgnored())
         continue;

      location = qso.GetTheirLocation();
      if (location == nullptr)
         continue;

      county = location->GetValue();
      if (county.empty() || location->HasErrors())
         continue;

      // is it a county?
      StringUtils::ToLower(county);
      auto iter = m_countyAbbrevs.find(county);
      if (iter == m_countyAbbrevs.end())
         continue;

      if (qso.ValidQso())
      {
         m_validCountiesWorked.insert(county);
      }
      else
      {
         m_invalidCountiesWorked.insert(county);
      }
   }

   // remove qso's from the invalid set that are in the valid set
   set<string> countiesToBeRemoved;
   for (string c : m_invalidCountiesWorked)
   {
      auto iter = m_validCountiesWorked.find(c);
      if (iter != m_validCountiesWorked.end())
      {
         countiesToBeRemoved.insert(c);
      }
   }

   // now remove the qso's
   for (string c : countiesToBeRemoved)
   {
      m_invalidCountiesWorked.erase(c);
   }
}

// Find the instate county count for each county that the instate station operated from
// (for instate stations, build a map of [county abbrev,qso count] )
void Station::DetermineInstateCountyCount()
{
   if (OutState())
      return;

   const Location *location = nullptr;
   string myLocation;
   for (Qso& qso : m_qsos)
   {
      location = qso.GetMyLocation();
      if (location != nullptr && !location->HasErrors())
      {
         myLocation = location->GetValue();
         if (!myLocation.empty())
         {
            auto iter = m_InstateCountyCount.find(myLocation);
            if (iter == m_InstateCountyCount.end())
            {
               m_InstateCountyCount[myLocation] = 1;
            }
            else
            {
               (*iter).second++;
            }
         }
      }
   }
}

void Station::AddInstateCounties(map<string, int>& countyCount)
{
   auto iter = m_InstateCountyCount.begin();
   auto iterEnd = m_InstateCountyCount.end();
   for (; iter != iterEnd; ++iter)
   {
      string county = (*iter).first;
      int count = (*iter).second;

      auto countyIter = countyCount.find(county);
      if (countyIter == countyCount.end())
      {
         countyCount[county] = count;
      }
      else
      {
         (*countyIter).second += count;
      }
   }
}

// Write the report log for all qso's
bool Station::WriteLogReport(const string& filename)
{
   TextFile file;
   const int SIZE = 120;
   char buffer[SIZE];

   string msg = StringUtils::CabrilloHeaderPair("Start-Of-Log", "3.0");
   file.AddLine(msg);

   msg = StringUtils::CabrilloHeaderPair("Callsign", m_callsign);
   file.AddLine(msg);

   string temp(m_stationLocation);
   StringUtils::ToUpper(temp);
   msg = StringUtils::CabrilloHeaderPair("Location", temp);
   file.AddLine(msg);

   if (m_category != nullptr)
   {
      const string& catString = m_category->m_title;
      if (catString.empty())
      {
         msg = StringUtils::CabrilloHeaderPair("Soapbox", "Category = <Missing>");
      }
      else
      {
         string value = string("Category = ") + catString;
         msg = StringUtils::CabrilloHeaderPair("Soapbox", value);
      }

      file.AddLine(msg);
   }

   msg = string(" ");
   file.AddLine(msg);

   file.AddLine("Results");

   sprintf_s(buffer, SIZE, "%5s:  %-10s %-10s %-10s", "Band", "CW", "Phone", "Digital");
   file.AddLine(buffer);

   int cwSum = 0;
   int phSum = 0;
   int rySum = 0;
   HamBand theBands[] = { e10m, e15m, e20m, e40m, e80m, e160m, e2m, e440 };
   for (int i = 0; i < 6; ++i)
   {
      HamBand aBand = theBands[i];
      string band = Qso::GetHamBandString(aBand);
      cwSum += m_cwQsoCountByBand[aBand];
      phSum += m_phQsoCountByBand[aBand];
      rySum += m_ryQsoCountByBand[aBand];
      sprintf_s(buffer, SIZE, "%5s:  %-10d %-10d %-10d", band.c_str(), m_cwQsoCountByBand[aBand],
                                                                 m_phQsoCountByBand[aBand],
                                                                 m_ryQsoCountByBand[aBand]);
      file.AddLine(buffer);
   }
   sprintf_s(buffer, SIZE, "%5s:  %-10d %-10d %-10d %-10d", "Total", cwSum, phSum, rySum, cwSum+phSum+rySum);
   file.AddLine(buffer);

   int multipliers = TotalMultipliers();
   sprintf_s(buffer, SIZE, "   Total Multipliers: %d", multipliers);
   file.AddLine(buffer);

   if (Mobile() && InState())
   {
	   int count = NumberOfMobileCountiesActivated();
	   sprintf_s(buffer, SIZE, "   Mobile Counties Activated: %d", count);
	   file.AddLine(buffer);
   }

   if (m_instate && InStateDxccMults())
   {
	   int dxcc = m_multipliersMgr->DxccMultipliers();
	   sprintf_s(buffer, SIZE, "   DXCC Multipliers Worked: %d", dxcc);
	   file.AddLine(buffer);
   }

   int bonusStationsCount = 0;
   if (m_bonusStationsPerBandPerMode != nullptr)
   {
	   bonusStationsCount = m_bonusStationsPerBandPerMode->NumberOfBonusStations();
   }
   else if (m_multipliersMgr != nullptr)
   {
	  bonusStationsCount = m_multipliersMgr->BonusStationsWorked();
   }

   if (bonusStationsCount > 0)
   {
	   sprintf_s(buffer, SIZE, "   Bonus Stations Worked: %d", bonusStationsCount);
	   file.AddLine(buffer);
   }

   sprintf_s(buffer, SIZE, "   Qso Points        : %d", m_qsoPoints);
   file.AddLine(buffer);

   if (m_bonusPoints > 0)
   {
	   sprintf_s(buffer, SIZE, "   Bonus Points      : %d", m_bonusPoints);
	   file.AddLine(buffer);
   }

   if (m_contest->HasPowerMultipliers())
   {
	   double powerMult = PowerMultiplier();
	   sprintf_s(buffer, SIZE, "   Power Multiplier  : %f", powerMult);
	   file.AddLine(buffer);
   }

//   int score = multipliers*m_qsoPoints + m_bonusPoints;
   int score = Score();
   sprintf_s(buffer, SIZE, "   Score             : %d", score);
   file.AddLine(buffer);

   msg = string(" ");
   file.AddLine(msg);

   int invalid = GetNumberOfInvalidQsos();
//   int invalid = int(m_qsos.size()) - m_numberValidQsos - m_ignoredModeQsos;
   sprintf_s(buffer, SIZE, "   Valid Q's: %d,   Invalid Q's: %d, Ignored Q's: %d  -> Total Q's: %zd", 
                           m_numberValidQsos, invalid, m_ignoredModeQsos, m_qsos.size());
   file.AddLine(buffer);

   if (m_invalidCountiesWorked.size() > 0)
   {
      // Write invalid counties worked
      msg = string(" ");
      file.AddLine(msg);
      msg = string("Counties Missed due to Busted Qso's");
      file.AddLine(msg);

      for (string s : m_invalidCountiesWorked)
      {
         StringUtils::ToUpper(s);
         file.AddLine(s);
      }
   }

   msg = string(" ");
   file.AddLine(msg);
   file.AddLine("Qso Details");

   msg = string(" ");
   file.AddLine(msg);

   list<string> qsoInfos;
   list<string> qsoErrors;
   int number = 0;
   for (Qso& qso : m_qsos)
   {
      number = qso.GetNumber();
      sprintf_s(buffer, SIZE, "[Q%04d] QSO: ", number);
      msg = string(buffer) + qso.OriginalText();
      if (qso.OriginalText().empty())
         msg = string("QSO: ") + qso.GetCreatedText(m_qsoTokenTypes);

      file.AddLine(msg);

      if (qso.GetQsoErrors(qsoErrors))
      {
         for (const string& err : qsoErrors)
         {
            sprintf_s(buffer, SIZE, "   Error: %s", err.c_str());
            file.AddLine(buffer);
         }
         file.AddLine(" ");
      }

      if (qso.GetQsoInfos(qsoInfos))
      {
         for (const string& info : qsoInfos)
         {
            sprintf_s(buffer, SIZE, "   Info : %s", info.c_str());
            file.AddLine(buffer);
         }
      }
   }

   file.Write(filename);

   return true;
}

// Setup State / Province / Country for this Station
bool Station::SetupStateProvinceCountry()
{
   if (m_stationLocation.empty())
   {
      if (m_callsign.empty())
         printf("Station ? is missing the location and callsign (Station::SetupStateProvinceCountry)\n");
      else
         printf("Station %s is missing the location (Station::SetupStateProvinceCountry)\n", m_callsign.c_str());

      return false;
   }

   const string loc = m_stationLocation;

   if (loc == "dx")
   {
      m_country = "dx";
      return true;
   }

   // Special check for Puerto Rico
   if (loc == "pr")
   {
      m_country = "dx";
      return true;
   }

   // is it the contest state?
   // TODO: what if it is a Canadian qso party?
   if (loc == m_contest->GetStateAbbrev())
   {
      m_country = "usa";
      m_state = loc;
      return true;
   }

   if (m_allLocations->IsStateAbbrev(loc))
   {
      m_country = "usa";
      m_state = loc;
      return true;
   }

   if (m_allLocations->IsProvinceAbbrev(loc))
   {
      m_country = "canada";
      m_province = loc;
      return true;
   }

   string spc = m_allLocations->GetStateOrProvinceFromSection(loc);
   if (!spc.empty())
   {
      if (m_allLocations->IsStateAbbrev(spc))
      {
         m_country = "usa";
         m_state = spc;
         return true;
      }

      if (m_allLocations->IsProvinceAbbrev(spc))
      {
         m_country = "canada";
         m_province = spc;
         return true;
      }
   }

   // Unable to determine the location from the provided location in the Cabrillo file,
   // infer the location from the Callsign
   Qso *nullQso = nullptr;
   Callsign callsign(nullQso);
   bool status = callsign.ProcessToken(m_callsign, nullQso);
   if (status)
   {
      if (callsign.IsDx())
      {
         m_country = "dx";
         const char* stationLoc = m_stationLocation.empty() ? "<Missing>" : m_stationLocation.c_str();

         printf("Information: Station %s - changing Cabrillo location from %s to %s based on callsign\n",
            m_callsign.c_str(), stationLoc, m_country.c_str());

         m_stationLocation = "dx";
         return true;
      }
   }

   return false;
}

// Note: this method does not check for duplicates, it finds the corresponding qso in the other stations log 
// and assigns the reference qso number.  It creates some errors, such as 'not found in log'.
void Station::ValidateQsos()
{
   for (Qso& qso : m_qsos)
   {
      // Only validate if the other station has not already validated the qso
      if (qso.GetRefQsoNumber() <= 0)
      {
//		  printf("Station::ValidateQsos qso number %d\n", qso.GetNumber());
         qso.Validate(m_contest);
      }
   }
}

void Station::AddMissingStation(const string& callsign)
{
   m_missingStations.insert(callsign);
}

Qso* Station::FindAvailableQso(HamBand band, 
                               QsoMode mode, 
                               const string& callsign, 
                               const string& myLocation, 
                               const string& theirLocation)
{
   for (Qso& qso : m_qsos)
   {
      if (qso.GetRefQsoNumber() <= 0)
      {
         bool match = qso.Match(band, mode, callsign, myLocation, theirLocation);

         if (match)
         {
            return &qso;
         }
      }
   }

   for (Station *station : m_childStations)
   {
      Qso *qso = station->FindAvailableQso(band, mode, callsign, myLocation, theirLocation);
      if (qso != nullptr)
      {
         return qso;
      }
   }

   return nullptr;
}

Qso* Station::GetQso(int n)
{
   if (n < 0 || n >= (int)m_qsos.size())
      return nullptr;

   Qso& qso = m_qsos[n];

   return &qso;
}

// Return the number of duplicate qso's found
int Station::CheckForDuplicateQsos()
{
   int dupeCount = 0;
   const bool cwAndDigitalAreTheSameMode = m_contest->GetCwAndDigitalAreTheSameMode();

   int qsoCount = (int)m_qsos.size();
   int number = 0;
   for (Qso& qso : m_qsos)
   {
      if (!qso.ValidQso())
         continue;

      number = qso.GetNumber();
      if (qso.GetDuplicateNumber() == -1 && number < qsoCount)
      {
         for (int i = number; i < qsoCount; ++i)
         {
            Qso *q = &m_qsos[i];
//            if (!q->ValidQso())
//               continue;

            if (qso.IsDuplicateQso(q, cwAndDigitalAreTheSameMode))
            {
               q->SetDuplicateNumber(qso.GetNumber());

               QsoError *qsoerror = nullptr;
               char buffer[80];

               qsoerror = new QsoError(eDuplicateQso);
               sprintf_s(buffer, 80, "Duplicate Qso with Qso %d", q->GetDuplicateNumber());
               qsoerror->m_error = string(buffer);
               q->AddQsoError(qsoerror);

               break;
            }
         }
      }
   }

   return dupeCount;
}

bool Station::FindMultipliers()
{
	bool status = false;
	if (m_multipliersMgr != nullptr)
	{
		status = m_multipliersMgr->FindMultipliers(m_qsos);
		return status;
	}
   for (Qso& qso : m_qsos)
   {
      if (qso.ValidQso()  && !qso.IsIgnored())
      {
		 const Location *theirLocation = qso.GetTheirLocation();
		 DxccCountry *dxcc = theirLocation->GetDxccCountry();
         string location = qso.GetTheirLocation()->GetValue();

         auto iter = m_allMultipliers.find(location);
         if (iter == m_allMultipliers.end() && dxcc == nullptr)
         {
            // is it a county?
            auto countyIter = m_countyAbbrevs.find(location);
            if (countyIter == m_countyAbbrevs.end())
            {
               // internal error, this should never happen
               // To be a valid qso, the location must be valid
               printf("*** Internal Error *** -> Station::FindMultipliers %s unknown location %s\n", m_callsign.c_str(),  location.c_str());
			   string originalText = qso.OriginalText();
			   printf("   *** %s\n", originalText.c_str());
			   printf("   *** Log File Name: %s\n", m_logFileName.c_str());
               continue;
            }
            else
            {
               location = "ks";
            }
         }

         iter = m_workedMultipliers.find(location);
         if (iter == m_workedMultipliers.end() && dxcc == nullptr)
         {
            m_workedMultipliers.insert(location);
            QsoInfo *info = new QsoInfo();
            string locUpper(location);
            StringUtils::ToUpper(locUpper);
            char buffer[80];
            sprintf_s(buffer, 80, "New Multiplier: %s", locUpper.c_str());
            info->m_msg = string(buffer);
            qso.AddQsoInfo(info);
         }
      }
   }

   return true;
}

// Ignore the qso if the station mode category does not match the qso mode
// i.e. if the station is ssb only category, ignore cw and digital qsos
void Station::FindIgnoredQsos()
{
   bool includeQso = false;
   m_ignoredModeQsos = 0;
   for (Qso& qso : m_qsos)
   {
      const string mode = qso.GetMode().GetValue();
      QsoMode qsoMode = qso.GetMode().GetMode();

      includeQso = IncludeQso(qsoMode);  // does the station mode match the qso mode?

      if (!includeQso)  // if the qso is excluded, increment the ignored count
      {
         if (qso.ValidQso())
         {
            qso.SetIgnore(true);
            QsoInfo *qi = new QsoInfo();
            qi->m_msg = "Qso Ignored because qso mode is different from station mode category";
            qso.AddQsoInfo(qi);
            m_ignoredModeQsos++;
         }
         continue;
      }
   }
}

bool Station::CountQsoPoints()
{
   int sum = 0;

   m_validPhoneQsos = 0;
   m_badPhoneQsos = 0;

   m_validCwQsos = 0;
   m_badCwQsos = 0;

   m_validDigitalQsos = 0;
   m_badDigitalQsos = 0;

   m_numberValidQsos = 0;

   int pointsScalerCache = 0;
   int pointsScaler = 0;

   // set of digital multipliers
   // used to calculate digital score
   set<string> digitalMultipliers;

   // SCQP scales instate station points... work instate station get 1 point per phone qso
   // work out of state station get 2 points per phone qso
   if (m_instate)
   {
	   pointsScalerCache = m_contest->GetInStateWorksOutOfStatePointsScaler();
   }

   for (int i = 0; i < eHamBandSize; ++i)
   {
      m_cwQsoCountByBand[i] = 0;
      m_phQsoCountByBand[i] = 0;
      m_ryQsoCountByBand[i] = 0;
   }

   bool includeQso = true;
   for (Qso& qso : m_qsos)
   {
      const string mode = qso.GetMode().GetValue();
      QsoMode qsoMode = qso.GetMode().GetMode();

	  pointsScaler = 0;
	  if (pointsScalerCache > 0)
	  {
		  // points scaling for instate stations is active
		  // is this qso working an instate or out of state station?
		  const Callsign& call = qso.GetTheirCallsign();
		  string callsign = call.GetCallsign();

		  // if it is Canada or DX then it is out of state
		  if (call.IsDx() || call.IsCanada())
			  pointsScaler = pointsScalerCache;
		  else
		  {
			  Station *s = m_contest->GetStation(callsign);
			  if (s != nullptr)
			  {
				  pointsScaler = s->InState() ? 0 : pointsScalerCache;
			  }
			  else
			  {
				  const Location* location = qso.GetTheirLocation();
				  string loc = location->GetValue();
				  auto iter = m_countyAbbrevs.find(loc);
				  if (iter == m_countyAbbrevs.end())
					  pointsScaler = pointsScalerCache;
			  }
		  }

	  }

      if (qso.ValidQso() && !qso.IsIgnored())
      {
         m_numberValidQsos++;
         if (mode == "cw")
         {
			 if (pointsScaler <= 0)
				 sum += m_cwPoints;
			 else
				 sum += m_cwPoints * pointsScaler;

            m_validCwQsos++;
            m_cwQsoCountByBand[qso.GetFreq().GetBand()]++;
         }
         else if (mode == "ph")
         {
			 if (pointsScaler <= 0)
				 sum += m_phonePoints;
			 else
				 sum += m_phonePoints * pointsScaler;

            m_validPhoneQsos++;
            m_phQsoCountByBand[qso.GetFreq().GetBand()]++;
         }
         else if (mode == "ry")
         {
			 if (pointsScaler <= 0)
				 sum += m_digitalPoints;
			 else
				 sum += m_digitalPoints * pointsScaler;

            m_validDigitalQsos++;
            m_ryQsoCountByBand[qso.GetFreq().GetBand()]++;

			string loc = qso.GetTheirLocation()->GetValue();
			digitalMultipliers.insert(loc);
         }
      }
      else
      {
         if (mode == "cw")
         {
            m_badCwQsos++;
         }
         else if (mode == "ph")
         {
            m_badPhoneQsos++;
         }
      }
   }

   m_qsoPoints = sum;

   if (m_validDigitalQsos > 0)
   {
	   int mults = (int)digitalMultipliers.size();
	   m_digitalMultipliers = mults;
	   m_pureDigitalScore = m_validDigitalQsos * mults * m_digitalPoints;
	   printf("DigitalScore: %8s : %4d points, qsos=%3d, mults=%3d\n", m_callsign.c_str(), m_pureDigitalScore, m_validDigitalQsos, mults);
   }

   return true;
}

// Return true if the station mode allows this qso to be included
// i.e. if the station mode is ssb only, return false for a cw qso
bool Station::IncludeQso(QsoMode qsoMode)
{
   bool includeQso = false;
   if (m_stationModeCat == eAnyModeCat || m_stationModeCat == eMixedModeCat)
   {
      includeQso = true;
   }
   else if (m_stationModeCat == eCwModeCat && qsoMode == eModeCw)
   {
      includeQso = true;
   }
   else if (m_stationModeCat == eSsbModeCat && qsoMode == eModePhone)
   {
      includeQso = true;
   }
   else if (m_stationModeCat == eDigitalModeCat && qsoMode == eModeDigital)
   {
      includeQso = true;
   }
   else
   {
      includeQso = false;
   }

   return includeQso;
}

void Station::GetMissingStationCallsigns(set<string>& missingStations, map<string, Station*>& m_stationMap)
{
   for (Qso& qso : m_qsos)
   {
      string callsign = qso.GetTheirCallsign().GetCallsign();
      if (!callsign.empty())
      {
         StringUtils::ToLower(callsign);
         auto iter = m_stationMap.find(callsign);
         if (iter == m_stationMap.end())
         {
            missingStations.insert(callsign);
         }
      }
   }
}

// Return the number of qso's added
int Station::AddMissingStationQsos(map<string, Station*>& missingStations)
{
   bool dump = false;
   if (dump)
   {
      printf("Station %s: AddMissingStationQsos\n", m_callsign.c_str());
   }
   int count = 0;
   string qsoString;
   for (Qso& qso : m_qsos)
   {
      string callsign = qso.GetTheirCallsign().GetCallsign();
      if (!callsign.empty())
      {
         StringUtils::ToLower(callsign);

         auto iter = missingStations.find(callsign);
         if (iter != missingStations.end())
         {
            Station* s = (*iter).second;
            Qso *newqso = new Qso();
            newqso->CopyAndFlip(&qso);

            if (dump)
            {
               qsoString = newqso->GetCreatedText(m_qsoTokenTypes);
               printf("   MissingQso: %s\n", qsoString.c_str());
            }

            s->AddQso(newqso);
            ++count;
         }
      }
   }
   return count;
}

void Station::AddQso(Qso* qso)
{
   m_qsos.push_back(*qso);
}

void Station::SetStationCallsign(const string& s)
{ 
   m_callsign = s; 
   m_callsignLower = s;

   StringUtils::ToUpper(m_callsign);
   StringUtils::ToLower(m_callsignLower);
}

bool SortQsoBySerialNumber(const Qso* q1, const Qso* q2)
{
   int s1 = q1->GetMySerialNumber().GetSerialNumber();
   int s2 = q2->GetMySerialNumber().GetSerialNumber();
   return  s1 < s2;
}

bool SortQsoByTime(const Qso* q1, const Qso* q2)
{
   int s1 = q1->GetQsoTime();
   int s2 = q2->GetQsoTime();
   return  s1 < s2;
}

// Sort the qso's by serial number
// This is used when generating missing logs
void Station::SortQsosBySerialNumber()
{
   int i = 0;
   vector<Qso*> qsoVector(m_qsos.size());

   vector<Qso> qsoCopies(m_qsos);

   for (Qso& qso : qsoCopies)
   {
      qsoVector[i++] = &qso;
      string sn = qso.GetMySerialNumber().GetValue();
      int isn = qso.GetMySerialNumber().GetSerialNumber();
      if (isn == 8888)
         isn++;
   }

   sort(qsoVector.begin(), qsoVector.end(), SortQsoBySerialNumber);

   i = 0;
   for (Qso& qso : m_qsos)
   {
      qso = *qsoVector[i++];
   }

}

// This is used when generating missing logs
void Station::SortQsosByTime()
{
   bool dump = false;
   string qsoString;
   if (dump)
   {
      printf("\nStation %s: SortQsosByTime\n", m_callsign.c_str());
      for (Qso& q : m_qsos)
      {
         qsoString = q.GetCreatedText(m_qsoTokenTypes);
         printf("   m_qso begin: %s\n", qsoString.c_str());
      }
   }

   int i = 0;
   const int numQsos = NumberOfQsos();

   // Create copies of the qsos on the heap for sorting
   vector<Qso*> qsoCopies(numQsos);
   for (i = 0; i < numQsos; ++i)
   {
      qsoCopies[i] = new Qso(m_qsos[i]);
   }

   if (dump)
   {
      for (Qso* q : qsoCopies)
      {
         qsoString = q->GetCreatedText(m_qsoTokenTypes);
         printf("   QsoCopy Before Sort: %s\n", qsoString.c_str());
      }
   }

   sort(qsoCopies.begin(), qsoCopies.end(), SortQsoByTime);

   if (dump)
   {
      for (Qso* q : qsoCopies)
      {
         qsoString = q->GetCreatedText(m_qsoTokenTypes);
         printf("   QsoCopy After Sort: %s\n", qsoString.c_str());
      }
   }

   i = 0;
   m_qsos.clear();
   for (i = 0; i < numQsos; ++i)
   {
      m_qsos.push_back(*(qsoCopies[i]));
   }

   if (dump)
   {
      for (Qso& q : m_qsos)
      {
         qsoString = q.GetCreatedText(m_qsoTokenTypes);
         printf("   m_qso final: %s\n", qsoString.c_str());
      }
   }

   // Delete the temporary copies of the qso used in sorting
   for (Qso *q : qsoCopies)
   {
      delete q;
   }
}

// Calculate bonus points for working the bonus station, once per contest
void Station::CalculateBonusPoints(const string& bonusStation, const int bonusStationPoints, int cabrilloBonusPoints)
{
   if (cabrilloBonusPoints > 0)
   {
      if (m_cabrilloFile)
      {
         m_bonusPoints += cabrilloBonusPoints;
      }
   }

   if (m_contest->GetBonusStationPointsPerBandPerMode())
   {
	   if (m_bonusStationsPerBandPerMode == nullptr)
	   {
		   m_bonusStationsPerBandPerMode = new BonusStationsPerBandPerMode();
	   }

	   int bonusPoints = m_bonusStationsPerBandPerMode->CalculateBonusPoints(bonusStation, bonusStationPoints, m_qsos);
	   m_bonusPoints += bonusPoints;
	   if (bonusPoints > 0)
		   m_workedBonusStation = true;
	   return;
   }

   for (Qso& qso : m_qsos)
   {
      auto theirCallsign = qso.GetTheirCallsign();
      string call = theirCallsign.GetCallsign();
      StringUtils::ToLower(call);
      if (call == bonusStation)
      {
         m_workedBonusStation = true;
         m_bonusPoints += bonusStationPoints;
         break;
      }
   }
}

// This method is used by 1x1 stations to update the letter in stations worked
void Station::ProcessShowMe1x1(map<string, Station*>& stationMap)
{
   char c = m_callsignLower.at(2);

   for (Qso& qso : m_qsos)
   {
      const Callsign& callsign = qso.GetTheirCallsign();
      string call = callsign.GetCallsign();
      auto iter = stationMap.find(call);
      if (iter == stationMap.end())
         continue;

      Station *s = (*iter).second;
      s->AddShowmeLetter(c);
   }
}


// Add a letter from a showme station
void Station::AddShowmeLetter(char c)
{
   auto iter = m_showmeMap.find(c);
   if (iter == m_showmeMap.end())
   {
      m_showmeMap[c] = 1;
   }
   else
   {
      int val = (*iter).second;
      (*iter).second = val + 1;
   }
}

// Once all the stations are processed, set the m_showme boolean
void Station::SetShowmeStatus()
{
   if (m_showmeMap.size() == 6)
      m_showme = true;
   else if (m_showmeMap.size() == 5)
   {
      if (m_workedBonusStation)
      {
//         printf("Station %s: needed bonus station to spell SHOWME\n", m_callsign.c_str());
         m_showme = true;
      }
   }
}

// Get the Soapbox Comments
void Station::GetSoapbox(list<string>& soapbox)
{
   soapbox.clear();
   soapbox = m_soapbox;
}

void Station::ReserveQsos(int count)
{
   if (count > (int)m_qsos.size())
   {
      m_qsos.reserve(count);
   }
}

// Convert the token to a StationCat
StationCat Station::ParseStationCategory(const string& tokenArg)
{
//   enum StationCat { eUnknownStationCat, eAnyStationCat, eFixedStationCat, eMobileStationCat, ePortableStationCat };

   string token(tokenArg);
   StringUtils::ToLower(token);

   if (m_stationCategoryMap.empty())
      SetupStaticStationCategoryMap();

   auto iter = m_stationCategoryMap.find(token);
   if (iter == m_stationCategoryMap.end())
      return eUnknownStationCat;

   StationCat cat = (*iter).second;

   return cat;
}

// Convert the token to a PowerCat
PowerCat Station::ParsePowerCategory(const string& tokenArg)
{
   string token(tokenArg);
   StringUtils::ToLower(token);

   if (m_powerCategoryMap.empty())
      SetupStaticPowerCategoryMap();

   auto iter = m_powerCategoryMap.find(token);
   if (iter == m_powerCategoryMap.end())
      return eUnknownPowerCat;

   PowerCat cat = (*iter).second;
   return cat;
}

// Convert the token to a OperatorCat
OperatorCat Station::ParseOperatorCategory(const string& tokenArg)
{
   string token(tokenArg);
   StringUtils::ToLower(token);

   if (m_operatorCategoryMap.empty())
      SetupStaticOperatorCategoryMap();

   auto iter = m_operatorCategoryMap.find(token);
   if (iter == m_operatorCategoryMap.end())
      return eUnknownOperatorCat;

   OperatorCat cat = (*iter).second;
   return cat;
}

StationModeCat Station::ParseStationModeCategory(const string& tokenArg)
{
   string token(tokenArg);
   StringUtils::ToLower(token);

   if (m_stationModeCategoryMap.empty())
      SetupStaticStationModeCategoryMap();

   auto iter = m_stationModeCategoryMap.find(token);
   if (iter == m_stationModeCategoryMap.end())
      return eUnknownStationModeCat;

   StationModeCat cat = (*iter).second;
   return cat;
}

// Setup the static station maps
void Station::SetupStaticData()
{
   SetupStaticStationCategoryMap();
   SetupStaticPowerCategoryMap();
   SetupStaticOperatorCategoryMap();
   SetupStaticStationModeCategoryMap();
}

//   enum StationCat { eUnknownStationCat, eAnyStationCat, eFixedStationCat, eMobileStationCat, ePortableStationCat };
void Station::SetupStaticStationCategoryMap()
{
   if (!m_stationCategoryMap.empty())
      return;

   m_stationCategoryMap["unknown"] = eUnknownStationCat;
   m_stationCategoryMap["any"] = eAnyStationCat;
   m_stationCategoryMap["fixed"] = eFixedStationCat;
   m_stationCategoryMap["mobile"] = eMobileStationCat;
   m_stationCategoryMap["portable"] = ePortableStationCat;
   m_stationCategoryMap["rover"] = eRoverStationCat;
   m_stationCategoryMap["roverlimited"] = eRoverLimitedStationCat;
   m_stationCategoryMap["roverunlimited"] = eRoverUnlimitedStationCat;
   m_stationCategoryMap["expedition"] = eExpeditionStationCat;
   m_stationCategoryMap["hq"] = eHQStationCat;
   m_stationCategoryMap["school"] = eSchoolStationCat;
}

//enum PowerCat { eUnknownPowerCat, eAnyPowerCat, eHighPower, eLowPower, eQrpPower };
void Station::SetupStaticPowerCategoryMap()
{
   if (!m_powerCategoryMap.empty())
      return;

   m_powerCategoryMap["unknown"] = eUnknownPowerCat;
   m_powerCategoryMap["any"]     = eAnyPowerCat;
   m_powerCategoryMap["high"]    = eHighPowerCat;
   m_powerCategoryMap["low"]     = eLowPowerCat;
   m_powerCategoryMap["qrp"]     = eQrpPowerCat;
}

// enum OperatorCat { eUnknownOperatorCat, eAnyOperatorCat, eSingleOperatorCat, eMultiOperatorCat };
void Station::SetupStaticOperatorCategoryMap()
{
   if (!m_operatorCategoryMap.empty())
      return;

   m_operatorCategoryMap["unknown"]  = eUnknownOperatorCat;
   m_operatorCategoryMap["any"]      = eAnyOperatorCat;
   m_operatorCategoryMap["single"]   = eSingleOperatorCat;
   m_operatorCategoryMap["multi"]    = eMultiOperatorCat;
   m_operatorCategoryMap["multiple"] = eMultiOperatorCat;
}

//enum StationModeCat  { eUnknownModeCat, eAnyModeCat, eCwModeCat, eSsbModeCat, eMixedModeCat };
void Station::SetupStaticStationModeCategoryMap()
{
   if (!m_stationModeCategoryMap.empty())
      return;

   m_stationModeCategoryMap["unknown"] = eUnknownStationModeCat;
   m_stationModeCategoryMap["any"]     = eAnyModeCat;
   m_stationModeCategoryMap["cw"]      = eCwModeCat;
   m_stationModeCategoryMap["ssb"]     = eSsbModeCat;
   m_stationModeCategoryMap["phone"]   = eSsbModeCat;
   m_stationModeCategoryMap["mixed"]   = eMixedModeCat;
   m_stationModeCategoryMap["digital"] = eDigitalModeCat;
}

// Return the master moqp result string for this station
string Station::GetMOQPMasterResult()
{
   string line;

   line = m_callsign + ", ";

   // home callsign
   if (m_operatorsList.empty())
   {
      line += m_callsign + ", ";
   }
   else if (m_operatorsList.size() == 1)
   {
      line += m_operators + ", ";
   }
   else
   {
      line += ", ";
   }

   // operators
   if (m_operatorsList.size() > 1)
   {
      line += m_operators + ", ";
   }
   else
   {
      line += ", ";
   }

   // categories
   line += m_category == nullptr ? ", " : m_category->m_title + ", ";

   // location
   string temp = m_stationLocation;
   StringUtils::ToUpper(temp);
   line += temp + ", ";

   // Score
   line += StringUtils::ToString(Score()) + ", ";

   // QSO's
   line += StringUtils::ToString(m_numberValidQsos) + ", ";

   // qso points
   line += StringUtils::ToString(m_qsoPoints) + ", ";

   // multipliers
   line += StringUtils::ToString(TotalMultipliers()) + ", ";

   // bonus points
   line += StringUtils::ToString(BonusPoints()) + ", ";

   // MOQP Cert
   line += ", ";

   // Sponsored Plaque
   line += ", ";

   if (m_showme)
   {
      line += "Y, ";
   }
   else
   {
      line +=  ", ";
   }

   // $3 in mail for cert
   line += ", ";

   // name
   line += m_operatorName.empty() ? ", " : m_operatorName + ", ";

   // mailing address from log
   if (!m_address.empty())
   {
//      line += string("\"") + m_address + string("\", ");
      line += m_address + ", ";
   }
   else
   {
      line += ", ";
   }

   // email address
   line += m_email.empty() ? ", " : m_email + ", ";

   // mailing address from qrz.com
   line += ", ";

   return line;
}

bool Station::AddChildStation(Station* pChild)
{
   if (m_childStations.empty())
   {
      m_childStations.push_back(pChild);
   }
   else
   {
      auto iter = std::find(m_childStations.begin(), m_childStations.end(), pChild);
      if (iter == m_childStations.end())
      {
         m_childStations.push_back(pChild);
      }
      else
      {
         printf("*** Error: Station::AddChildStation failed\n");
         return false;
      }
   }
   return true;
}

// If one station logs a callsign incorrectly, that station can get credit for the qso
// and the station that logged the qso correctly can lose credict for the qso.
// Find and fix these problems
int Station::FixQsosWithLoggingErrors()
{
   int qsoCount = 0;
   // loop through all the qso's and find those with 1 error where the qso is not found in the other stations log
   list<Qso*> qsos;
   for (Qso &q : m_qsos)
   {
      int qsoTime = q.GetQsoTime();
      if (q.ErrorCount() == 1)
      {
         QsoError *qsoError = q.GetQsoError(0);
         if (qsoError->m_errorType == eQsoNotFoundInLog)
         {
            // for each q not found in the others log, ask that station for all q's with missing stations
            Callsign otherCall = q.GetTheirCallsign();
            string call = otherCall.GetCallsign();
            StringUtils::ToLower(call);
            Station *otherStation = m_contest->GetStation(call);
            string otherStationCallsign = otherStation->StationCallsign();
            StringUtils::ToUpper(otherStationCallsign);

            vector<Qso*> otherStationQsos;
            otherStation->GetQsosWithMissingLogFiles(otherStationQsos);
            for (Qso* otherQso : otherStationQsos)
            {
               // Do the band, mode and location match?
               bool sameBand = q.GetFreq().SameHamBand(otherQso->GetFreq());
               bool sameMode = q.GetMode() == otherQso->GetMode();
               // Check that both logged locations match
               string theirLoc1 = q.GetTheirLocation()->GetValue();
               string myLoc1 = otherQso->GetMyLocation()->GetValue();
               bool sameLoc1 = theirLoc1 == myLoc1;

               string myLoc2 = q.GetMyLocation()->GetValue();
               string theirLoc2 = otherQso->GetTheirLocation()->GetValue();
               int refQsoNumber = otherQso->GetRefQsoNumber();
               bool sameLoc2 = myLoc2 == theirLoc2;
               int t = otherQso->GetQsoTime();
               int dt = qsoTime - t;
               if (sameBand && sameMode && sameLoc1 && sameLoc2 && abs(dt) < 10 && refQsoNumber == -1)
               {
                  const Callsign &call = otherQso->GetTheirCallsign();
                  string callsign = call.GetCallsign();
                  StringUtils::ToLower(callsign);

                  // do the callsigns almost match?
                  int count = StringUtils::NumberOfDifferentChars(m_callsignLower, callsign);
                  if (count == 1 || count == 2)
                  {
                     // Clear the error from 'this' qso
                     q.ClearQsoErrors();

                     // Add a callsign logging error to the 'other' qso
                     char buffer[120];
                     StringUtils::ToUpper(callsign);
                     QsoError *qsoerror = new QsoError(eQsoLoggedWrongCallsign);
                     sprintf_s(buffer, 120, "Wrong Callsign: %s Logged %s but worked %s [qso %d]", 
                                           otherStationCallsign.c_str(), callsign.c_str(), m_callsign.c_str(), q.GetNumber());
                     qsoerror->m_error = string(buffer);
                     otherQso->AddQsoError(qsoerror);
                     ++qsoCount;

                     break;  // all done for this qso
                  }
                  else if (count == 3000)
//                  else if (count == 3)
                  {
                     int t = otherQso->GetQsoTime();
                     int dt = qsoTime - t;
                     if (abs(dt) <= 10)
                     {
                        printf("Info: Station %-7s looking in %-7s log for %-7s but %-7s differs by 3 characters, dt=%d min\n",
                           m_callsign.c_str(),
                           otherStationCallsign.c_str(), 
                           m_callsignLower.c_str(), 
                           callsign.c_str(), dt);
                     }
                  }
               }
            }
         }
      }
   }

   return qsoCount;
}

// Return the qso's made with stations that are missing log files
void Station::GetQsosWithMissingLogFiles(vector<Qso*>& qsos)
{
   qsos.clear();

   for (Qso& q : m_qsos)
   {
      if (q.OtherStationMissing())
      {
         qsos.push_back(&q);
      }
   }
}

// Count the number of valid qso's qith 1x1 stations
void Station::Calculate1x1Count()
{
   m_1x1Count = 0;
   m_1x1CountUnique = 0;
   set<string> unique1x1Calls;
   for (Qso& qso : m_qsos)
   {
      if (qso.ValidQso())
      {
         const string& theirCall = qso.GetTheirCallsign().GetCallsign();
         if (theirCall.length() == 3)
         {
            ++m_1x1Count;
            string theirCallsignLower(theirCall);
            StringUtils::ToLower(theirCallsignLower);
            auto iter = unique1x1Calls.find(theirCallsignLower);
            if (iter == unique1x1Calls.end())
            {
               unique1x1Calls.insert(theirCallsignLower);
            }
         }
      }
   }

   m_1x1CountUnique = (int)unique1x1Calls.size();
}

bool Station::IsRoverStationCat(StationCat stationCat)
{
   if (stationCat == eRoverLimitedStationCat ||
      stationCat == eRoverStationCat ||
      stationCat == eRoverUnlimitedStationCat)
   {
      return true;
   }

   return false;
}

// Count the number of valid VHF Qso's
void Station::CountValidVhfQsos()
{
   int count = m_cwQsoCountByBand[e6m] + m_cwQsoCountByBand[e2m] + m_cwQsoCountByBand[e440];
   count = count + m_phQsoCountByBand[e6m] + m_phQsoCountByBand[e2m] + m_phQsoCountByBand[e440];
   count = count + m_ryQsoCountByBand[e6m] + m_ryQsoCountByBand[e2m] + m_ryQsoCountByBand[e440];

   m_validVhfQsos = count;
}

// InState stations get multipliers for dx contacts based on ARRL DXCC countries
bool Station::InStateDxccMults()
{
	return m_contest->InStateDxccMults();
}

DxccCountryManager *Station::GetDxccCountryManager()
{
	return m_contest->GetDxccCountryManager();
}

int Station::TotalMultipliers() const
{
	int mobileCounties = 0;
	if (Mobile() && InState())
	{
		if (m_contest->GetContestConfig()->GetMobileCountiesActivatedAreMultipliers())
		{
			mobileCounties = NumberOfMobileCountiesActivated();
		}
	}

	if (m_multipliersMgr != nullptr)
	{
		return m_multipliersMgr->MultiplierPoints() + mobileCounties;
	}

	return (int)m_workedMultipliers.size() + mobileCounties;
}

int Station::Score() const 
   {
	if (m_contest->HasPowerMultipliers())
	{
		double powerMultiplier = PowerMultiplier();
		double scorex = TotalMultipliers() * QsoPoints() * powerMultiplier;
		int score = (int)scorex + BonusPoints();
		return score;
	}

	return TotalMultipliers() * QsoPoints() + BonusPoints(); 
   }


// Defaults to 1.0 if the contest does not use power multipliers
double Station::PowerMultiplier() const
{
	double powerMult = 1.0;
	if (m_contest->HasPowerMultipliers())
	{
		if (m_powerCat == eQrpPowerCat)
		{
			powerMult = m_contest->GetPowerMultiplierQRP();
		}
		else if (m_powerCat == eLowPowerCat)
		{
			powerMult = m_contest->GetPowerMultiplierLow();
		}
		else if (m_powerCat == eHighPowerCat)
		{
			powerMult = m_contest->GetPowerMultiplierHigh();
		}
	}

	return powerMult;
}

// Determine the counties activated by mobile stations
void Station::DetermineMobileCountiesActivated()
{
	if (!Mobile())
		return;

	if (!InState())
		return;

	for (Qso& qso : m_qsos)
	{
		if (qso.ValidQso())
		{
			const Location* loc = qso.GetMyLocation();
			string county = loc->GetValue();
			StringUtils::ToLower(county);
			auto iter = m_mobileCountiesActivated.find(county);
			if (iter == m_mobileCountiesActivated.end())
			{
				m_mobileCountiesActivated.insert(county);
			}
		}
	}

}

int Station::NumberOfMobileCountiesActivated() const
{
	size_t x = m_mobileCountiesActivated.size();

	return (int)x;
}

ContestConfig *Station::GetContestConfig() const
{
	return m_contest->GetContestConfig();
}

double Station::GetInvalidQsoPercentage() const
{
	double value = 0.0;

	int totalQs = GetNumberOfTotalQsos();

	if (totalQs == 0)
		return 0.0;

	int numInvalid = GetNumberOfInvalidQsos();

	value = double(numInvalid) / double(totalQs) * 100.0;

	return value;
}

// Return the data associated with dataName as a string
string Station::GetValue(const string& dataName)
{
	string key(dataName);
	StringUtils::ToLower(key);

	if (key == "callsign")
	{
		return m_callsign;
	}
	else if (key == "state")
	{
		if (!m_state.empty())
			return m_state;
		else if (!m_province.empty())
			return m_province;
		
		return "DX";
	}
	else if (key == "score")
	{
		return StringUtils::ToString(Score());
	}
	else if (key == "totalqsos")
	{
		return StringUtils::ToString(GetNumberOfValidQsos());
	}
	else if (key == "cwqsos")
	{
		return StringUtils::ToString(m_validCwQsos);
	}
	else if (key == "phqsos")
	{
		return StringUtils::ToString(m_validPhoneQsos);
	}
	else if (key == "digitalqsos")
	{
		return StringUtils::ToString(m_validDigitalQsos);
	}
	else if (key == "totalbonus")
	{
		return StringUtils::ToString(m_bonusPoints);
	}
	else if (key == "multipliers")
	{
		return StringUtils::ToString(TotalMultipliers());
	}
	else if (key == "totalcounties")
	{
		return StringUtils::ToString(GetValidCountiesWorked());
	}
	else if (key == "operatorclass")
	{
		return "single";
	}


	return string("");

}

