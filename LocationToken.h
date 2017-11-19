
#pragma once

#include "QsoTokenType.h"

// Class for parsing location tokens
class LocationToken : public QsoTokenType
{
public:
   LocationToken();
   virtual ~LocationToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new LocationToken(); }

};