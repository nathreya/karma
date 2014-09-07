#ifndef PARSE_H
#define PARSE_H

#include "c_ast.h"
#include "preprocess.h"
#include "graphs.h"
#include "lex_utils.h"
#include "expr.h"
#include "cpp_token.h"
#include "semantics.h"
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
		shared_ptr<c_scope> init_global_c_scope();
		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl);
		shared_ptr<c_symbol> parse_external_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_function_definition(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_declaration_or_function_definition(shared_ptr<c_parser> parser);
	}
}

#endif
