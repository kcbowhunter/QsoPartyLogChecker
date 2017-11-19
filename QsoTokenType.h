
#pragma once

// Base class for token types
class QsoTokenType
{
   public:
      QsoTokenType(const string& qsoTokenType);
      virtual ~QsoTokenType();

      bool Error() const { return m_error; }
      string ErrorString() const { return m_errorString; }

      virtual bool Parse(const string& token) = 0;

      // Clone this object
      virtual QsoTokenType* Clone() = 0;

      string TokenType() const { return m_tokenType; }

      int GetMinSize() const { return m_minSize; }

   protected:
      string m_tokenType;
      string m_token;
      bool m_error;
      string m_errorString;
      int    m_minSize;  // minimum string size when writing qso as text (generating missing logs)

   private:
      QsoTokenType();
};
