
#pragma once

#include "QsoItem.h"

class Qso;
enum HamBand;

class Freq : public QsoItem
{
   public:
      Freq(Qso* owner);
      Freq(const Freq& f);
      virtual ~Freq();

      // Copy from the source frequency to this frequency object
      virtual void Copy(const Freq& source);

      virtual bool ProcessToken(const string& token, Qso* qso);

      HamBand GetBand() const { return m_band; }

      HamBand FindBand(int freq);

      // Return the string equivalent for the ham band for this freq
      string GetHamBandString() const;

      bool SameHamBand(const Freq& freq) const { return m_band == freq.m_band; }

   private:
      HamBand m_band;

      enum BandSize { eBandSize = 13 };
      int m_start[eBandSize];
      int m_end[eBandSize];
      HamBand m_bands[eBandSize];

   private:
      Freq();
};

