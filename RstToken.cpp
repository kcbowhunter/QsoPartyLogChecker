
#include "stdafx.h"
#include "RstToken.h"


RstToken::RstToken()
:
QsoTokenType("rst")
{
}


RstToken::~RstToken()
{
}

bool RstToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

