#pragma once

#include "stdafx.h"

class TextFile
{
public:
	TextFile();
	virtual ~TextFile();

	// Read the given TextFile, return true on success
	bool Read(const string& TextFilename);

   // Write the file, return true on success 
   bool Write(const string& TextFilename);

	int ErrnoOpenTextFile() const { return m_errnoOpenFile; }

	// Return a copy of the text lines in the TextFile
	void GetLines(vector<string>& lines);

	// Error string
	string ErrorString() const { return m_errorString; }

   void AddLine(const string& line) { m_lines.push_back(line); }

   void AddLine(const char* buffer)
   {
	   if (*buffer)
		   AddLine(string(buffer));
   }

   void AddLines(const vector<string>& lines);

private:
	list<string> m_lines;
	FILE* m_pFile;

	bool Open(const string& TextFilename);
	bool ExtractLines(vector<char>& buffer, size_t bytes);

	int m_errnoOpenFile; // error from opening the TextFile
	string m_errorString;
};
