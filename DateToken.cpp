
#include "stdafx.h"
#include "DateToken.h"

DateToken::DateToken()
:
QsoTokenType("date")
{
}


DateToken::~DateToken()
{
}

bool DateToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

