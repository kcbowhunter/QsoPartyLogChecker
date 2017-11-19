
#include "stdafx.h"
#include "QsoName.h"
#include "Qso.h"

QsoName::QsoName(Qso* qso)
   :
   QsoItem(qso)
   {
   }

QsoName::~QsoName()
   {
   }

bool QsoName::ProcessToken(const string& token, Qso* qso)
{
   QsoItem::ProcessToken(token, qso);

   return true;
}

// Copy the data for the qso item
void QsoName::Copy(const QsoItem* source)
{
   if (source == nullptr)
      return;

   QsoItem::Copy(source);
}

void QsoName::Copy(const QsoItem& source)
{
   QsoItem::Copy(source);
}

