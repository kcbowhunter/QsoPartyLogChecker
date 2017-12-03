
#pragma once

struct Category;
class Station;

class CategoryMgr
{
public:
   CategoryMgr();
   virtual ~CategoryMgr();

   void DetermineStationCategories(vector<Station*>& stations, bool useCategoryAbbrevs, bool assignCategoryToStation, bool matchFirstCategory);

   // Get a copy of the list of categories
   void GetCategories(list<Category*>& categories);

   // Create the category objects
   bool CreateCategories(const list<list<string>>& categoryData);

   bool GetMobileRoverEquivalent() const { return m_mobileRoverEquiv; }

private:
   list<Category*> m_categories;

   void CreateMOQPCategories();

   // Setup the m_mobileRoverEquiv flag
   // When all the categories have only rover or only mobile, then rover and mobile
   // are considered equivalent in user Cabrillo files
   void SetupMobileRoverEquivalentFlag();

   // Return the number of categories the station matched
   int DetermineStationCategory(Station *s, bool useCategoryAbbrevs, bool assignCategoryToStation, bool matchFirstCategory);

   Category* FindCategoryByAbbrev(const string& categoryAbbrev);

   Category* m_checkLog;  // checklog category

   // Setup the m_mobileRoverEquiv flag
   // When all the categories have only rover or only mobile, then rover and mobile
   // are considered equivalent in user Cabrillo files
   int m_roverCount;  // number of categories with Rover StationCat
   int m_mobileCount; // number of categories with Mobile StationCat
   bool m_mobileRoverEquiv;  // true => Rover and Mobile are Equivalent
};