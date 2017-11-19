
#pragma once

#include "QsoItem.h"
class Qso;

// the date string is of the form yyyy-mm-dd
class Date : public QsoItem
{
   public:
      Date(Qso *qso);
      virtual ~Date();

      virtual bool ProcessToken(const string& token, Qso* qso);

      virtual void Copy(const QsoItem* source) override;
      virtual void Copy(const QsoItem& source) override;

      int m_minutes;

   private:
      Date();
};

