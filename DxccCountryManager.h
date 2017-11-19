#pragma once


// Manages DXCC Country Classes
// Allows the user to locate the country given a callsign
// Country definitions are driven by a text file
// [USA]
// K
// W
// N
// AA
// ...

class DxccCountry;

class DxccCountryManager
{
public:
	DxccCountryManager();
	~DxccCountryManager();

	bool ProcessDxccCountriesFile(const string& dxccfilename);

	// Return the dxcc country for the given callsign
	bool FindCountryName(const string& callsign, string& country);

	// Return the dxcc country object for the given callsign
	DxccCountry *FindCountry(const string& callsign) const;

	// Return the dxcc country object with the given country name
	DxccCountry *FindCountryByName(const string& countryName) const;

private:
	list<DxccCountry*> m_countries;  // dxcc countries
};

