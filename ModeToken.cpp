
#include "stdafx.h"
#include "ModeToken.h"

ModeToken::ModeToken()
:
QsoTokenType("mode")
{
}


ModeToken::~ModeToken()
{
}

bool ModeToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

