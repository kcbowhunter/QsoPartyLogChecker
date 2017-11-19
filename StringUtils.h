
#pragma once

#include "stdafx.h"

class StringUtils
{
public:
	StringUtils();
	~StringUtils();

	// Convert the string to lower case
	static void ToLower(string& str);

   // Convert the string to lower case
   static void ToUpper(string& str);

	// Strip off the comment from the string
	// i.e. passing '#' "hi mom # comment" returns "hi mom "
	static string RemoveComment(const string& str, const char comment);

	// Add a trailing slash if the end of the string is not a slash
	static void AddTrailingSlashIfNeeded(string& str);

   // Split the string line into tokens using ' ' and '\t' as whitespace
   static void Split(list<string>& tokens, const string& line);

   // Return the first digit from the string
   // Used to extract the digit from a callsign for sorting
   static int GetFirstDigit(const string& text);

   static string CabrilloHeaderPair(const char* key, const char* value);
   static string CabrilloHeaderPair(const char* key, const string& value);

   // Return true if the string is an integer
   static bool IsInteger(const string& token);

   // Parse str and return the key=value data, key is returned in lowercase
   static bool GetKeyValuePair(const string& str, string& key, string& value, const string& errorMsg);

   // Parse the token as a boolean, true/yes is True
   static bool ParseBoolean(bool& bvalue, const string& token);

   // Parse the token as a double, true/yes is True
   static double ParseDouble(double& bvalue, const string& token);

   // Convert ival to a string and return
   static string ToString(const int ival);

   // Replace a character in a string
   static void ReplaceCharacter(string& str, const char oldChar, const char newChar);

   // Return the number of different characters
   static int NumberOfDifferentChars(const string& str1, const string& str2);

   // pad with char c on right to make size of string n chars
   static void PadRight(string& str, char c, const int size);

   // Given the string "[abc]" return "abc"
   static bool StringUtils::ExtractHeading(string& text);
};


