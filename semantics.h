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
#include "traverse_c_ast.h"

using namespace __karma::__ast;
using namespace __karma::__lex::__token;
using namespace __karma::__lex;
using namespace __karma::__preprocess;
using namespace __karma::__preprocess::__token;
using namespace __karma::__util;

namespace __karma {
	namespace __semantics {
		enum c_parser_diagnostic_kind {
			PARSER_DIAGNOSTIC_KIND_ERROR = 0x0001, PARSER_DIAGNOSTIC_KIND_WARNING = 0x0002, PARSER_DIAGNOSTIC_KIND_NOTE = 0x0004
		};

		c_type_kind determine_declaration_specifier_list_type_kind(vector<shared_ptr<c_declaration_specifier>> declspec_list);

		void internal_compiler_error(shared_ptr<c_parser> parser, shared_ptr<c_ast_node> ast_node);
		void parser_message(shared_ptr<c_parser> parser, string file, int line, string message, c_parser_diagnostic_kind kind);
		shared_ptr<c_scope> init_global_c_scope();
		bool remove_last_scope(shared_ptr<c_scope> scope);
		//run this before adding new values to a scope: for function definitions, add incomplete symbol in beginning, complete at end
		//make sure to check c_scope::declaration_list.size() != c_scope::scope_separators[c_scope::scope_separators.size() - 1]
		//current scope: c_scope::scope_separators[c_scope::scope_separators.size() - 1] <= x < c_scope::declaration_list.size()
		bool init_new_scope(shared_ptr<c_scope> scope, c_scope_kind kind);
	}
}

#endif