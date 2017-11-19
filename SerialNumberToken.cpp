
#include "stdafx.h"
#include "SerialNumberToken.h"

SerialNumberToken::SerialNumberToken()
:
QsoTokenType("serialnumber")
{
}

SerialNumberToken::~SerialNumberToken()
{
}

bool SerialNumberToken::Parse(const string& token)
{
   m_token = token;

   return true;
}

