
#pragma once

#include "QsoTokenType.h"

// Class for parsing callsign tokens
class CallsignToken : public QsoTokenType
{
public:
   CallsignToken();
   virtual ~CallsignToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new CallsignToken(); }
};