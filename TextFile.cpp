#include "stdafx.h"
#include "TextFile.h"


TextFile::TextFile()
	:
	m_lines(),
	m_pFile(NULL)
{
}

TextFile::~TextFile()
{
}

// Read the given file, return true on success
bool TextFile::Read(const string& filename)
{
	bool status = Open(filename);
	if (!status)
	{
		m_errorString = string("Unable to open file: ") + filename;
		return false;
	}

	const DWORD bufSize = 1024 * 1024;  // 1MB buffer
	vector<char> buffer(bufSize);

	DWORD actualBytes = 0;

	size_t bytes = fread(&buffer[0], 1, bufSize, m_pFile);

	int error = ferror(m_pFile);
	fclose(m_pFile);

	if (error == 0)
	{
		ExtractLines(buffer, bytes);
		return true;
	}

	return false;
}

bool TextFile::ExtractLines(vector<char>& buffer, size_t bytes)
{
	size_t count = 0;
	string temp;
	for (char c : buffer)
	{
		if (count++ == bytes)
		{
			if (!temp.empty())
			{
				m_lines.push_back(temp);
			}
			break;
		}

		if (c == EOF)
		{
			if (!temp.empty())
			{
				m_lines.push_back(temp);
			}
			break;
		}
		else if (c == '\n')
		{
			m_lines.push_back(temp);
			temp.erase();
		}
		else
		{
			temp.push_back(c);
		}
	}

	return true;
}

bool TextFile::Open(const string& filename)
{
	if (filename.empty())
	{
		return false;
	}

	errno_t status = fopen_s(&m_pFile, filename.c_str(), "rt");

	if (status == 0)
	{
		return true;
	}

	m_errnoOpenFile = errno;  // capture global variable
	return false;
}

// Return a copy of the text lines in the file
void TextFile::GetLines(vector<string>& lines)
{
	lines.clear();

	if (m_lines.empty())
		return;

	lines.resize(m_lines.size());

	int count = 0;
	for (string s : m_lines)
	{
		lines[count++] = s;
	}
}

bool TextFile::Write(const string& filename)
{
   if (filename.empty())
   {
      return false;
   }

   errno_t status = fopen_s(&m_pFile, filename.c_str(), "wt");

   if (status != 0)
   {
      m_errnoOpenFile = errno;  // capture global variable
      return false;
   }

   const char* eol = "\n";
   size_t count = 0;
   size_t nbytes = 0;
   for (string line : m_lines)
   {
      nbytes = line.size();
      count = fwrite(line.c_str(), 1, nbytes, m_pFile);
      fwrite(eol, 1, 1, m_pFile);
   }

   fclose(m_pFile);

   return false;
}

void TextFile::AddLines(const vector<string>& lines)
{
   for (auto iter : lines)
   {
      m_lines.push_back(iter);
   }
}
