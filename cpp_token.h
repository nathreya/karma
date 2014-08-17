#ifndef CPP_TOKEN_H
#define CPP_TOKEN_H

#include "token.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>

using std::cout;
using std::cin;
using std::string;
using std::vector;
using std::shared_ptr;
using std::ostringstream;

using namespace __karma::__lex::__token;

namespace __karma {
	namespace __preprocess {
		namespace __token {
			class cpp_token {
				public:
					cpp_token(string f, int l, token t);
					~ cpp_token();
					string get_qualifier();
					int get_id();
					int get_line_number();
					string get_file_name();
					bool wide();
					c_suffix_token_kind get_c_suffix_token_kind() const;
				private:
					int _line_number;
					string _file_name;
					string _qual;
					int _id;
					bool flag;
					c_suffix_token_kind suffix_token_kind;
			};
		}
	}
}

#endif
