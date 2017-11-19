
#include "stdafx.h"
#include "Callsign.h"
#include "StringUtils.h"
#include "Qso.h"

Callsign::Callsign(Qso *qso)
   :
   QsoItem(qso),
   m_callsign(),
   m_prefix(),
   m_suffix(),
   m_isCanada(false),
   m_isUSA(false)
   {
   }

Callsign::Callsign(const Callsign& c)
   :
   QsoItem(c),
   m_callsign(c.m_callsign),
   m_prefix(c.m_prefix),
   m_suffix(c.m_suffix),
   m_isCanada(c.m_isCanada),
   m_isUSA(c.m_isUSA)
   {
   }

Callsign::~Callsign()
{
}

void Callsign::Copy(const Callsign& source)
{
   QsoItem::Copy(source);

   m_callsign = source.m_callsign;
   m_prefix = source.m_prefix;
   m_suffix = source.m_suffix;

   m_isCanada = source.m_isCanada;
   m_isUSA = source.m_isUSA;
}

bool Callsign::ProcessToken(const string& token, Qso* qso)
{
   bool status = QsoItem::ProcessToken(token, qso);

   if (!status)
      return false;

   if (token.empty())
   {
      printf("Callsign::ProcessToken - empty token\n");
      return false;
   }

   auto pos = token.find('.');
   if (pos != string::npos)
   {
      printf("Callsign Error: Bad Character in Callsign %s\n", token.c_str());
   }

   pos = token.find('/');
   if (pos == string::npos)
   {
      m_callsign = token;
      DetermineCountry();
      return true;
   }

   string t1 = token.substr(0, pos);
   string t2 = token.substr(pos + 1, token.length() - pos);

   if (t1.length() == t2.length())
   {
      int i = StringUtils::GetFirstDigit(t1);
      int j = StringUtils::GetFirstDigit(t2);

      if (i >= 0)
      {
         m_callsign = t1;
         m_suffix = t2;
      }
      else
      {
         m_suffix = t1;
         m_callsign = t2;
      }
   }
   else if (t1.length() > t2.length())
   {
      m_callsign = t1;
      m_suffix = t2;
   }
   else
   {
      m_prefix = t1;
      m_callsign = t2;
   }

   DetermineCountry();

   return true;
}

void Callsign::DetermineCountry()
{
   if (m_callsign.empty())
      return;

   string lowerCallsign(m_callsign);
   StringUtils::ToLower(lowerCallsign);

   char c = lowerCallsign.at(0);
   if (c == 'a' || c == 'k' || c == 'n' || c == 'w')
   {
      m_isUSA = true;
      return;
   }
   
   string prefix2 = lowerCallsign.substr(0, 2);
   if (prefix2 == "va" || prefix2 == "ve" || prefix2 == "cf")
   {
      m_isCanada = true;
   }

   // if we get here, the callsign is not Canada or USA so the Dx function will return true
}