
#pragma once

#include "QsoItem.h"
class Qso;

// parse the time in the format "hhmm"
class QsoTime : public QsoItem
{
   public:
      QsoTime(Qso *qso);
      virtual ~QsoTime();

      virtual bool ProcessToken(const string& token, Qso* qso);

      virtual void Copy(const QsoItem* source) override;
      virtual void Copy(const QsoItem& source) override;

      // hh*60 + mm
      int m_minutes;

   private:
      QsoTime();
};

