
#include "stdafx.h"
#include "FreqToken.h"

FreqToken::FreqToken()
:
QsoTokenType("freq")
{
   m_minSize = 5;  // minimum number of characters when writing out as a string
}

FreqToken::~FreqToken()
{
}

bool FreqToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

