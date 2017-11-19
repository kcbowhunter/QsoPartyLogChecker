
#include "stdafx.h"
#include "QsoTokenType.h"

QsoTokenType::QsoTokenType(const string& qsoTokenType)
   :
   m_error(false),
   m_tokenType(qsoTokenType),
   m_minSize(0)
{
}

QsoTokenType::~QsoTokenType()
{
}



