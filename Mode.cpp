
#include "stdafx.h"
#include "Mode.h"
#include "QsoError.h"
#include "Station.h"
#include "Qso.h"

bool operator==(const Mode& lhs, const Mode& rhs) { return lhs.GetMode() == rhs.GetMode(); }
bool operator!=(const Mode& lhs, const Mode& rhs) { return lhs.GetMode() != rhs.GetMode(); }

Mode::Mode(Qso* qso)
   :
   QsoItem(qso),
   m_mode(eUnknownMode)
   {
   }

Mode::Mode(const Mode& m)
:
QsoItem(m),
m_mode(m.m_mode)
{
}

Mode::~Mode()
{
}

void Mode::Copy(const Mode& source)
{
   QsoItem::Copy(source);
   m_mode = source.m_mode;
}

bool Mode::ProcessToken(const string& token, Qso* qso)
{
   QsoItem::ProcessToken(token, qso);

   if (token == "cw")
      m_mode = eModeCw;
   else if (token == "ph")
      m_mode = eModePhone;
   else if (token == "ry" || token == "pk")
      m_mode = eModeDigital;
   else
   {
      Station *station = qso->GetStation();
      string callsign = station->StationCallsign();
      const char* text1 = callsign.empty() ? "(Missing)" : callsign.c_str();
      char buffer[80];
      sprintf_s(buffer, 80, "Mode Token Error: Station %s,  %s is not a valid mode", text1, token.c_str());
      printf("%s\n", buffer);

      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      qso->AddQsoError(qsoerror);

      return false;
   }

   return true;
}
