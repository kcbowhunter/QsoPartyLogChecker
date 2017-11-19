
#include "stdafx.h"
#include "QsoTime.h"
#include "Qso.h"

QsoTime::QsoTime(Qso* qso)
   :
   QsoItem(qso),
   m_minutes(0)
   {
   }

QsoTime::~QsoTime()
   {
   }

bool QsoTime::ProcessToken(const string& token, Qso* qso)
{
   QsoItem::ProcessToken(token, qso);

   // the time is a string "hhmm"
   // parse the string and calculate the number of minutes as hh*60 + mm
   string t = token;
   if (t.length() != 4)
   {
      const char* pmsg = t.empty() ? "<empty>" : t.c_str();
      printf("Error: QsoTime - bad string %s\n", pmsg);
      return false;
   }

   string h = t.substr(0, 2);
   int hours = atoi(h.c_str());

   string m = t.substr(2, 2);
   int min = atoi(m.c_str());

   m_minutes = hours * 60 + min;

   return true;
}

// Copy the data for the qso item
void QsoTime::Copy(const QsoItem* source)
{
   if (source == nullptr)
      return;

   QsoItem::Copy(source);

   const QsoTime* time = dynamic_cast<const QsoTime*>(source);
   if (time != nullptr)
   {
      m_minutes = time->m_minutes;
   }
}

void QsoTime::Copy(const QsoItem& source)
{
   QsoItem::Copy(source);

   const QsoTime* time = dynamic_cast<const QsoTime*>(&source);
   if (time != nullptr)
   {
      m_minutes = time->m_minutes;
   }
}

