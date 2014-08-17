#ifndef LEX_UTILS_H
#define LEX_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include <locale>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <Windows.h>

using std::cout;
using std::string;
using std::vector;
using std::istream;
using std::fstream;
using std::ostringstream;
using std::stringstream;
using std::istringstream;
using std::locale;
using std::cerr;

namespace __karma {
	namespace __util {
		vector <string> & split(string toParse, char delim, vector <string>& elements);
		vector <string> split(string toParse, char delim);
		string clean_space(string str);
		void leading_trim(string& str, locale const& loc = locale());
		void trailing_trim(string& str, locale const& loc = locale());
		void trim(string& str, locale const& loc = locale());
		int _stoi(string str);
		string itos(int value);
		string replace_all(string toReplace, string find, string replace);
		vector <string>& split(string toParse, char delim, vector <string> & elements);
		vector <string> split(string toParse, char delim);
		string clean_space(string str);
		void leading_trim(string& str, locale const & loc);
		void trailing_trim(string& str, locale const & loc);
		void trim(string& str, locale const & loc);
		int _stoi(string str);
		string itos(int value);
		string replace_all(string toReplace, string find, string replace);
		bool ends_with(string& toSearch, string& toCheck);
		void msg(string file, int line, string msg);
		void note(string file, int line, string msg);
		void warn(string file, int line, string msg);
		void caret(int pos);
	}
}

#endif
