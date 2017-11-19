
#include "stdafx.h"
#include "StringUtils.h"
#include "boost/algorithm/string.hpp"

// convert the given string to lower case
void StringUtils::ToLower(string& str)
{
	if (str.empty())
		return;

	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

// convert the given string to lower case
void StringUtils::ToUpper(string& str)
{
   if (str.empty())
      return;

   std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}


// Strip off the comment from the string
// i.e. passing '#' "hi mom # comment" returns "hi mom "
string StringUtils::RemoveComment(const string& str, const char comment)
{
	if (str.empty())
		return string();

	size_t pos = str.find(comment);
	if (pos == string::npos)
	{
		return string(str);
	}

	if (pos == 0)
	{
		return string();
	}

	string retval = str.substr(0, pos);

	return retval;
}

// Add a trailing slash if the end of the string is not a slash
void StringUtils::AddTrailingSlashIfNeeded(string& str)
{
	if (str.empty())
		return;

	size_t len = str.length();
	char c = str.at(len - 1);

	if (c != '\\')
	{
		str += string("\\");
	}
}

// Split the string line into tokens using ' ' and '\t' as whitespace
void StringUtils::Split(list<string>& tokens, const string& line)
{
   tokens.clear();

   const int looking = 1;
   const int found = 2;
   int phase = looking;

   const int size = (int)line.size();
   int i = 0;
   char c = ' ';
   string str;
   while (i < size)
   {
      c = line.at(i++);
      if (phase == looking)
      {
         if (c != ' ' && c != '\t')
         {
            phase = found;
            str += c;
         }
      }
      else  // phase == found
      {
         if (c == ' ' || c == '\t')
         {
            tokens.push_back(str);
            str.clear();
            phase = looking;
         }
         else
         {
            str += c;
         }
      }
   }

   if (!str.empty())
   {
      tokens.push_back(str);
   }
}

// Return the first digit from the string
// Used to extract the digit from a callsign for sorting
int StringUtils::GetFirstDigit(const string& text)
{
   if (text.empty())
      return -1;

   int result = -1;
   const char* str = text.c_str();
   size_t length = text.length();
   for (size_t i = 0; i < length; ++i)
   {
      char c = str[i];
      if (c >= '0' && c <= '9')
      {
         result = c - '0';
         break;
      }
   }

   return result;
}

string StringUtils::CabrilloHeaderPair(const char* key, const string& value)
{
   if (value.empty())
      return CabrilloHeaderPair(key, "?");

   return CabrilloHeaderPair(key, value.c_str());
}

string StringUtils::CabrilloHeaderPair(const char* key, const char* value)
{
	size_t len = strlen(key) + strlen(value) + 25;
	char *buffer = new char[len];

//   char buffer[80];
   sprintf_s(buffer, len, "%-23s : %s", key, value);
   string ret = string(buffer);
   delete[] buffer;
   return ret;
}

bool StringUtils::IsInteger(const string& token)
{
   if (token.empty())
      return false;

   const char* str = token.c_str();
   size_t length = token.length();
   for (size_t i = 0; i < length; ++i)
   {
      char c = str[i];
      if (c < '0' || c > '9')
      {
         return false;
      }
   }

   return true;
}

// Parse str and return the key=value data, key is returned in lowercase
bool StringUtils::GetKeyValuePair(const string& str, string& key, string& value, const string& errorMsg)
{
   size_t pos = str.find('=');
   if (pos == string::npos)
   {
      printf("Error in %s Processing key value pair, '=' not found: %s\n", errorMsg.c_str(), str.c_str());
      return false;
   }

   size_t len = str.size();
   if (pos == 0 || pos == len - 1)
   {
      printf("Error in %s Processing Section data, bad key=value pair: %s\n", errorMsg.c_str(), str.c_str());
      return false;
   }

   key = str.substr(0, pos);
   value = str.substr(pos + 1, len - pos);

   boost::trim(key);
   boost::trim(value);

   if (key.empty() || value.empty())
   {
      printf("Error in %s Processing Section data, missing key and/or value %s\n", errorMsg.c_str(), str.c_str());
      return false;
   }

   return true;
}

// Parse the as a boolean, true/yes is True
// yes, true -> True
// no, false -> False
bool StringUtils::ParseBoolean(bool& bvalue, const string& tokenArg)
{
   string token(tokenArg);
   ToLower(token);

   bvalue = false;
   if (token.empty())
      return false;

   char c = token.at(0);
   if (c == 't' || c == 'y')
      bvalue = true;
   else if (c == 'f' || c == 'n')
      bvalue = false;
   else
      return false;

   return true;
}

double StringUtils::ParseDouble(double& value, const string& token)
{
	if (token.empty())
	{
		value = -1.0E100;
		return value;
	}

	double result = atof(token.c_str());

	value = result;

	return result;
}

// Convert ival to a string and return
string StringUtils::ToString(const int ival)
{
   char buffer[80];
   sprintf_s(buffer, 80, "%d", ival);

   string line(buffer);
   return line;
}

// Replace a character in a string
void StringUtils::ReplaceCharacter(string& str, const char oldChar, const char newChar)
{
   if (str.empty())
      return;

   size_t len = str.length();
   for (size_t i = 0; i < len; ++i)
   {
      if (str[i] == oldChar)
      {
         str[i] = newChar;
      }
   }
}

// Return the number of different characters
int StringUtils::NumberOfDifferentChars(const string& str1, const string& str2)
{
   const int l1 = (int)str1.length();
   const int l2 = (int)str2.length();

   if (str1.empty())
      return l2;

   if (str2.empty())
      return l1;

   if (l1 > l2)
   {
      int count = 0;
      for (int i = 0; i < l2; ++i)
      {
         if (str1[i] != str2[i])
            ++count;
      }
      return count + (l1 - l2);
   }
   else if (l2 > l1)
   {
      int count = 0;
      for (int i = 0; i < l1; ++i)
      {
         if (str1[i] != str2[i])
            ++count;
      }
      return count + (l2 - l1);
   }
   else
   {
      int count = 0;
      for (int i = 0; i < l1; ++i)
      {
         if (str1[i] != str2[i])
            ++count;
      }
      return count;
   }

   return 0;
}

// pad with char c on right to make size of string n chars
void StringUtils::PadRight(string& str, char c, const int size)
{
   int strSize = (int)str.size();

   // If the string is greater than or equal to the desired size, do nothing
   if (strSize >= size)
      return;

   for (int i = 0; i < size - strSize; ++i)
   {
      str = str + c;
   }
}

// Given the string "[abc]" return "abc"
bool StringUtils::ExtractHeading(string& text)
{
	boost::trim(text);
	if (text.empty())
		return false;

	size_t len = text.size();
	char first = text.at(0);
	char last = text.at(len - 1);
	if (first != '[' || last != ']')
	{
		text.clear();
		return false;
	}

	text = text.substr(1, len - 2);
	boost::trim(text);

	return true;
}