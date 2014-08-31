#ifndef PARSE_H
#define PARSE_H

#include "c_ast.h"
#include "preprocess.h"
#include "graphs.h"
#include "lex_utils.h"
#include "expr.h"
#include "cpp_token.h"
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
using std::cout;
using std::string;
using std::vector;
using std::shared_ptr;

namespace __karma {
	namespace __parse {

		struct c_parser : public c_ast_node {
			vector<shared_ptr<cpp_token>> token_list;
			int pos;
			shared_ptr<c_scope> global_scope;
		};

		shared_ptr<c_scope> init_global_c_scope();
		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl);
		shared_ptr<c_symbol> external_declaration();
	}
}

#endif
