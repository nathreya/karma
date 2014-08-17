#ifndef EXPR_H
#define EXPR_H

#include <string>
#include <iostream>
#include <vector>
#include <locale>
#include <algorithm>
#include <sstream>
#include <set>
#include <cstring>
#include <iterator>
#include <stack>
#include <cstdio>
#include <fstream>
#include <map>
#include <sys/stat.h>
#include <cstring>
#include <cstring>
#include <ctime>
#include <cctype>
#include <cmath>
#include "lex_utils.h"

using namespace std;

namespace __karma {
	namespace __util {
		enum types {
			DELIMITER = 0x0001,
			NUMBER = 0x0004
		};

		class expr {
			private:
				char * pExpresion;
				char(token) [80];
				char tokenType;
				void recursive_3(int & culmination);
				void recursive_2(int & culmination);
				void recursive_1(int & culmination);
				void recursive0(int & culmination);
				void recursive1(int & culmination);
				void recursive2(int & culmination);
				void recursive3(int & culmination);
				void recursive4(int & culmination);
				void recursive5(int & culmination);
				void bomb(int & culmination);
				void nextToken();
				void serror(int error);
				int isdelim(char c);
				string file;
				string dir;
				int ecounter;
				int& line;
			public:
				expr(int & l, string f, string d);
				string use_notation();
				void set_expr(string e);
				~ expr();
				int get_error_counter();
		};
	}
}

#endif
