#ifndef PREPROCESS_H
#define PREPROCESS_H

#include "token.h"
#include "cpp_token.h"
#include "file.h"
#include "lex.h"
#include "lex_utils.h"
#include "expr.h"
#include <ctime>
#include <algorithm>

using namespace __karma::__io;
using namespace __karma::__util;
using namespace __karma::__lex;
using namespace __karma::__lex::__token;
using std::pair;

namespace __karma {
	namespace __preprocess {
		namespace __token {
			extern string const _line_macro;
			extern string const _time_macro;
			extern string const _date_macro;
			extern string const _timestamp_macro;
			extern string const _stdc_macro;
			extern string const _file_macro;
			extern string const _defined;
			extern string const _va_args_macro;
			extern string const _pragma_operator;
			extern string const _week_days [];
			extern string const _months [];
		}
		using namespace __karma::__preprocess::__token;
		void replace_macro(vector <token> & source, int & i, vector <pair<pair<string,string>,string>> symbol_table, string & _file_name, int & _line_number);
		vector <cpp_token> replace_all_defines(vector <token> & source, string _file = "");
		string evaluate_basic_bool_expr(string res);
		bool evaluate_preprocessor_conditional(string code, vector <pair<pair<string,string>,string>> symbol_table, int & line, string file, string dir);
		vector <cpp_token> preprocess(string source, string _file_name = "");
	}
}

#endif
