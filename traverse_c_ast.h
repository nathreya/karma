#ifndef TRAVERSE_C_AST_H
#define TRAVERSE_c_AST_H

#include "c_ast.h"
#include "lex_utils.h"
#include "lex.h"
#include "cpp_token.h"
#include "expr.h"
#include "graphs.h"
#include "preprocess.h"

using namespace __karma::__lex::__token;
using namespace __karma::__lex;
using namespace __karma::__preprocess;
using namespace __karma::__preprocess::__token;
using namespace __karma::__util;

namespace __karma {
	namespace __ast {
		vector<shared_ptr<c_expression>> query_identifier(shared_ptr<c_c_declaration> decl);
	}
}

#endif