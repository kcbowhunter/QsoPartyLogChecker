
#include "stdafx.h"

#include "CategoryMgr.h"
#include "Category.h"
#include "Station.h"

CategoryMgr::CategoryMgr()
   :
   m_mobileCount(0),
   m_roverCount(0),
   m_mobileRoverEquiv(false),
   m_checkLog(nullptr)
{
//   CreateMOQPCategories();
}

CategoryMgr::~CategoryMgr()
{
   for (Category* cat : m_categories)
   {
      delete cat;
   }
   m_categories.clear();
}

// Create the category objects
bool CategoryMgr::CreateCategories(const list<list<string>>& categoryData)
{
   bool status = false;
   int errorCount = 0;
   for (const list<string>& category : categoryData)
   {
      Category *cat = new Category(this);
      status = cat->ReadData(category);
      if (status)
      {
		  m_categories.push_back(cat);
      }
	  else
	  {
		  ++errorCount;
		  delete cat;  // the category was not created correctly
	  }
   }

   // Add the checklog category
   Category *cat    = new Category(this);
   cat->m_title     = string("CheckLog");
   cat->m_catabbrev = string("CheckLog");
   cat->m_stationOperatorCat = eCheckLogCat;
   m_categories.push_back(cat);
   m_checkLog = cat;

   SetupMobileRoverEquivalentFlag();

   return errorCount == 0;
}

void CategoryMgr::CreateMOQPCategories()
{
   auto* canada = new Category(this);
   canada->m_country = "canada";
   canada->m_title = "Canada";
   canada->m_instate = eFalseBool;
   m_categories.push_back(canada);

   auto* dx = new Category(this);
   dx->m_country = "dx";
   dx->m_title = "DX";
   dx->m_instate = eFalseBool;
   m_categories.push_back(dx);

   auto *mobile_multiop_low = new Category(this);
   mobile_multiop_low->m_title = "Mobile Multi-Op Low Power";  // MOMOLP
   mobile_multiop_low->m_instate = eTrueBool;
   mobile_multiop_low->m_powerCat = eLowPowerCat;
   mobile_multiop_low->m_stationCat = eMobileStationCat;
   mobile_multiop_low->m_stationOperatorCat = eMultiOperatorCat;
   m_categories.push_back(mobile_multiop_low);

   auto *mobile_singleop_low = new Category(this);
   mobile_singleop_low->m_title = "Mobile Single-Op Low Power";  // MOSOLP
   mobile_singleop_low->m_instate = eTrueBool;
   mobile_singleop_low->m_powerCat = eLowPowerCat;
   mobile_singleop_low->m_stationCat = eMobileStationCat;
   mobile_singleop_low->m_stationOperatorCat = eSingleOperatorCat;
   m_categories.push_back(mobile_singleop_low);

   auto *mobile_singleop_low_cw = new Category(this);
   mobile_singleop_low_cw->m_title = "Mobile Single-Op Low Power CW";  // MOSOLPCW
   mobile_singleop_low_cw->m_instate = eTrueBool;
   mobile_singleop_low_cw->m_powerCat = eLowPowerCat;
   mobile_singleop_low_cw->m_stationCat = eMobileStationCat;
   mobile_singleop_low_cw->m_stationOperatorCat = eSingleOperatorCat;
   mobile_singleop_low_cw->m_stationModeCat = eCwModeCat;
   m_categories.push_back(mobile_singleop_low_cw);

   auto *mobile_singleop_low_ssb = new Category(this);
   mobile_singleop_low_ssb->m_title = "Mobile Single-Op Low Power SSB";  // MOSOLPSSB
   mobile_singleop_low_ssb->m_instate = eTrueBool;
   mobile_singleop_low_ssb->m_powerCat = eLowPowerCat;
   mobile_singleop_low_ssb->m_stationCat = eMobileStationCat;
   mobile_singleop_low_ssb->m_stationOperatorCat = eSingleOperatorCat;
   mobile_singleop_low_ssb->m_stationModeCat = eSsbModeCat;
   m_categories.push_back(mobile_singleop_low_ssb);

   auto *mobile_unlimited = new Category(this);
   mobile_unlimited->m_title = "Mobile Unlimited";  // 
   mobile_unlimited->m_instate = eTrueBool;
   mobile_unlimited->m_powerCat = eHighPowerCat;
   mobile_unlimited->m_stationCat = eMobileStationCat;
   mobile_unlimited->m_stationOperatorCat = eAnyOperatorCat;
   m_categories.push_back(mobile_unlimited);

   auto *mo_fixed_singleop_low = new Category(this);
   mo_fixed_singleop_low->m_title = "MO Fixed SingleOp Low Power";
   mo_fixed_singleop_low->m_instate = eTrueBool;
   mo_fixed_singleop_low->m_powerCat = eLowPowerCat;
   mo_fixed_singleop_low->m_stationCat = eFixedStationCat;
   mo_fixed_singleop_low->m_stationOperatorCat = eSingleOperatorCat;
   m_categories.push_back(mo_fixed_singleop_low);

   auto *mo_fixed_singleop_qrp = new Category(this);
   mo_fixed_singleop_qrp->m_title = "MO Fixed SingleOp QRP Power";
   mo_fixed_singleop_qrp->m_instate = eTrueBool;
   mo_fixed_singleop_qrp->m_powerCat = eQrpPowerCat;
   mo_fixed_singleop_qrp->m_stationCat = eFixedStationCat;
   mo_fixed_singleop_qrp->m_stationOperatorCat = eSingleOperatorCat;
   m_categories.push_back(mo_fixed_singleop_qrp);

   auto *outstate_singleop = new Category(this);
   outstate_singleop->m_title = "Non Missouri Single Op";
   outstate_singleop->m_instate = eFalseBool;
   outstate_singleop->m_stationOperatorCat = eSingleOperatorCat;
   m_categories.push_back(outstate_singleop);

   auto *outstate_multiop = new Category(this);
   outstate_multiop->m_title = "Non Missouri Multi Op";
   outstate_multiop->m_instate = eFalseBool;
   outstate_multiop->m_stationOperatorCat = eMultiOperatorCat;
   m_categories.push_back(outstate_multiop);

   auto *mo_fixed_multiop = new Category(this);
   mo_fixed_multiop->m_title = "MO Fixed MultiOp";
   mo_fixed_multiop->m_instate = eFalseBool;
   mo_fixed_multiop->m_powerCat = eAnyPowerCat;
   mo_fixed_multiop->m_stationCat = eFixedStationCat;
   mo_fixed_multiop->m_stationOperatorCat = eAnyOperatorCat;
   mo_fixed_multiop->m_secondaryCategory = true;
   m_categories.push_back(mo_fixed_multiop);
}

void CategoryMgr::DetermineStationCategories(vector<Station*>& stations, bool useCategoryAbbrevs, bool assignCategoryToStation)
{
   int num = 0;
   for (Station *s : stations)
   {
      num = DetermineStationCategory(s, useCategoryAbbrevs, assignCategoryToStation);
   }

   bool display = false;
   if (display)
   {
      for (Category* cat : m_categories)
      {
         printf("Category: %s\n", cat->m_title.c_str());

         for (Station* s : cat->m_stations)
         {
            printf("   %15s : Score %d \n", s->StationCallsign().c_str(), s->Score());
         }
         printf("\n");
      }
   }

   // Count the number of stations with no category
   int count = 0;
   for (Station *s : stations)
   {
      if (!s->HasCategory())
      {
         if (++count < 10)
         {
            printf("%d) Station %s has no category\n", count, s->StationCallsign().c_str());
         }
      }
   }

   if (count > 0)
   {
      printf("There are %d stations out of %zd with no category\n", count, stations.size());
   }
}

// Return the number of categories the station matched
int CategoryMgr::DetermineStationCategory(Station *s, bool useCategoryAbbrevs, bool assignCategoryToStation)
{
	string callsign = s->StationCallsign();
	bool dump = true;
	if (dump)
	{
		printf("\nCategoryMgr::DetermineStationCategory Enter -> Station %s\n", callsign.c_str());
	}

   // Is it a checklog?
   if (s->CheckLog())
   {
      s->SetCategory(m_checkLog);
      m_checkLog->m_stations.push_back(s);
      return 1;
   }

   // Is there a category abbreviation provided?
   const string categoryAbbrev = s->GetCategoryAbbrev();
   if (!categoryAbbrev.empty() && useCategoryAbbrevs)
   {
      Category *cat = FindCategoryByAbbrev(categoryAbbrev);
      if (cat == nullptr)
      {
         string callsign = s->StationCallsign();
         printf("Error: CategoryMgr -> unable to find category for abbreviation %s\n", categoryAbbrev.c_str());
         printf("   Station %s will be considered a checklog\n", callsign.c_str());
		 if (assignCategoryToStation)
            s->SetCategory(m_checkLog);

         m_checkLog->m_stations.push_back(s);
      }
      else
      {
	     if (assignCategoryToStation)
	        s->SetCategory(cat);

         cat->m_stations.push_back(s);

		 if (dump)
			 printf("\tStation %s has category abbreviation %s\n", callsign.c_str(), categoryAbbrev.c_str());
         return 1;
      }
   }

   list<Category*> matchingCategories;
   for (Category *cat : m_categories)
   {
      if (cat->Match(s))
      {
		  const string& title = cat->m_title;
		  if (dump)
     		  printf("\tStation %s matches category %s\n", callsign.c_str(), title.c_str());

		  if (assignCategoryToStation)
             s->SetCategory(cat);

         cat->m_stations.push_back(s);
         matchingCategories.push_back(cat);
      }
   }

   if (matchingCategories.size() > 1)
   {
      list<Category*> highCat;
      int highSpecCount = 0;
      Category *highSpecCountCat = nullptr;

	  if (dump)
         printf("CategoryMgr Error - Station %s matches multiple categories\n", s->StationCallsign().c_str());
      for (Category* cat : matchingCategories)
      {
		  if (dump)
             printf("   Category: SpecCount=%d Title=%s\n", cat->m_specCount, cat->m_title.c_str());
         if (cat->m_specCount > highSpecCount)
         {
            highSpecCountCat = cat;
            highSpecCount = cat->m_specCount;
            highCat.clear();
            highCat.push_back(cat);
         }
         else if (cat->m_specCount == highSpecCount)
         {
            highCat.push_back(cat);
         }

         cat->m_stations.remove(s);
      }

      if (highCat.size() == 1)
      {
         s->SetCategory(highSpecCountCat);
         highSpecCountCat->m_stations.push_back(s);
      }
      else
      {
         printf("Error: Station %s matches multiple categories\n", callsign.c_str());
         for (auto *cat : highCat)
         {
            printf("   SpecCount: %3d Category: %s\n", cat->m_specCount, cat->m_title.c_str());
         }
         s->SetCategory(m_checkLog);
         m_checkLog->m_stations.push_back(s);
         printf("   Converting Station %s to a checklog\n", callsign.c_str());
      }

//      for (string s : matches)
//      {
//         printf("   Category: SpecCount=%d %s\n", s.c_str());
//      }
   }

   return (int)matchingCategories.size();
}

// Get a copy of the list of categories
void CategoryMgr::GetCategories(list<Category*>& categories)
{
   categories = m_categories;
}

Category* CategoryMgr::FindCategoryByAbbrev(const string& categoryAbbrev)
{
   for (Category* cat2 : m_categories)
   {
      if (cat2->m_catabbrev == categoryAbbrev)
      {
         return cat2;
      }
   }

   return nullptr;
}

// Setup the m_mobileRoverEquiv flag
// When all the categories have only rover or only mobile, then rover and mobile
// are considered equivalent in user Cabrillo files
void CategoryMgr::SetupMobileRoverEquivalentFlag()
{
   m_mobileCount = 0;
   m_roverCount = 0;
   for (Category *cat : m_categories)
   {
      StationCat stationCat = cat->m_stationCat;
      if (stationCat == eMobileStationCat)
      {
         m_mobileCount++;
      }
      else if (Station::IsRoverStationCat(stationCat))
      {
         m_roverCount++;
      }
   }

   if (m_mobileCount == 0 || m_roverCount == 0)
   {
      m_mobileRoverEquiv = true;
   }
   else
   {
      m_mobileRoverEquiv = false;
   }

   bool dump = false;
   if (dump)
   {
      printf("CategoryMgr::SetupMobileRoverEquivalentFlag\n");
      printf("   Mobile Category Count = %d\n", m_mobileCount);
      printf("   Rover  Category Count = %d\n", m_roverCount);

      const char* pTrueFalse = m_mobileRoverEquiv ? "True" : "False";
      printf("   Mobile and Rover are Equivalent = %s\n", pTrueFalse);
   }
}
