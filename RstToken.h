
#pragma once

#include "QsoTokenType.h"

// Class for parsing rst tokens
class RstToken : public QsoTokenType
{
public:
   RstToken();
   virtual ~RstToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new RstToken(); }
};