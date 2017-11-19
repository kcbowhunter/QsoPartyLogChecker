
#pragma once

class Qso;

class QsoItem
{
public:
   QsoItem(Qso* qso=nullptr);
   QsoItem(const QsoItem& q);
   virtual ~QsoItem();

   virtual void Copy(const QsoItem* source);
   virtual void Copy(const QsoItem& source);

   void SetValue(const string&s) { m_value = s; }
   string GetValue() const { return m_value; }

   virtual bool ProcessToken(const string& token, Qso* qso);

   bool HasErrors() const { return !m_errorString.empty(); }
   string GetErrorString() const { return m_errorString; }

   void SetOwner(Qso* qso) { m_owner = qso; }

protected:
   string m_value;
   Qso *m_owner;

   string m_errorString;
};