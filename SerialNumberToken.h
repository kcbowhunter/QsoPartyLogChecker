
#pragma once

#include "QsoTokenType.h"

// Class for parsing serial number tokens
class SerialNumberToken : public QsoTokenType
{
public:
   SerialNumberToken();
   virtual ~SerialNumberToken();

   virtual bool Parse(const string& token);

   virtual QsoTokenType* Clone() { return new SerialNumberToken(); }
};