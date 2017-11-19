
#pragma once

#include "QsoItem.h"
class Qso;

class SerialNumber : public QsoItem
{
   public:
      SerialNumber(Qso *qso);
      SerialNumber(const SerialNumber& m);

      virtual ~SerialNumber();

      virtual void Copy(const SerialNumber& source);

      virtual bool ProcessToken(const string& token, Qso* qso);

      int GetSerialNumber() const { return m_serialNumber; }

private:
   int m_serialNumber;

private:
   SerialNumber();
};

