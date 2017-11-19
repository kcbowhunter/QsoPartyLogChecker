
#pragma once

#include "QsoTokenType.h"

// Class for parsing date tokens
class DateToken : public QsoTokenType
{
public:
   DateToken();
   virtual ~DateToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new DateToken(); }
};