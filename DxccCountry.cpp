#include "stdafx.h"
#include "DxccCountry.h"

#include "boost/algorithm/string.hpp"
using namespace boost;


DxccCountry::DxccCountry()
	:
	m_number(0),
	m_name(),
	m_defaultPrefix()
{
}

DxccCountry::~DxccCountry()
{
}

bool DxccCountry::AddPrefix(const string& prefix)
{
	if (prefix.empty())
		return true;

	if (m_prefixes.empty())
	{
		m_prefixes.insert(prefix);
		return true;
	}

	auto iter = m_prefixes.find(prefix);
	if (iter == m_prefixes.end())
	{
		m_prefixes.insert(prefix);
	}
	else
	{
		printf("DxccCountry Warning: prefix %s already exists for country %s\n", prefix.c_str(), m_name.c_str());
		return true;  // prefix already exists
	}

	return true;
}

// Return true if the callsign matches this dxcc country
// Also return the number of matching characters
bool DxccCountry::Match(const string& callsign, int& count) const
{
	count = 0;
	auto iter    = m_prefixes.begin();
	auto iterEnd = m_prefixes.end();
	for (; iter != iterEnd; ++iter)
	{
		const string& prefix = *iter;
		size_t len = prefix.length();
		string s = callsign.substr(0, len);
		if (boost::iequals(s, prefix))
		{
			if (len > count)
			{
				count = (int)len;
			}
		}
	}

	return count > 0;
}

// Return true if the country is dx (not USA or Canada)
bool DxccCountry::IsDxStation() const
{
	bool dx = true;

	if (boost::iequals("usa", m_name))
	{
		dx = false;
	}
	else if (boost::iequals("canada", m_name))
	{
		dx = false;
	}

	return dx;
}
