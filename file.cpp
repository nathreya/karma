#include "stdafx.h"
#include "file.h"

namespace __karma {
	namespace __io {
		string file::to_string() {
			return string(sFileName);
		}

		void file::create_file() {
#if defined _WIN32 || defined __MINGW32__
			system(string(string("type NUL > ") + sFileName).c_str());
#else
			system(string(string("touch ") + sFileName).c_str());
#endif
		}

		void file::remove_file() {
			remove(sFileName);
		}

		void file::create_directory() {
			if(exists()) {
			}
			else {
#if defined _WIN32 || defined MINGW32
				_mkdir(sFileName);
#else
				mkdir(sFileName, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
			}	
		}

		file& file::operator=(file& param) {
			sFileName = param.sFileName;
			return*this;
		}

		bool file::operator==(file& param) {
			return( sFileName == param.sFileName);
		}

		file::file(char const* sbfn) {
			sFileName = sbfn;
		}

		file::file(string sbfn) {
#if defined _WIN32 || defined __MINGW32__
			sFileName = _strdup(sbfn.c_str());
#else
			sFileName = sbfn.c_str();
#endif
		}

		file::file() {
			sFileName = "";
		}

		file::~ file() {}

		bool file::exists() {
			struct stat s;
			if(stat(sFileName,&s) == 0) {
				if(s.st_mode & S_IFDIR) {
					return true;
				}
				else if(s.st_mode & S_IFREG) {
					return true;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}

		bool file::is_file() {
			struct stat s;
			if(stat(sFileName,&s) == 0) {
				if(s.st_mode & S_IFDIR) {
					return false;
				}
				else if(s.st_mode & S_IFREG) {
					return true;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}

		bool file::is_directory() {
			struct stat s;
			if(stat(sFileName,&s) == 0) {
				if(s.st_mode & S_IFDIR) {
					return true;
				}
				else if(s.st_mode & S_IFREG) {
					return false;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}

		int file::file_size() {
			if(exists()) {
				ifstream i;
				i.open(sFileName,std::ios_base::binary | std::ios_base::in);
				if(!i.good() || i.eof() || !i.is_open()) 
					return -1;
				i.seekg(0,std::ios_base::beg);
				std::ifstream::pos_type begin_pos = i.tellg();
				i.seekg(0,std::ios_base::end);
				return static_cast<int>(i.tellg() - begin_pos);
			}
			else {
				return -1;
			}
		}

		string file::get_current_directory() {
			char cwd_buffer[256];
#if defined _WIN32 || defined MINGW32
			char* cwd_result = _getcwd(cwd_buffer,256);
#else
			char* cwd_result = getcwd(cwd_buffer, 256);
#endif
			string result = cwd_result;
			return result;
		}

		bool file::read_only() {
			if(exists() && !can_write()) {
				return true;
			}
			else {
				return false;
			}
		}

		bool file::can_write() {
			if(exists()) {
				ofstream ifile(sFileName);
				if(ifile.is_open()) {
					return true;
				}
				else {
					return false;
				}		
			}
			else {
				return false;
			}
		}

		bool file::can_read() {
			if(exists()) {
				ifstream ifile(sFileName);
				if(ifile.is_open()) {
					return true;
				}
				else {
					return false;
				}		
			}
			else {
				return false;
			}
		}

		bool file::write_only() {
			if(exists() && !can_read()) {
				return true;
			}
			else {
				return false;
			}
		}

		bool file::read_and_write() {
			if(exists() && can_read() && can_write()) {
				return true;
			}
			else {
				return false;
			}
		}

		void file::operator <<(char* toWrite) {
			if(!exists() && !is_directory() && !read_only()) return;
			else {
				std::ofstream os;
				os.open(sFileName);
				os << toWrite;
			}
		}

		void file::set_read_only() {
			if(exists() && is_file()) {
#if defined _WIN32 || defined MINGW32
				_chmod(sFileName,S_IREAD);
#else
				chmod(sFileName, S_IREAD);
#endif
			}
			else {
			}
		}

		void file::set_read_and_write() {
			if(exists() && is_file()) {
#if defined _WIN32 || defined MINGW32
				_chmod(sFileName,S_IREAD | S_IWRITE);
#else
				chmod(sFileName, S_IREAD | S_IWRITE);
#endif
			}
		}

		void file::set_write_only() {
			if(exists() && is_file()) {
#if defined _WIN32 || defined MINGW32
				_chmod(sFileName,S_IWRITE);
#else
				chmod(sFileName, S_IWRITE);
#endif
			}
		}

		string file::get_text() {
			string line;
			string ret;
			if(!exists() || is_directory()) {
				return NULL;		
			}
			else {
				ifstream ifile(sFileName);
				return string((std::istreambuf_iterator<char>(ifile)),(std::istreambuf_iterator<char>()));		
			}		
		}
	}
}
