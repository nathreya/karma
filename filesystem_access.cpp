#include "stdafx.h"
#include "filesystem_access.h"
#include "file.h"

namespace __karma {
	namespace __io {

		void copy(file f, file dir) {
			string orig = f.to_string();
			string now = dir.to_string();
#if defined _WIN32 || defined __MINGW32__ 
			string command = "copy \"" + orig + "\" \"" + now + "\"";
#else
			string command = "cp \"" + orig + "\" \"" + now + "\"";
#endif
			system(command.c_str());
		}

		void cut(file f, file dir) {
			copy(f, dir);
			remove(f.to_string().c_str());
		}

		void rmfile(file f) {
			remove(f.to_string().c_str());
		}

		void rmdir(file f) {
#if defined _WIN32 || defined __MINGW32__
			string command = "rmdir \"" + f.to_string() + "\"";
#else
			string command = "rm -r \"" + f.to_string() + "\"";
#endif
			system(command.c_str());
		}

#if defined __MINGW32__ || defined _WIN32
		vector<string> map_directory(file f) {

			char* sDir = _strdup(f.to_string().c_str());

			WIN32_FIND_DATA fdFile;
			HANDLE hFind = NULL;
			vector<string> sf;

			char sPath[2048];
			string sp = sDir;
			if (sp[sp.length() - 1] == '\\') sp.erase(sp.length() - 1, 1);
			sDir = _strdup(sp.c_str());
			sprintf(sPath, "%s\\*.*", sDir);
			if ((hFind = FindFirstFile((LPCWSTR) sPath, &fdFile)) == INVALID_HANDLE_VALUE) {

				return sf;
			}

			do {

				if (strcmp((const char*) fdFile.cFileName, ".") != 0 && strcmp((const char*) fdFile.cFileName, "..") != 0) {
					sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
					if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) {

						sf.push_back(sPath);
						vector<string> t;
						file f(sPath);
						t = map_directory(f);
						for (int i = 0; i < t.size(); i++) {

							sf.push_back(t[i]);
						}
					}

					else {

						sf.push_back(sPath);
					}
				}
			} while (FindNextFile(hFind, &fdFile));

			FindClose(hFind);
			return sf;
		}
#else
		bool is_dir(string dir) {
			struct stat file_info;
			stat(dir.c_str(), &file_info);
			if(S_ISDIR(file_info.st_mode)) {
				return true;
			} 
			else {
				return false;
			}
		}

		vector <string> map_directory(file f) {
			string base_dir = f.to_string();
			if(base_dir[base_dir.length() - 1] == '/');
			else base_dir += '/';
			DIR *dp;
			struct dirent *dirp;
			vector<string> ret;
			if((dp = opendir(base_dir.c_str())) == NULL) {
				return ret;
			} 
			else {
				while((dirp = readdir(dp)) != NULL) {
					if(dirp->d_name != string(".") && dirp->d_name != string("..")) {
						if(is_dir(base_dir + dirp->d_name) == true) {
							ret.push_back(base_dir + dirp->d_name + "/");
							vector<string> tmp = map_directory(base_dir + dirp->d_name + "/");
							ret.insert(ret.end(), tmp.begin(), tmp.end());	
						} 
						else {
							ret.push_back(base_dir + dirp->d_name);
						}
					}
				}
				closedir(dp);
			}
			return ret;
		}
#endif
	}
		}
