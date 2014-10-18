#include "stdafx.h"
#include "semantics.h"

namespace __karma {
	namespace __semantics {

#define SEMANTICS_ASSERT(EXPRESSION) if(!(EXPRESSION)) semantics_assert(#EXPRESSION, __FILE__, __LINE__)

		void semantics_assert(char* message, char* file, int line) {
			cerr << "Semantics error(" << file << "," << line << "):" << message << "\n";
			std::terminate();
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
				SEMANTICS_ASSERT(true && "c_parser_diagnostic_kind not detected (this should be unreachable).");
			if (kind == c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR || kind == c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_WARNING) {
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
		}

		shared_ptr<c_scope> init_global_c_scope() {
			shared_ptr<c_scope> scope = make_shared<c_scope>();
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

		c_precedence_level get_binary_operator_c_precedence_level(shared_ptr<cpp_token> tok) {
			SEMANTICS_ASSERT(tok != nullptr && "tok should never equal a nullptr.");
			switch (tok->get_id()) {
			case token::STAR_EQUALS:
			case token::DIVIDE_EQUALS:
			case token::MODULUS_EQUALS:
			case token::PLUS_EQUALS:
			case token::MINUS_EQUALS:
			case token::SHIFT_LEFT_EQUALS:
			case token::SHIFT_RIGHT_EQUALS:
			case token::AND_EQUALS:
			case token::OR_EQUALS:
			case token::XOR_EQUALS: 
				return c_precedence_level::PRECEDENCE_LEVEL_ASSIGNMENT;
				break;
			case token::SHIFT_LEFT:
			case token::SHIFT_RIGHT: 
				return c_precedence_level::PRECEDENCE_LEVEL_SHIFT;
				break;
			case token::QUESTION_MARK:
				return c_precedence_level::PRECEDENCE_LEVEL_CONDITIONAL;
				break;
			case token::OR:
				return c_precedence_level::PRECEDENCE_LEVEL_OR;
				break;
			case token::AND:
				return c_precedence_level::PRECEDENCE_LEVEL_AND;
				break;
			case token::SINGLE_OR:
				return c_precedence_level::PRECEDENCE_LEVEL_INCLUSIVE_OR;
				break;
			case token::XOR:
				return c_precedence_level::PRECEDENCE_LEVEL_EXCLUSIVE_OR;
				break;
			case token::SINGLE_AND:
				return c_precedence_level::PRECEDENCE_LEVEL_SINGLE_AND;
				break;
			case token::GREATER_THAN:
			case token::GREATER_THAN_OR_EQUAL_TO:
			case token::LESS_THAN:
			case token::LESS_THAN_OR_EQUAL_TO:
				return c_precedence_level::PRECEDENCE_LEVEL_RELATIONAL;
				break;
			case token::PLUS:
			case token::MINUS:
				return c_precedence_level::PRECEDENCE_LEVEL_ADDITIVE;
				break;
			case token::MODULUS:
			case token::STAR:
			case token::DIVIDE:
				return c_precedence_level::PRECDENCE_LEVEL_MULTIPLICATIVE;
				break;
			default:
				return c_precedence_level::PRECEDENCE_LEVEL_UNKNOWN;
				break;
			}
			SEMANTICS_ASSERT(true && "this should be unreachable");
		}
	}
}