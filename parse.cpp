#include "stdafx.h"
#include "parse.h"

namespace __karma {
	namespace __parse {
		shared_ptr<c_scope> init_global_c_scope() {
			shared_ptr<c_scope> scope;
			scope->complete_node = false;
			scope->parent_scope = nullptr;
			scope->scope_kind = c_scope_kind::SCOPE_GLOBAL;
			scope->error_node = false;
			scope->declaration_list = vector<shared_ptr<c_symbol>>();
			scope->source_begin_pos = 0;
			scope->source_end_pos = parser_unset_source_pos;
			return scope;
		}

		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl) {
			parser->complete_node = false;
			parser->error_node = false;
			parser->global_scope = init_global_c_scope();
			parser->pos = 0;
			parser->token_list = tokl;
			parser->source_begin_pos = 0;
			parser->source_end_pos = tokl.size();
			parser->global_scope->source_end_pos = tokl.size();
			return true;
		}

		shared_ptr<c_symbol> parse_external_declaration(shared_ptr<c_parser> parser) {
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			switch (tok->get_id()) {
			case token::TYPEDEF:
				int save = parser->pos;
				parser->pos++;
				shared_ptr<c_c_declaration> c_c_decl = parse_declaration(parser);
				shared_ptr<c_symbol> sym = convert_c_c_declaration_to_c_symbol(c_c_decl);
				shared_ptr<c_typedef_symbol> tsym = make_shared<c_typedef_symbol>();
				tsym->complete_node = true;
				tsym->declaration_symbol = sym;
				tsym->error_node = false;
				tsym->source_begin_pos = save;
				tsym->source_end_pos = parser->pos;
				tsym->symbol_kind = c_symbol_kind::C_SYMBOL_TYPEDEF;
				return static_pointer_cast<c_symbol>(tsym);
			};
		}
	}
}	
