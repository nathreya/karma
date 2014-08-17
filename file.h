#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <cstdio>
#include <fstream>
#include <istream>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#if defined _WIN32 || defined MINGW32
#include <direct.h>
#include <io.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

using std::cout;
using std::cin;
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

namespace __karma {
	namespace __io {
		class file {
			public:
				file(char const* sbfn);
				file(string sbfn);
				file();
				~file();
				bool exists();
				bool is_file();
				int file_size();
				string get_current_directory();
				bool is_directory();
				bool read_only();
				bool write_only();
				bool read_and_write();
				bool can_write();
				bool can_read();
				void operator <<(char* toWrite);
				void set_read_only();
				void set_write_only();
				void set_read_and_write();
				string get_text();
				bool operator==(file& other);
				file& operator=(file& other);
				void create_file();
				void create_directory();
				void remove_file();
				string to_string();
			private:
				char const* sFileName;
		};
	}
}

#endif
