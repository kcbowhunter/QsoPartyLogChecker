
#include "stdafx.h"

#include "QsoItem.h"

QsoItem::QsoItem(Qso* qso)
   :
   m_value(),
   m_errorString(),
   m_owner(qso)
   {
   }

QsoItem::QsoItem(const QsoItem& q)
   :
   m_value(q.m_value),
   m_errorString(),
   m_owner(nullptr)
   {
   }

QsoItem::~QsoItem()
   {
   }

// Copy the data for the qso item
void QsoItem::Copy(const QsoItem* source)
{
   if (source == nullptr)
      return;

   m_value = source->m_value;
}

void QsoItem::Copy(const QsoItem& source)
{
   m_value = source.m_value;
}


bool QsoItem::ProcessToken(const string& token, Qso *qso)
   {
      m_value = token;
      m_owner = qso;

      return true;
   }