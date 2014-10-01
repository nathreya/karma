#include "stdafx.h"
#include "semantics.h"

namespace __karma {
	namespace __semantics {
		void internal_compiler_error(shared_ptr<c_parser> parser, shared_ptr<c_ast_node> ast_node) {
			if (parser->token_list.size() == 0 || ast_node == nullptr || ast_node->source_begin_pos == parser_unset_source_pos || ast_node->source_end_pos == parser_unset_source_pos) {
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

		void parser_message(shared_ptr<c_parser> parser, string file, int line, string message, c_parser_diagnostic_kind kind) {
			if (kind == c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR)
				msg(file, line, message);
			else if (kind == c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_WARNING)
				warn(file, line, message);
			else if (kind == c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_NOTE) {
				note(file, line, message);
				return;
			}
			else
				internal_compiler_error(parser, nullptr);
			cerr << '\n' << '\n' << '\t';
			int limit = parser->pos;
			if (limit == parser->temporary_pos) limit++;
			for (int pos = parser->temporary_pos; pos < limit; pos++) {
				cerr << parser->token_list[pos]->get_qualifier() << ' ';
			}
			cerr << '\n' << '\n' << '\t';
			caret(1);
			cerr << '\n';
		}

		shared_ptr<c_scope> init_global_c_scope() {
			shared_ptr<c_scope> scope;
			scope->complete_node = false;
			scope->scope_separators.push_back(0); //global scope
			scope->scope_kind_list.push_back(c_scope_kind::SCOPE_GLOBAL);
			scope->error_node = false;
			scope->declaration_list = vector<shared_ptr<c_c_declaration>>();
			scope->source_begin_pos = 0;
			scope->source_end_pos = parser_unset_source_pos;
			return scope;
		}

		bool remove_last_scope(shared_ptr<c_scope> scope) {
			scope->declaration_list.erase(scope->declaration_list.begin() + scope->scope_separators[scope->scope_separators.size() - 1], scope->declaration_list.end()); 
			scope->scope_separators.erase(scope->scope_separators.end() - 1, scope->scope_separators.end());
			scope->scope_kind_list.erase(scope->scope_kind_list.end() - 1, scope->scope_kind_list.end());
			return true;
		}

		bool init_new_scope(shared_ptr<c_scope> scope, c_scope_kind kind) {
			scope->scope_separators.push_back(scope->declaration_list.size());
			scope->scope_kind_list.push_back(kind);
			return true;
		}

		c_type_kind determine_declaration_specifier_list_type_kind(vector<shared_ptr<c_declaration_specifier>> declspec_list) {

		}
	}
}