#ifndef PARSE_H
#define PARSE_H

#include "c_ast.h"
#include "preprocess.h"
#include "graphs.h"
#include "lex_utils.h"
#include "expr.h"
#include "cpp_token.h"
#include "semantics.h"
#include "traverse_c_ast.h"
#include <memory>
#include <iostream>
#include <string>
#include <vector>

using namespace __karma::__ast;
using namespace __karma::__lex;
using namespace __karma::__lex::__token;
using namespace __karma::__preprocess;
using namespace __karma::__preprocess::__token;
using namespace __karma::__util;
using namespace __karma::__semantics;

using std::cout;
using std::string;
using std::vector;
using std::shared_ptr;

namespace __karma {
	namespace __parse {
		
		extern string const semicolon_missing_error_message;
		extern string const declarator_missing_error_message;

		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl);
		shared_ptr<c_c_declaration> parse_external_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_function_definition(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_declaration_or_function_definition(shared_ptr<c_parser> parser);
		vector<shared_ptr<c_declaration_specifier>> parse_declaration_specifier_list(shared_ptr<c_parser> parser);
		vector<shared_ptr<c_declarator>> parse_declarator_list(shared_ptr<c_parser> parser);
		shared_ptr<c_declarator> parse_declarator(shared_ptr<c_parser> parser);
	}
}

#endif
