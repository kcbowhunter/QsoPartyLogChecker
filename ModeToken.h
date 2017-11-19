
#pragma once

#include "QsoTokenType.h"

// Class for parsing mode tokens
class ModeToken : public QsoTokenType
{
public:
   ModeToken();
   virtual ~ModeToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new ModeToken(); }

};