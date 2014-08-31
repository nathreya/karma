#include "stdafx.h"
#include "parse.h"

namespace __karma {
	namespace __parse {
		shared_ptr<c_scope> init_global_c_scope() {
			shared_ptr<c_scope> scope;
			scope->complete_node = false;
			scope->parent_scope = nullptr;
			scope->scope_kind = c_scope_kind::SCOPE_GLOBAL;
			return scope;
		}

		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl) {
			parser->complete_node = false;
			parser->error_node = false;
			parser->global_scope = init_global_c_scope();
			parser->pos = 0;
			parser->token_list = tokl;
			return true;
		}
	}
}	
