
#include "stdafx.h"
#include "Category.h"
#include "Station.h"
#include "StringUtils.h"
#include "CategoryMgr.h"

Category::Category(CategoryMgr *catMgr)
   :
   m_country("usa"),
   m_stationCat(eFixedStationCat),
   m_powerCat(eAnyPowerCat),
   m_stationOperatorCat(eAnyOperatorCat),
   m_stationModeCat(eAnyModeCat),
   m_txCat(eUnknownTxCat),
   m_instate(true),
   m_title(),
   m_secondaryCategory(false),
   m_catabbrev(""),
   m_categoryMgr(catMgr),
   m_specCount(0)
{
}

Category::~Category()
{
   // The Category does not own the stations so it does not delete them
   m_stations.clear();
}

// Return true if this station matches this category and add to the list of stations
bool Category::Match(Station* s)
{
   string country = s->Country();
   StringUtils::ToLower(country);

   const bool mobileRoverEquiv = m_categoryMgr == nullptr ? false : m_categoryMgr->GetMobileRoverEquivalent();

   if (m_country == "any")  // VTQP location categories are VT and non-VT
   {
	   ;  // do nothing
   }
   else if (m_country == "usacan")  // is the station in the USA or Canada?
   {
      if (country != "usa" && country != "canada")
      {
         return false;
      }
   }
   else if (m_country != country)
      return false;

   if (m_instate != s->InState())
      return false;

   // Fixed, Mobile, Portable Station
   if (m_stationCat != eAnyStationCat)
   {
      StationCat stationStationCat = s->GetStationCat();
      if (m_stationCat != stationStationCat)
      {
         if (!mobileRoverEquiv)
         {
            return false;
         }
         else if (m_stationCat == eMobileStationCat)
         {
            if (!Station::IsRoverStationCat(stationStationCat))
            {
               return false;
            }
         }
         else if (Station::IsRoverStationCat(m_stationCat))
         {
            if (stationStationCat != eMobileStationCat)
            {
               return false;
            }
         }
         else
         {
            return false;
         }
      }
   }

   // High, Low, Qrp Power
   if (m_powerCat != eAnyPowerCat)
   {
      if (m_powerCat != s->GetPowerCat())
         return false;
   }

   // SingleOp, MultiOp
   if (m_stationOperatorCat != eAnyOperatorCat)
   {
      if (m_stationOperatorCat != s->GetOperatorCat())
         return false;
   }

   if (m_stationModeCat != eAnyModeCat)
   {
      if (m_stationModeCat != s->GetStationModeCat())
         return false;
   }

   // If all the criteria are met, add the station to the category
//   m_stations.push_back(s);

   return true;
}

// Read the list of strings and populate this category object
bool Category::ReadData(const list<string>& data)
{
   string key;
   string value;
   string keyLower;

   int errorCount = 0;
   string errorMsg("Categories");
   for (const string& str : data)
   {
      if (StringUtils::GetKeyValuePair(str, key, value, errorMsg))
      {
         bool status = AssignData(key, value);
         if (!status)
         {
            ++errorCount;
         }
      }
      else
      {
         ++errorCount;
      }
   }

   CalcSpecCount();

   return errorCount == 0;
}

// Assign a key/value pair to a member ofthis class
bool Category::AssignData(const string& keyArg, const string& value)
{
   bool status = true;
   string key(keyArg);
   StringUtils::ToLower(key);

   string valueLower(value);
   StringUtils::ToLower(valueLower);

   const char* titleMsg = m_title.empty() ? "<Missing>" : m_title.c_str();
   if (key == "title")
      m_title = value;
   else if (key == "country")
   {
      m_country = value;
      StringUtils::ToLower(m_country);
      if (m_country == "usa") 
      {
         ;
      }
      else if (m_country == "canada" || m_country == "dx" || m_country == "usacan" || m_country == "any")
      {
         m_instate = false;
      }
      else
      {
         printf("Error in %s Category Data\n   Unknown Value=%s for key %s\n", titleMsg, value.c_str(), key.c_str());
         printf("   Expected country=usa or canada or dx\n\n");
         return false;
      }
   }
   else if (key == "instate")
   {
      status = StringUtils::ParseBoolean(m_instate, value);
      if (!status)
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category Data\n   Instate = %s not recognized\n\n", titleMsg, msg);
         return false;
      }
   }
   else if (key == "power")
   {
      m_powerCat = Station::ParsePowerCategory(valueLower);
      if (m_powerCat == eUnknownPowerCat)
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category\n   Unknown power category: %s\n\n", titleMsg, msg);
         return false;
      }
   }
   else if (key == "station")
   {
      m_stationCat = Station::ParseStationCategory(valueLower);
      if (m_stationCat == eUnknownStationCat)
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category\n   Unknown Station Category: %s\n\n", titleMsg, msg);
         return false;
      }
   }
   else if (key == "operator")
   {
      m_stationOperatorCat = Station::ParseOperatorCategory(valueLower);
      if (m_stationOperatorCat == eUnknownOperatorCat)
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category\n  Unknown Operator category: %s\n\n", titleMsg, msg);
         return false;
      }
   }
   else if (key == "mode")
   {
      m_stationModeCat = Station::ParseStationModeCategory(valueLower);
      if (m_stationModeCat == eUnknownStationModeCat)
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category\n   Unknown Station Mode Category: %s\n\n", titleMsg, msg);
         return false;
      }
   }
   else if (key == "secondarycategory")
   {
      status = StringUtils::ParseBoolean(m_secondaryCategory, value);
      if (!status)
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category\n   Unknown SecondaryCategory: %s\n\n", titleMsg, msg);
         return false;
      }
   }
   else if (key == "abbrev")
   {
      m_catabbrev = valueLower;
   }
   else if (key == "tx") // transmitter category
   {
      if (valueLower == "single")
      {
         m_txCat = eSingleTxCat;
      }
      else if (valueLower == "any" || valueLower == "multiple" || valueLower == "multi")
      {
         m_txCat = eAnyTxCat;
      }
      else
      {
         const char* msg = value.empty() ? "<Missing>" : value.c_str();
         printf("Error in %s Category\n   Unknown Transmitter Category: %s\n\n", titleMsg, msg);
         return false;
      }
   }
   else
   {
      const char* msg = value.empty() ? "<Missing>" : value.c_str();
      printf("Error in %s Category\n   Unknown key value pair: %s = %s\n\n", titleMsg, key.c_str(), msg);
      return false;
   }

   return status;
}

// Calculate the SpecCount value (number of properties different from 'any'
void Category::CalcSpecCount()
{
   m_specCount = 0;
   if (m_stationCat != eAnyStationCat)
   {
      m_specCount++;
   }

   if (m_powerCat != eAnyPowerCat)
   {
      m_specCount++;
   }

   if (m_stationOperatorCat != eAnyOperatorCat)
   {
      m_specCount++;
   }

   if (m_stationModeCat != eAnyModeCat)
   {
      m_specCount++;
   }

   if (m_txCat != eAnyTxCat)
   {
      m_specCount++;
   }

//   printf("Category: SpecCount = %2d for %s\n", m_specCount, m_title.c_str());
}

