
#pragma once

enum StationCat;
enum PowerCat;
enum OperatorCat;
enum StationModeCat;
enum TxCat;

class Station;
class CategoryMgr;

// Boolean with 3 states: True, False, Unspecified
enum TernaryBool { eUnspecifiedBool, eTrueBool, eFalseBool };

struct Category
{
public:
   Category(CategoryMgr *catMgr);
   virtual ~Category();

   // Read the list of strings and populate this category object
   bool ReadData(const list<string>& data);

   // Assign a key/value pair to a member ofthis class
   bool AssignData(const string& key, const string& value);

   // Return true if this station matches this category and add to the list of stations
   bool Match(Station* s);

   // Calculate the SpecCount value (number of properties different from 'any'
   void CalcSpecCount();

   CategoryMgr *m_categoryMgr;

   string m_title;

   string m_country;

   // Some categories apply to both instate and out of state stations
   TernaryBool m_instate;

   // Bonus Station
   TernaryBool m_bonusStation;

   // Check Log
   TernaryBool m_checkLog;

   // Category abbreviation
   string m_catabbrev;

   // This category only applies if it is the only option
   bool m_secondaryCategory;

   // fixed, mobile, portable
   StationCat m_stationCat;

   // qrp, low, high
   PowerCat m_powerCat;

   // single op, multi op
   OperatorCat m_stationOperatorCat;

   // cw, ssb or mixed mode
   StationModeCat m_stationModeCat;

   // Transmitter Category: single or multiple (any)
   TxCat m_txCat;

   // List of stations in this category
   list<Station*> m_stations;

   // Number of specifications
   // (Properties that are different than 'any'
   int m_specCount;
};

