#include "stdafx.h"
#include "FileUtils.h"


FileUtils::FileUtils()
{
}


FileUtils::~FileUtils()
{
}

// return true if the given file exists
bool FileUtils::FileExists(const string& dir)
{
	if (dir.empty())
		return false;

	DWORD ftyp = GetFileAttributesA(dir.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if ((ftyp & FILE_ATTRIBUTE_DIRECTORY) == 0)
		return true;

	return true;
}
