#pragma once
#include "QsoTokenType.h"

// Class for parsing name tokens
class NameToken : public QsoTokenType
{
public:
   NameToken();
   virtual ~NameToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new NameToken(); }
};