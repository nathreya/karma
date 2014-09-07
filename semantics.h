#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "token.h"
#include "cpp_token.h"
#include "expr.h"
#include "lex.h"
#include "lex_utils.h"
#include "preprocess.h"
#include "graphs.h"
#include "c_ast.h"

using namespace __karma::__ast;
using namespace __karma::__lex::__token;
using namespace __karma::__lex;
using namespace __karma::__preprocess;
using namespace __karma::__preprocess::__token;
using namespace __karma::__util;

namespace __karma {
	namespace __semantics {
		shared_ptr<c_symbol> convert_c_c_declaration_to_c_symbol(shared_ptr<c_c_declaration> c_c_decl);
		void internal_compiler_error(shared_ptr<c_parser> parser, shared_ptr<c_ast_node> ast_node);
	}
}

#endif