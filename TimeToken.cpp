
#include "stdafx.h"
#include "TimeToken.h"

TimeToken::TimeToken()
   :
   QsoTokenType("time")
   {
   }

TimeToken::~TimeToken()
{
}

bool TimeToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

