#pragma once

// DXCC Country
class DxccCountry
{
public:
	DxccCountry();
	~DxccCountry();

	int GetNumber() const { return m_number;  }
	void SetNumber(const int num) { m_number = num; }

	const string& GetName() const { return m_name; }
	void SetName(const string& name) { m_name = name; }
	
	bool AddPrefix(const string& prefix);
	
	const string& GetDefaultPrefix() const { return m_defaultPrefix; }
	void SetDefaultPrefix(const string& prefix) { m_defaultPrefix = prefix; }

	// Return true if the callsign matches this dxcc country
	// Also return the number of matching characters
	bool Match(const string& callsign, int& count) const;

	// Return true if the country is dx (not USA or Canada)
	bool IsDxStation() const;

private:
	int m_number;   // Country Number
	string m_name;  // Country Name
	string m_defaultPrefix;

	set<string> m_prefixes;
};

