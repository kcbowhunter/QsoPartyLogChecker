
#pragma once


class Win32Utils
{
   public:
      Win32Utils();
      ~Win32Utils();

      // Return current working directory (with trailing backslash)
      static string GetCurrentWorkingDirectory();
};

