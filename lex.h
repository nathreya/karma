#ifndef LEX_H
#define LEX_H

#include "token.h"
#include <iostream>
#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include <locale>
#include <sstream>

using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::istream;
using std::fstream;
using std::ostringstream;
using std::stringstream;
using std::istringstream;
using std::locale;

using namespace __karma::__util;
using namespace __karma::__lex::__token;

namespace __karma {
	namespace __lex {
		string remove_line_continuations(string source);
		vector <token> lex(string source);
	}
}

#endif
