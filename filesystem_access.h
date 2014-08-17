#ifndef FILESYSTEM_ACCESS_H
#define FILESYSTEM_ACCESS_H

#include "file.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <cstring>
#include <sys/types.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ostream>
#include <istream>
#if defined _WIN32 || defined __MINGW32__
#include <direct.h>
#include <io.h>
#include <Windows.h>
#else
#include <dirent.h>
#endif

using std::cout;
using std::unique;
using std::string;
using std::vector;
using __karma::__io::file;

namespace __karma {
	namespace __io {
		void copy(file f, file dir);
		void cut(file f, file dir);
		void rmfile(file f);
		void rmdir(file f);
		bool is_dir(string dir);
		vector <string> map_directory(file f);
	}
}

#endif
