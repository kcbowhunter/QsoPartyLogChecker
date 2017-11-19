
#include "stdafx.h"
#include "Freq.h"
#include "StringUtils.h"
#include "QsoError.h"
#include "Station.h"
#include "Qso.h"

Freq::Freq(Qso* qso)
   :
   QsoItem(qso),
   m_band(eUnknownBand)
   {
   m_start[0] = -1;
   m_start[1] = 1800;
   m_start[2] = 3500;
   m_start[3] = 7000;
   m_start[4] = 14000;
   m_start[5] = 21000;
   m_start[6] = 28000;
   m_start[7] = 50000;
   m_start[8] = 144000;
   m_start[9] = 420000;
   m_start[10] = 50;
   m_start[11] = 144;
   m_start[12] = 420;

   m_end[0] = -1;
   m_end[1] = 2000;
   m_end[2] = 4000;
   m_end[3] = 7300;
   m_end[4] = 14350;
   m_end[5] = 21450;
   m_end[6] = 29700;
   m_end[7] = 54000;
   m_end[8] = 148000;
   m_end[9] = 450000;
   m_end[10] = 54;
   m_end[11] = 148;
   m_end[12] = 450;

   m_bands[0] = eUnknownBand;
   m_bands[1] = e160m;
   m_bands[2] = e80m;
   m_bands[3] = e40m;
   m_bands[4] = e20m;
   m_bands[5] = e15m;
   m_bands[6] = e10m;
   m_bands[7] = e6m;
   m_bands[8] = e2m;
   m_bands[9] = e440;
   m_bands[10] = e6m;
   m_bands[11] = e2m;
   m_bands[12] = e440;
   }

Freq::Freq(const Freq& f)
:
QsoItem(f),
m_band(f.m_band)
{
}

Freq::~Freq()
{
}

bool Freq::ProcessToken(const string& token, Qso* qso)
{
   QsoItem::ProcessToken(token, qso);

   bool integer = StringUtils::IsInteger(token);

   if (!integer)
   {
      Station *station = qso->GetStation();
      string callsign = station->StationCallsign();
      const char* text1 = callsign.empty() ? "(Missing)" : callsign.c_str();
      char buffer[80];
      sprintf_s(buffer, 80, "Freq Token Error: Station %s,  %s is not an integer", text1, token.c_str());
      printf("%s\n", buffer);

      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      qso->AddQsoError(qsoerror);

      return false;
   }

   int freq = atoi(token.c_str());

   m_band = FindBand(freq);
   if (m_band == eUnknownBand)
   {
      Station *station = qso->GetStation();
      string callsign = station->StationCallsign();
      const char* text1 = callsign.empty() ? "(Missing)" : callsign.c_str();
      char buffer[80];
      sprintf_s(buffer, 80, "Freq Token Error: Station %s,  %s is not a valid frequency", text1, token.c_str());
      printf("%s\n", buffer);

      QsoError *qsoerror = new QsoError();
      qsoerror->m_error = string(buffer);
      qso->AddQsoError(qsoerror);

      return false;
   }

   return true;
}

HamBand Freq::FindBand(int freq)
{
   for (int i = 1; i < eBandSize; ++i)
   {
      if (freq >= m_start[i] && freq <= m_end[i])
      {
         return m_bands[i];
      }
   }

   return eUnknownBand;
}

// Copy from the source frequency to this frequency object
void Freq::Copy(const Freq& source)
{
   if (&source == nullptr)
      return;

   QsoItem::Copy(source);
   m_band = source.m_band;
}

// Return the string equivalent for the ham band for this freq
string Freq::GetHamBandString() const
{
   return Qso::GetHamBandString(m_band);
}
