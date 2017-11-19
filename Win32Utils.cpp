
#include "stdafx.h"
#include "Win32Utils.h"



Win32Utils::Win32Utils()
   {
   }

Win32Utils::~Win32Utils()
   {

   }

// Return current working directory (with trailing backslash)
string Win32Utils::GetCurrentWorkingDirectory()
{
   string folder;

   const DWORD buffsize = 2048;
   char buffer[buffsize + 1];
   DWORD ret = ::GetCurrentDirectory(buffsize, buffer);

   folder = string(buffer) + string("\\");

   return folder;
}
