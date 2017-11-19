
#include "stdafx.h"
#include "NameToken.h"

NameToken::NameToken()
:
QsoTokenType("name")
{
}

NameToken::~NameToken()
{
}

bool NameToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

