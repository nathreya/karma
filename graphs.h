#ifndef GRAPHS_H
#define GRAPHS_H

#include <iostream>
#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include "lex_utils.h"
#include "lex.h"
#include "token.h"

using namespace __karma::__lex;
using namespace __karma::__util;
using namespace __karma::__lex::__token;
using std::cout;
using std::string;
using std::vector;
using std::istream;
using std::fstream;

namespace __karma {
	namespace __lex {
		string remove_trigraphs(string source);
		string remove_digraphs(string source);
		string remove_graphs(string source);
	}
}

#endif
