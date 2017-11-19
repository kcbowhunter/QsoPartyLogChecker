
#include "stdafx.h"
#include "Date.h"
#include "Qso.h"

Date::Date(Qso *qso)
:
QsoItem(qso),
m_minutes(0)
{
}

Date::~Date()
{
}

// Copy the data for the qso item
void Date::Copy(const QsoItem* source)
{
   if (source == nullptr)
      return;

   QsoItem::Copy(source);

   const Date* date = dynamic_cast<const Date*>(source);
   if (date != nullptr)
   {
      m_minutes = date->m_minutes;
   }
}

void Date::Copy(const QsoItem& source)
{
   QsoItem::Copy(source);

   const Date* date = dynamic_cast<const Date*>(&source);
   if (date != nullptr)
   {
      m_minutes = date->m_minutes;
   }
}

// date is "yyyy-mm-dd"
bool Date::ProcessToken(const string& token, Qso* qso)
{
   QsoItem::ProcessToken(token, qso);

   string s = token;

   if (s.length() != 10)
   {
      const char* msg = s.empty() ? "<empty>" : s.c_str();
      printf("Error: Date::ProcessToken %s\n", msg);
      return false;
   }

   string y = s.substr(0, 4);
   string m = s.substr(5, 2);
   string d = s.substr(8, 2);

   int year = atoi(y.c_str()) - 2000;
   int mon  = atoi(m.c_str());
   int day  = atoi(d.c_str());

   // calc the number of hours for this date
   int hour = (year * 366 + mon * 31 + day) * 24;

   // convert hours to minutes
   m_minutes = hour * 60;

   return true;
}
