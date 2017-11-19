
#pragma once

#include "QsoTokenType.h"

// Class for parsing frequency tokens
class FreqToken : public QsoTokenType
{
public:
   FreqToken();
   virtual ~FreqToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new FreqToken(); }

};