
#include "stdafx.h"
#include "LocationToken.h"

LocationToken::LocationToken()
:
QsoTokenType("location")
{
}

LocationToken::~LocationToken()
{
}

bool LocationToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

