
#pragma once

#include "QsoItem.h"
class Qso;

class Callsign : public QsoItem
{
   public:
      Callsign(Qso *qso);
      Callsign(const Callsign& c);
      virtual ~Callsign();

      virtual void Copy(const Callsign& source);

      virtual bool ProcessToken(const string& token, Qso* qso);

      string GetCallsign() const { return m_callsign; }

      string GetPrefix() const { return m_prefix; }
      string GetSuffix() const { return m_suffix; }

      bool IsCanada() const { return m_isCanada; }
      bool IsUSA()    const { return m_isUSA; }
      bool IsUSACanada() const { return m_isCanada || m_isUSA; }
      bool IsDx()     const { return !m_callsign.empty() && !IsUSACanada(); }

private:
   string m_callsign;
   string m_prefix;
   string m_suffix;

   bool m_isCanada;  // Canadian Callsign
   bool m_isUSA;     // USA Callsign

private:
   Callsign();

   void DetermineCountry();
};

