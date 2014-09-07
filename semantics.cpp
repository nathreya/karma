#include "stdafx.h"
#include "semantics.h"

namespace __karma {
	namespace __semantics {
		void internal_compiler_error(shared_ptr<c_parser> parser, shared_ptr<c_ast_node> ast_node) {
			if (ast_node->source_begin_pos == parser_unset_source_pos || ast_node->source_end_pos == parser_unset_source_pos) {
				msg("???", 0, "Error: fatal internal compiler error. No source diagnostic could be given.");
			}
			else {
				msg(parser->token_list[ast_node->source_begin_pos]->get_file_name(), parser->token_list[ast_node->source_begin_pos]->get_id(), "Error: fatal internal compiler error.\nRegion given here for reference: ");
				cerr << '\n' << '\n' << '\t';
				for (parser->pos = ast_node->source_begin_pos; parser->pos < parser->token_list.size() && parser->pos < ast_node->source_end_pos; parser->pos++) {
					cerr << parser->token_list[parser->pos]->get_qualifier() << ' ';
				}
				cerr << '\n' << '\n' << '\t';
				caret(1);
				cerr << '\n';
			}
			exit(1);
		}

		shared_ptr<c_symbol> convert_c_c_declaration_to_c_symbol(shared_ptr<c_parser> parser, shared_ptr<c_c_declaration> c_c_decl) {
			if (c_c_decl == nullptr) return nullptr;
			c_c_declaration_kind ccdkind = c_c_decl->c_declaration_kind;
			if (ccdkind == c_c_declaration_kind::C_C_DECLARATION_TYPEDEF) {

			}
			else if (ccdkind == c_c_declaration_kind::C_C_DECLARATION_FUNCTION_DEFINITION) {

			}
			else if (ccdkind == c_c_declaration_kind::C_C_DECLARATION_DECLARATION || ccdkind == c_c_declaration_kind::C_C_DECLARATION_ENUM_DECLARATION || ccdkind == c_c_declaration_kind::C_C_DECLARATION_STRUCT_UNION_DECLARATION) {

			}
			else {
				internal_compiler_error(parser, c_c_decl);
			}
		}
	}
}