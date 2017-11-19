
#include "stdafx.h"
#include "SerialNumber.h"
#include "StringUtils.h"
#include "Station.h"
#include "QsoError.h"
#include "Qso.h"

SerialNumber::SerialNumber(Qso *qso)
   :
   QsoItem(qso),
   m_serialNumber(0)
   {
   }

SerialNumber::SerialNumber(const SerialNumber& m)
:
QsoItem(m),
m_serialNumber(m.m_serialNumber)
{
}

SerialNumber::~SerialNumber()
{
}

void SerialNumber::Copy(const SerialNumber& source)
{
   QsoItem::Copy(source);
   m_serialNumber = source.m_serialNumber;
}

bool SerialNumber::ProcessToken(const string& token, Qso* qso)
{
   QsoItem::ProcessToken(token, qso);

   if (token.empty())
   {
      Station *station = qso->GetStation();
      string callsign = station->StationCallsign();
      const char* text1 = callsign.empty() ? "(Missing)" : callsign.c_str();
      char buffer[80];
      sprintf_s(buffer, 80, "SerialNumber Token Error: Station %s,  serial number is missing", text1);
      printf("%s\n", buffer);

      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      qso->AddQsoError(qsoerror);

      return false;
   }

   bool integer = StringUtils::IsInteger(token);

   if (!integer)
   {
      Station *station = qso->GetStation();
      string callsign = station->StationCallsign();
      const char* text1 = callsign.empty() ? "(Missing)" : callsign.c_str();
      char buffer[80];
      sprintf_s(buffer, 80, "SerialNumber Token Error: Station %s,  %s is not a valid serial number", text1, token.c_str());
      printf("%s\n", buffer);

      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      qso->AddQsoError(qsoerror);
   }
   else
   {
      m_serialNumber = atoi(token.c_str());
   }


   return true;
}