
#pragma once

#include "QsoItem.h"
class Qso;

// Parse the name for a qso
class QsoName : public QsoItem
{
   public:
      QsoName(Qso *qso);
      virtual ~QsoName();

      virtual bool ProcessToken(const string& token, Qso* qso);

      virtual void Copy(const QsoItem* source) override;
      virtual void Copy(const QsoItem& source) override;

   private:
      QsoName();
};

