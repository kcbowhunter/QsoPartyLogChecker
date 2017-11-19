
#include "stdafx.h"
#include "CallsignToken.h"


CallsignToken::CallsignToken()
:
QsoTokenType("callsign")
{
   m_minSize = 7;  // minimum number of characters when writing out as a string
}

CallsignToken::~CallsignToken()
{
}

bool CallsignToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

