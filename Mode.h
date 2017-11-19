
#pragma once

#include "QsoItem.h"

enum QsoMode;

class Mode : public QsoItem
{
   public:
      Mode(Qso *qso);
      Mode(const Mode& m);
      virtual ~Mode();

      virtual void Copy(const Mode& source);

      virtual bool ProcessToken(const string& token, Qso* qso);

      QsoMode GetMode() const { return m_mode; }

      bool operator==(const Mode& rhs) { return m_mode == rhs.m_mode; }
      bool operator!=(const Mode& rhs) { return m_mode != rhs.m_mode; }

   private:
      QsoMode m_mode;

   private:
      Mode();
};

bool operator==(const Mode& lhs, const Mode& rhs);
bool operator!=(const Mode& lhs, const Mode& rhs);

