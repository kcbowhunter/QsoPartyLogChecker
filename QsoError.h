
#pragma once

enum QsoErrorType { eUnknownError, eQsoNotFoundInLog, eQsoLoggedWrongCallsign, eDuplicateQso};

class QsoError
{
public:
   QsoError(QsoErrorType errorType = eUnknownError);
   virtual ~QsoError();

   string m_error;
   QsoErrorType m_errorType;
};