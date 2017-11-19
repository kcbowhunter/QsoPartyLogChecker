#pragma once
class FileUtils
{
public:
	FileUtils();
	~FileUtils();

	// return true if the given file exists
	static bool FileExists(const string& dir);
};

