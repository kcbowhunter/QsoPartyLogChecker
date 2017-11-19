
#pragma once

#include "QsoTokenType.h"

// Class for parsing time tokens
class TimeToken : public QsoTokenType
{
public:
   TimeToken();
   virtual ~TimeToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new TimeToken(); }
};