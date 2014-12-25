#include "stdafx.h"
#include "parse.h"

namespace __karma {
	namespace __parse {

		string const semicolon_missing_error_message = "Error: expected a semicolon or a comma after the declaration.\nRegion given here for reference:";
		string const declarator_missing_error_message = "Error: expected a declarator after a declaration of a fundamental type.\nRegion given here for reference:";
		string const knr_declarations_not_supported_note_message = "Note: KNR declarations (ie. 'void foo(a) int a; {}') are note supported.";
		string const only_one_declarator_for_a_function_definition_expected_error_message = "Error: expected only one declarator for a function definition.\nRegion given here for reference:";
		string const expected_a_function_declarator_error_message = "Error: expected a function declarator.\nRegion given here for reference:";
		string const misformed_declarator_error_message = "Error: misformed declarator found.\nRegion given here for reference:";
		string const missing_a_close_parenthesis_error_message = "Error: missing a ')'.\nRegion given here for reference:";
		string const missing_a_declaration_specifier_warning_message = "Warning: missing a declaration specifier: defaulting to 'int'.\nRegion given here for reference:";
		string const expected_at_least_one_parameter_before_va_args_error_message = "Error: expected at least one parameter before an ellipsis ('...').\nRegion given here for reference:";
		string const misformed_or_missing_function_parameter = "Error: misformed or missing function parameter.\nRegion given here for reference:";
		string const missing_a_close_bracket_error_message = "Error: missing a ']'.\nRegion given here for reference:";
		string const misformed_array_declarator_error_message = "Error: misformed array declarator.\nRegion give here or reference:";
		string const implicit_int_note_message = "Note: Implicit 'int' assumed here.\nRegion given here for reference:";
		string const enum_body_empty_warning_message = "Warning: empty enum-body in C is technically not allowed: continuing.\nRegion given here for reference";
		string const expected_an_identifier_for_an_enumerator_error_message = "Error: expected an identifier for an enumerator.\nRegion given here for reference:";
		string const expected_a_comma_to_separate_enumerators_error_message = "Error: expected a ',' to separate enumerators.\nRegion given here for reference";
		string const expected_a_named_or_unnamed_with_definition_struct_union_error_message = "Error: expected a named or unnamed with definition struct or union.\nRegion given here for reference:";
		string const expected_a_named_or_unnamed_with_definition_enum_error_message = "Error: expected a named or unnamed with definition enum.\nRegion given here for reference:";
		string const ignoring_extraneous_semicolon_warning_message = "Warning: extraneous ';': ignoring.\nRegion given here for reference:";
		string const expected_a_comma_to_separate_struct_declarators_error_message = "Error: expected a comma to separate struct-declarator's.\nRegion given here for reference:";
		string const expected_an_expression_after_a_comma_error_message = "Error: expected an expression after a comma.\nRegion given here for reference:";
		string const expected_colon_for_ternary_expression_error_message = "Error: expected a colon for a ternary expression.\nRegion given here for reference:";
		string const expected_an_expression_error_message = "Error: expected an expression.\nRegion given here for reference:";
		string const expected_an_identifier_for_a_structure_error_message = "Error: expected an identifier to refer to.\nRegion given here for reference:";
		string const expected_a_comma_to_separate_expressions_error_message = "Error: expected a comma to separated expression.\nRegion given here for reference:";

		shared_ptr<c_scope> global_typedef_scope = nullptr;

#define PARSER_ASSERT(EXPRESSION) if(!(EXPRESSION)) parser_assert(#EXPRESSION, __FILE__, __LINE__)

		void parser_assert(char* message, char* file, int line) {
			cerr << "Parser error(" << file << "," << line << "): " << message << "\n";
			std::terminate();
		}

		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl) {
			parser->complete_node = false;
			parser->error_node = false;
			parser->global_scope = init_global_c_scope();
			parser->pos = 0;
			tokl.push_back(make_shared<cpp_token>("???", 0, token("!!EOF!!")));
			parser->token_list = tokl;
			parser->source_begin_pos = 0;
			parser->source_end_pos = tokl.size();
			parser->global_scope->source_end_pos = tokl.size();
			parser->temporary_pos = 0;
			global_typedef_scope = init_global_c_scope();
			return true;
		}

		//NOTE: need to add parse_program(...) function to keep running parse_external_declaration(...) until the file has been completed.

		shared_ptr<c_c_declaration> parse_external_declaration(shared_ptr<c_parser> parser) {
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			parser->temporary_pos = parser->pos;
			switch (tok->get_id()) {
			case token::TYPEDEF: {
				shared_ptr<c_c_declaration> c_c_decl = parse_declaration(parser);
				global_typedef_scope->declaration_list.push_back(c_c_decl);
				return c_c_decl;
			}
			default: {
				int save = parser->pos;
				shared_ptr<c_c_declaration> ccd = parse_declaration_or_function_definition(parser);
				return ccd;
			}
			};
			return nullptr;
		}

		shared_ptr<c_c_declaration> parse_declaration_or_function_definition(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			vector<shared_ptr<c_declaration_specifier>> declspec_list = parse_declaration_specifier_list(parser, false);
			if (parser->token_list[parser->pos]->get_id() == token::SEMICOLON) {
				parser->pos++;
				shared_ptr<c_declaration> decl = make_shared<c_declaration>();
				decl->complete_node = true;
				decl->declaration_specifier_list = declspec_list;
				if (declspec_list.size() == 1 && declspec_list[0] == nullptr)
					decl->error_node = true;
				else
					decl->error_node = false;
				vector<shared_ptr<c_declarator>> declr_list;
				declr_list.push_back(nullptr);
				decl->init_declarator_list = declr_list;
				decl->source_begin_pos = save;
				decl->source_end_pos = parser->pos;
				decl->complete_node = true;
				c_type_kind kind = determine_declaration_specifier_list_type_kind(declspec_list);
				PARSER_ASSERT(kind != c_type_kind::TYPE_KIND_UNKNOWN && "c_type_kind should not be unknown.");
				if (kind == c_type_kind::TYPE_KIND_STRUCT_UNION_SPECIFIER) decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_STRUCT_UNION_DECLARATION;
				else if (kind == c_type_kind::TYPE_KIND_ENUM_SPECIFIER) decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_ENUM_DECLARATION;
				else if (kind == c_type_kind::TYPE_KIND_TYPEDEF) decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_TYPEDEF;
				else parser_message(parser, parser->token_list[parser->pos - 1]->get_file_name(), parser->token_list[parser->pos - 1]->get_line_number(), declarator_missing_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				shared_ptr<c_c_declaration> c_c_decl = static_pointer_cast<c_c_declaration>(decl);
				if (decl->c_declaration_kind == c_c_declaration_kind::C_C_DECLARATION_TYPEDEF)
					global_typedef_scope->declaration_list.push_back(c_c_decl);
				return c_c_decl;
			}
			vector<shared_ptr<c_declarator>> cdeclr = parse_declarator_list(parser, false);
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() == token::SEMICOLON) {
				parser->pos++;
				shared_ptr<c_declaration> decl = make_shared<c_declaration>();
				decl->complete_node = true;
				decl->declaration_specifier_list = declspec_list;
				c_type_kind kind = determine_declaration_specifier_list_type_kind(declspec_list);
				if (kind == c_type_kind::TYPE_KIND_TYPEDEF)
					decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_TYPEDEF;
				else
					decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_DECLARATION;
				if ((cdeclr.size() == 1 && cdeclr[0] == nullptr) || (declspec_list.size() == 1 && declspec_list[0] == nullptr))
					decl->error_node = true;
				else
					decl->error_node = false;
				decl->init_declarator_list = cdeclr;
				decl->source_begin_pos = save;
				decl->source_end_pos = parser->pos;
				shared_ptr<c_c_declaration> ccdecl = static_pointer_cast<c_c_declaration>(decl);
				if (decl->c_declaration_kind == c_c_declaration_kind::C_C_DECLARATION_TYPEDEF)
					global_typedef_scope->declaration_list.push_back(ccdecl);
				return ccdecl;
			}
			//Could be the start of a declaration for a KNR declaration list, but those are not supported
			else if (tok->get_id() == token::OPEN_BRACE) {
				shared_ptr<c_statement> stmt = parse_compound_statement(parser);
				shared_ptr<c_function_definition> cfunc = make_shared<c_function_definition>();
				cfunc->complete_node = true;
				cfunc->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_DECLARATION;
				cfunc->declaration_specifier_list = declspec_list;
				if (cdeclr.size() != 1) parser_message(parser, tok->get_file_name(), tok->get_line_number(), only_one_declarator_for_a_function_definition_expected_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				shared_ptr<c_declarator> fdeclr = cdeclr[0];
				if (fdeclr->initialized != nullptr || fdeclr->derived_declarator_list[fdeclr->derived_declarator_list.size() - 1]->derived_declarator_kind != c_derived_declarator_kind::DERIVED_DECLARATOR_FUNCTION) parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_a_function_declarator_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				cfunc->declarator = fdeclr;
				if ((cdeclr.size() == 1 && cdeclr[0] == nullptr) || (declspec_list.size() == 1 && declspec_list[0] == nullptr) ||
					stmt == nullptr)
					cfunc->error_node = true;
				else
					cfunc->error_node = false;
				shared_ptr<c_function_declarator> func_declr = static_pointer_cast<c_function_declarator>(fdeclr->derived_declarator_list[fdeclr->derived_declarator_list.size() - 1]);
				cfunc->parameter_list = func_declr->new_style_parameter_list;
				cfunc->source_begin_pos = save;
				cfunc->source_end_pos = parser->pos;
				cfunc->statement = stmt;
				shared_ptr<c_c_declaration> ccdecl = static_pointer_cast<c_function_definition>(cfunc);
				return ccdecl;
			}
			else {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), semicolon_missing_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), knr_declarations_not_supported_note_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_NOTE);
				return nullptr;
			}
			return nullptr;
		}

		vector<shared_ptr<c_declarator>> parse_declarator_list(shared_ptr<c_parser> parser, bool can_omit_identifier) {
			vector<shared_ptr<c_declarator>> declr_list;
			shared_ptr<c_declarator> declr = parse_declarator(parser, can_omit_identifier);
			if (declr == nullptr) {
				declr_list.clear();
				declr_list.push_back(nullptr);
				parser_message(parser, parser->token_list[parser->pos]->get_file_name(), parser->token_list[parser->pos]->get_line_number(), misformed_declarator_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return declr_list;
			}
			declr_list.push_back(declr);
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			while (tok->get_id() == token::COMMA) {
				parser->pos++;
				shared_ptr<c_declarator> declr = parse_declarator(parser, can_omit_identifier);
				tok = parser->token_list[parser->pos];
				if (tok->get_id() == token::EQUALS) {
					parser->pos++;
					shared_ptr<c_expression> expr = parse_initializer(parser);
					declr->initialized = expr;
				}
				declr_list.push_back(declr);
				tok = parser->token_list[parser->pos];
			}
			tok = parser->token_list[parser->pos];
			if (tok->get_id() != token::SEMICOLON) {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), misformed_declarator_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				declr_list.clear();
				declr_list.push_back(nullptr);
				return declr_list;
			}
			return declr_list;
		}

		shared_ptr<c_declarator> parse_declarator(shared_ptr<c_parser> parser, bool can_omit_identifier) {
			int save = parser->pos;
			shared_ptr<c_pointer_declarator> ptr_declr = make_shared<c_pointer_declarator>();
			ptr_declr->complete_node = false;
			ptr_declr->derived_declarator_kind = c_derived_declarator_kind::DERIVED_DECLARATOR_POINTER;
			ptr_declr->error_node = false;
			ptr_declr->pointer_list = vector<shared_ptr<c_pointer>>();
			ptr_declr->source_begin_pos = parser_unset_source_pos;
			ptr_declr->source_end_pos = parser_unset_source_pos;
			shared_ptr<c_derived_declarator> pddeclr = static_pointer_cast<c_derived_declarator>(ptr_declr);
			shared_ptr<c_declarator> declr = make_shared<c_declarator>();
			declr->complete_node = false;
			declr->declarator = nullptr;
			declr->declarator_kind = c_declarator_kind::DECLARATOR_UNKNOWN;
			declr->derived_declarator_list = vector<shared_ptr<c_derived_declarator>>();
			declr->error_node = false;
			declr->identifier = nullptr;
			declr->initialized = nullptr;
			declr->pointer_declarator = pddeclr;
			declr->declarator_type_kind = c_declarator_type_kind::DECLARATOR_TYPE_UNKNOWN;
			shared_ptr<c_declarator> declarator = parse_declarator_2(parser, declr, can_omit_identifier);
			if (declarator == nullptr)
				return nullptr;
			declarator->complete_node = true;
			declarator->source_begin_pos = save;
			declarator->source_end_pos = parser->pos;
			return declarator;
		}

		shared_ptr<c_declarator> parse_declarator_2(shared_ptr<c_parser> parser, shared_ptr<c_declarator> declarator, bool can_omit_identifier) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() == token::STAR) {
				int save1 = parser->pos;
				parser->pos++;
				vector<shared_ptr<c_declaration_specifier>> declspec_list = parse_type_qualifier_list(parser,false);
				shared_ptr<c_pointer> ptr = make_shared<c_pointer>();
				if (declspec_list.size() == 1 && declspec_list[0] == nullptr)
					ptr->error_node = true;
				else
					ptr->error_node = false;
				PARSER_ASSERT(declarator->pointer_declarator->derived_declarator_kind != c_derived_declarator_kind::DERIVED_DECLARATOR_POINTER && "expected a pointer declarator from c_declarator::pointer_declarator.");
				shared_ptr<c_pointer_declarator> ptr_declr = static_pointer_cast<c_pointer_declarator>(declarator->pointer_declarator);
				ptr->complete_node = true;
				ptr->source_begin_pos = save1;
				ptr->source_end_pos = parser->pos;
				ptr->type_qualifier_list = declspec_list;
				ptr_declr->pointer_list.push_back(ptr);
				if (ptr_declr->source_begin_pos == parser_unset_source_pos)
					ptr_declr->source_begin_pos = save1;
				declarator->pointer_declarator = static_pointer_cast<c_derived_declarator>(ptr_declr);
				return parse_declarator_2(parser, declarator, can_omit_identifier);
			}
			else {
				int save1 = parser->pos;
				PARSER_ASSERT(declarator->pointer_declarator->derived_declarator_kind != c_derived_declarator_kind::DERIVED_DECLARATOR_POINTER && "expected a pointer declarator from c_declarator::pointer_declarator.");
				shared_ptr<c_pointer_declarator> ptr_declr = static_pointer_cast<c_pointer_declarator>(declarator->pointer_declarator);
				if (ptr_declr->source_begin_pos != parser_unset_source_pos) {
					ptr_declr->source_end_pos = save1;
					declarator->pointer_declarator = static_pointer_cast<c_pointer_declarator>(ptr_declr);
				}
				return parse_direct_declarator(parser, declarator, can_omit_identifier);
			}
		}

		shared_ptr<c_declarator> parse_direct_declarator(shared_ptr<c_parser> parser, shared_ptr<c_declarator> declarator, bool can_omit_identifier) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() == token::IDENTIFIER) {
				parser->pos++;
				shared_ptr<c_identifier> ident = make_shared<c_identifier>();
				ident->complete_node = true;
				ident->error_node = false;
				ident->expression_kind = c_expression_kind::EXPRESSION_IDENTIFIER;
				ident->source_begin_pos = save;
				ident->source_end_pos = parser->pos;
				ident->token = tok;
				shared_ptr<c_expression> expr = static_pointer_cast<c_expression>(ident);
				declarator->identifier = expr;
				declarator->declarator_kind = c_declarator_kind::DECLARATOR_IDENTIFIER;
				declarator->declarator = nullptr;
				declarator->declarator_type_kind = c_declarator_type_kind::DECLARATOR_TYPE_DIRECT;
			}
			if(tok->get_id() == token::OPEN_PARENTHESIS) {
				//sets all of the fields for the c_declarator struct. 
				declarator = parse_potential_parenthesized_declarator(parser, declarator, can_omit_identifier);
				if (declarator == nullptr)
					return nullptr;
			}
			while (true) {
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				if (tok->get_id() == token::OPEN_PARENTHESIS) {
					shared_ptr<c_derived_declarator> ddeclr = parse_function_declarator(parser);
					declarator->derived_declarator_list.push_back(ddeclr);
				}
				else if (tok->get_id() == token::OPEN_BRACKET) {
					shared_ptr<c_derived_declarator> ddeclr = parse_array_declarator(parser);
					declarator->derived_declarator_list.push_back(ddeclr);
				}
				else break;
			}
			return declarator;
		}

		shared_ptr<c_declarator> parse_potential_parenthesized_declarator(shared_ptr<c_parser> parser, shared_ptr<c_declarator> declarator, bool can_omit_identifier) {
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::OPEN_PARENTHESIS && "Expected a '(' to parse a parenthesized declarator.");
			bool parenthesized_declarator = false;
			tok = parser->token_list[parser->pos + 1];
			if (!can_omit_identifier) parenthesized_declarator = true;
			else if (tok->get_id() == token::CLOSE_PARENTHESIS || parse_declaration_specifier_list(parser, true)[0] != nullptr)
				parenthesized_declarator = false;
			else parenthesized_declarator = true;

			if (parenthesized_declarator) {
				parser->pos++;
				shared_ptr<c_declarator> declr = parse_declarator(parser, can_omit_identifier);
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				if (tok->get_id() != token::CLOSE_PARENTHESIS) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_close_parenthesis_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					return nullptr;
				}
				parser->pos++;
				declarator->declarator = declr;
				declarator->declarator_kind = c_declarator_kind::DECLARATOR_DECLARATOR;
				declarator->identifier = nullptr;
				declarator->declarator_type_kind = c_declarator_type_kind::DECLARATOR_TYPE_DIRECT;
				return declarator;
			}

			declarator->declarator_kind = c_declarator_kind::DECLARATOR_ABSTRACT;
			declarator->identifier = nullptr;
			declarator->declarator = nullptr;
			declarator->declarator_type_kind = c_declarator_type_kind::DECLARATOR_TYPE_ABSTRACT;
			shared_ptr<c_derived_declarator> func_declr = parse_function_declarator(parser);
			declarator->derived_declarator_list.push_back(func_declr);
			return declarator;
		}

		shared_ptr<c_derived_declarator> parse_function_declarator(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::OPEN_PARENTHESIS && "expected a '(' to start a function declarator.");
			parser->pos++;
			if (tok->get_id() == token::CLOSE_PARENTHESIS) {
				parser->pos++;
				shared_ptr<c_function_declarator> ddeclr = make_shared<c_function_declarator>();
				ddeclr->complete_node = true;
				ddeclr->error_node = false;
				ddeclr->derived_declarator_kind = c_derived_declarator_kind::DERIVED_DECLARATOR_FUNCTION;
				ddeclr->function_declarator_parameters_kind = c_function_declarator_parameters_kind::FUNCTION_DECLARATOR_PARAMETERS_NONE;
				ddeclr->function_declarator_va_args_present_kind = c_function_declarator_va_args_present_kind::FUNCTION_DECLARATOR_VA_ARGS_NOT_PRESENT;
				ddeclr->new_style_parameter_list = vector<shared_ptr<c_c_declaration>>();
				vector<shared_ptr<c_expression>> ident_list;
				ident_list.push_back(nullptr);
				ddeclr->old_style_parameter_list = ident_list;
				ddeclr->source_begin_pos = save;
				ddeclr->source_end_pos = parser->pos;
				shared_ptr<c_derived_declarator> fddeclr = static_pointer_cast<c_derived_declarator>(ddeclr);
				return fddeclr;
			}
			shared_ptr<c_derived_declarator> ret = parse_parameter_list(parser);
			return ret;
		}

		shared_ptr<c_derived_declarator> parse_parameter_list(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			shared_ptr<c_function_declarator> ddeclr = make_shared<c_function_declarator>();
			vector<shared_ptr<c_c_declaration>> parm_list;
			c_function_declarator_va_args_present_kind kind = c_function_declarator_va_args_present_kind::FUNCTION_DECLARATOR_VA_ARGS_NOT_PRESENT;
			auto temporary_parser_increment = [&]() -> bool { parser->pos++; return true; };
			do {
				int save1 = parser->pos;
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				shared_ptr<cpp_token> look_ahead = parser->token_list[parser->pos + 1];
				if (tok->get_id() == token::VA_ARGS && parm_list.size() == 0) {
					parser->pos++;
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_at_least_one_parameter_before_va_args_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					parm_list.clear();
					parm_list.push_back(nullptr);
					break;
				}
				else if (tok->get_id() == token::VA_ARGS && look_ahead->get_id() == token::CLOSE_PARENTHESIS) {
					parser->pos++;
					kind = c_function_declarator_va_args_present_kind::FUNCTION_DECLARATOR_VA_ARGS_PRESENT;
					break;
				}
				vector<shared_ptr<c_declaration_specifier>> declspec_list = parse_declaration_specifier_list(parser, false);
				shared_ptr<c_declarator> declr = parse_declarator(parser, true);
				if ((declspec_list.size() == 1 && declspec_list[0] == nullptr) || declr == nullptr) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), misformed_or_missing_function_parameter, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					parm_list.clear();
					parm_list.push_back(nullptr);
					break;
				}
				shared_ptr<c_declaration> decl = make_shared<c_declaration>();
				decl->complete_node = true;
				decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_DECLARATION;
				decl->declaration_specifier_list = declspec_list;
				vector<shared_ptr<c_declarator>> parm_decl_list;
				parm_decl_list.push_back(declr);
				decl->error_node = false;
				decl->init_declarator_list = parm_decl_list;
				decl->source_begin_pos = save1;
				decl->source_end_pos = parser->pos;
				shared_ptr<c_c_declaration> ccdecl = static_pointer_cast<c_c_declaration>(decl);
				parm_list.push_back(ccdecl);
			} while (parser->token_list[parser->pos]->get_id() == token::COMMA && temporary_parser_increment());
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() != token::CLOSE_PARENTHESIS) {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_close_parenthesis_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
			parser->pos++;
			ddeclr->complete_node = true;
			ddeclr->derived_declarator_kind = c_derived_declarator_kind::DERIVED_DECLARATOR_FUNCTION;
			ddeclr->error_node = false;
			ddeclr->function_declarator_parameters_kind = c_function_declarator_parameters_kind::FUNCTION_DECLARATOR_PARAMETERS_NEW_STYLE;
			ddeclr->function_declarator_va_args_present_kind = kind;
			ddeclr->new_style_parameter_list = parm_list;
			vector<shared_ptr<c_expression>> expr_list;
			expr_list.push_back(nullptr);
			ddeclr->old_style_parameter_list = expr_list;
			ddeclr->source_begin_pos = save;
			ddeclr->source_end_pos = parser->pos;
			shared_ptr<c_derived_declarator> ret = static_pointer_cast<c_derived_declarator>(ddeclr);
			return ret;
		}

		shared_ptr<c_derived_declarator> parse_array_declarator(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::OPEN_BRACKET && "expected a '[' to start an array declarator");
			parser->pos++;
			tok = parser->token_list[parser->pos];
			if (tok->get_id() == token::CLOSE_BRACKET) {
				parser->pos++;
				shared_ptr<c_array_declarator> adeclr = make_shared<c_array_declarator>();
				adeclr->array_declarator_kind = c_array_declarator_kind::ARRAY_DECLARATOR_NO_STATIC;
				adeclr->array_size = nullptr;
				adeclr->complete_node = true;
				adeclr->derived_declarator_kind = c_derived_declarator_kind::DERIVED_DECLARATOR_ARRAY;
				adeclr->error_node = false;
				adeclr->source_begin_pos = save;
				adeclr->source_end_pos = parser->pos;
				vector<shared_ptr<c_declaration_specifier>> tqual_list;
				tqual_list.push_back(nullptr);
				adeclr->type_qualifier_list = tqual_list;
				shared_ptr<c_derived_declarator> ret = static_pointer_cast<c_derived_declarator>(adeclr);
				return ret;
			}
			else {
				int save1 = parser->pos;
				parser->pos++;
				shared_ptr<c_array_declarator> adeclr = make_shared<c_array_declarator>();
				adeclr->array_declarator_kind = c_array_declarator_kind::ARRAY_DECLARATOR_NO_STATIC;
				shared_ptr<c_array_size> asize = make_shared<c_array_size>();
				asize->complete_node = true;
				asize->error_node = false;
				shared_ptr<c_expression> expr = parse_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
				asize->expression = expr;
				asize->source_begin_pos = save1;
				asize->source_end_pos = parser->pos;
				adeclr->array_size = asize;
				adeclr->complete_node = true;
				adeclr->derived_declarator_kind = c_derived_declarator_kind::DERIVED_DECLARATOR_ARRAY;
				adeclr->error_node = false;
				adeclr->source_begin_pos = save;
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				if (tok->get_id() == token::CLOSE_BRACKET) {
					parser->pos++;
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_close_bracket_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					return nullptr;
				}
				adeclr->source_end_pos = parser->pos;
				vector<shared_ptr<c_declaration_specifier>> declspec_list;
				declspec_list.push_back(nullptr);
				adeclr->type_qualifier_list = declspec_list;
				shared_ptr<c_derived_declarator> ret = static_pointer_cast<c_derived_declarator>(adeclr);
				return ret;
			}
			tok = parser->token_list[parser->pos];
			parser_message(parser, tok->get_file_name(), tok->get_line_number(), misformed_array_declarator_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
			return nullptr;
		}

		shared_ptr<c_c_declaration> parse_declaration(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			vector<shared_ptr<c_declaration_specifier>> declspec_list = parse_declaration_specifier_list(parser, false);
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() == token::SEMICOLON) {
				parser->pos++;
				shared_ptr<c_declaration> decl = make_shared<c_declaration>();
				decl->complete_node = true;
				c_type_kind kind = determine_declaration_specifier_list_type_kind(declspec_list);
				PARSER_ASSERT(kind != c_type_kind::TYPE_KIND_UNKNOWN && "c_type_kind should always be known");
				if (kind == c_type_kind::TYPE_KIND_STRUCT_UNION_SPECIFIER) decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_STRUCT_UNION_DECLARATION;
				else if (kind == c_type_kind::TYPE_KIND_ENUM_SPECIFIER) decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_ENUM_DECLARATION;
				else if (kind == c_type_kind::TYPE_KIND_TYPEDEF) decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_TYPEDEF;
				else parser_message(parser, tok->get_file_name(), tok->get_line_number(), declarator_missing_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				decl->declaration_specifier_list = declspec_list;
				vector<shared_ptr<c_declarator>> declr_list;
				declr_list.push_back(nullptr);
				decl->error_node = false;
				decl->init_declarator_list = declr_list;
				decl->source_begin_pos = save;
				decl->source_end_pos = parser->pos;
				shared_ptr<c_c_declaration> ret = static_pointer_cast<c_c_declaration>(decl);
				return ret;
			}
			vector<shared_ptr<c_declarator>> declr_list = parse_declarator_list(parser, false);
			tok = parser->token_list[parser->pos];
			if (tok->get_id() != token::SEMICOLON) {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), semicolon_missing_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
			parser->pos++;
			shared_ptr<c_declaration> decl = make_shared<c_declaration>();
			decl->complete_node = true;
			c_type_kind kind = determine_declaration_specifier_list_type_kind(declspec_list);
			if (kind == c_type_kind::TYPE_KIND_TYPEDEF)
				decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_TYPEDEF;
			else
				decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_DECLARATION;
			decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_DECLARATION;
			decl->declaration_specifier_list = declspec_list;
			decl->error_node = false;
			decl->init_declarator_list = declr_list;
			decl->source_begin_pos = save;
			decl->source_end_pos = parser->pos;
			shared_ptr<c_c_declaration> ret = static_pointer_cast<c_c_declaration>(decl);
			return ret;
		}

		vector<shared_ptr<c_declaration_specifier>> parse_declaration_specifier_list(shared_ptr<c_parser> parser, bool check_one_declaration_specifier) {
			int save = parser->pos;
			vector<shared_ptr<c_declaration_specifier>> declspec_list;
			if (check_one_declaration_specifier) {
				//return an unfilled c_declaration_specifier struct (dummy struct for checking against nullptr)
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				switch (tok->get_id()) {
				case token::TYPEDEF:
				case token::EXTERN:
				case token::STATIC:
				case token::AUTO:
				case token::REGISTER:
				case token::_VOID:
				case token::CHAR:
				case token::SHORT:
				case token::INT:
				case token::LONG:
				case token::FLOAT:
				case token::DOUBLE:
				case token::SIGNED:
				case token::UNSIGNED:
				case token::_BOOL:
				case token::_COMPLEX:
				case token::STRUCT:
				case token::UNION:
				case token::_CONST:
				case token::RESTRICT:
				case token::VOLATILE:
				case token::_INLINE_: {
					shared_ptr<c_declaration_specifier> declspec = make_shared<c_declaration_specifier>();
					declspec_list.push_back(declspec);
					return declspec_list;
				}
					break;
				case token::IDENTIFIER: {
					string ident = tok->get_qualifier();
					vector<shared_ptr<c_c_declaration>> decl_list = global_typedef_scope->declaration_list;
					for (int i = 0; i < decl_list.size(); i++) {
						vector<shared_ptr<c_expression>> expr_list = query_identifier(decl_list[i]);
						for (int j = 0; j < expr_list.size(); j++) {
							if (expr_list[j] == nullptr) break;
							string comp = static_pointer_cast<c_identifier>(expr_list[j])->token->get_qualifier();
							if (ident == comp) {
								shared_ptr<c_declaration_specifier> declspec = make_shared<c_declaration_specifier>();
								declspec_list.push_back(declspec);
								return declspec_list;
							}
						}
					}
					declspec_list.push_back(nullptr);
					return declspec_list;
				}
					break;
				default:
					declspec_list.push_back(nullptr);
					return declspec_list;
					break;
				}
				PARSER_ASSERT(true && "this should be unreachable");
			}
			else {
				bool passed_type_specifier = false;
				while (true) {
					int save1 = parser->pos;
					shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
					switch (tok->get_id()) {
					default: {
					declaration_specifier_list_parsing_completed:
						if (declspec_list.size() == 0 || !passed_type_specifier) {
							parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_declaration_specifier_warning_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_WARNING);
							parser_message(parser, tok->get_file_name(), tok->get_line_number(), implicit_int_note_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_NOTE);
						}
						return declspec_list;
					}
					case token::TYPEDEF:
					case token::EXTERN:
					case token::STATIC:
					case token::AUTO:
					case token::REGISTER: {
						parser->pos++;
						shared_ptr<c_storage_class_specifier> scs = make_shared<c_storage_class_specifier>();
						scs->complete_node = true;
						scs->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_STORAGE_CLASS_SPECIFIER;
						scs->error_node = false;
						scs->source_begin_pos = save1;
						scs->source_end_pos = parser->pos;
						if (tok->get_id() == token::TYPEDEF)
							scs->storage_class_specifier_kind = c_storage_class_specifier_kind::STORAGE_CLASS_SPECIFIER_TYPEDEF;
						else if (tok->get_id() == token::EXTERN)
							scs->storage_class_specifier_kind = c_storage_class_specifier_kind::STORAGE_CLASS_SPECIFIER_EXTERN;
						else if (tok->get_id() == token::STATIC)
							scs->storage_class_specifier_kind = c_storage_class_specifier_kind::STORAGE_CLASS_SPECIFIER_STATIC;
						else if (tok->get_id() == token::AUTO)
							scs->storage_class_specifier_kind = c_storage_class_specifier_kind::STORAGE_CLASS_SPECIFIER_AUTO;
						else
							scs->storage_class_specifier_kind = c_storage_class_specifier_kind::STORAGE_CLASS_SPECIFIER_REGISTER;
						shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(scs);
						declspec_list.push_back(declspec);
					}
						break; 
					case token::_VOID:
					case token::CHAR:
					case token::SHORT:
					case token::INT:
					case token::LONG:
					case token::FLOAT:
					case token::DOUBLE:
					case token::SIGNED:
					case token::UNSIGNED:
					case token::_BOOL:
					case token::_COMPLEX: {
						parser->pos++;
						shared_ptr<c_type_specifier> typespec = make_shared<c_type_specifier>();
						typespec->complete_node = true;
						typespec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER;
						typespec->error_node = false;
						typespec->source_begin_pos = save1;
						typespec->source_end_pos = parser->pos;
						if (tok->get_id() == token::_VOID)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_VOID;
						else if (tok->get_id() == token::CHAR)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_CHAR;
						else if (tok->get_id() == token::SHORT)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_SHORT;
						else if (tok->get_id() == token::INT)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_INT;
						else if (tok->get_id() == token::FLOAT)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_FLOAT;
						else if (tok->get_id() == token::LONG)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_LONG;
						else if (tok->get_id() == token::DOUBLE)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_DOUBLE;
						else if (tok->get_id() == token::SIGNED)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_SIGNED;
						else if (tok->get_id() == token::UNSIGNED)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_UNSIGNED;
						else if (tok->get_id() == token::_BOOL)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER__BOOL;
						else
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER__COMPLEX;
						shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(typespec);
						declspec_list.push_back(declspec);
						passed_type_specifier = true;
					}
						break;
					case token::_CONST:
					case token::RESTRICT:
					case token::VOLATILE: {
						parser->pos++;
						shared_ptr<c_type_qualifier> typequal = make_shared<c_type_qualifier>();
						typequal->complete_node = true;
						typequal->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_QUALIFIER;
						typequal->error_node = false;
						typequal->source_begin_pos = save1;
						typequal->source_end_pos = parser->pos;
						if (tok->get_id() == token::_CONST)
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_CONST;
						else if (tok->get_id() == token::RESTRICT)
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_RESTRICT;
						else
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_VOLATILE;
						shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(typequal);
						declspec_list.push_back(declspec);
					}
						break;
					case token::_INLINE_: {
						parser->pos++;
						shared_ptr<c_function_specifier> funcspec = make_shared<c_function_specifier>();
						funcspec->complete_node = true;
						funcspec->function_specifier_kind = c_function_specifier_kind::FUNCTION_SPECIFIER_INLINE;
						funcspec->error_node = false;
						funcspec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_FUNCTION_SPECIFIER;
						funcspec->source_begin_pos = save1;
						funcspec->source_end_pos = parser->pos;
						shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(funcspec);
						declspec_list.push_back(declspec);
					}
						break;
					case token::STRUCT:
					case token::UNION: {
						shared_ptr<c_declaration_specifier> declspec = parse_struct_union_specifier(parser);
						if (declspec == nullptr) {
							declspec_list.clear();
							declspec_list.push_back(nullptr);
							goto declaration_specifier_list_parsing_completed;
						}
						declspec_list.push_back(declspec);
						passed_type_specifier = true;
					}
						break;
					case token::ENUM: {
						shared_ptr<c_declaration_specifier> declspec = parse_enum_specifier(parser);
						if (declspec == nullptr) {
							declspec_list.clear();
							declspec_list.push_back(nullptr);
							goto declaration_specifier_list_parsing_completed;
						}
						declspec_list.push_back(declspec);
						passed_type_specifier = true;
					}
						break;
					case token::IDENTIFIER: {
						int save1 = parser->pos;
						if (passed_type_specifier)
							goto declaration_specifier_list_parsing_completed;
						bool complete = false;
						string ident = tok->get_qualifier();
						vector<shared_ptr<c_c_declaration>> decl_list = global_typedef_scope->declaration_list;
						for (int i = 0; i < decl_list.size(); i++) {
							vector<shared_ptr<c_expression>> expr_list = query_identifier(decl_list[i]);
							for (int j = 0; j < expr_list.size(); j++) {
								if (expr_list[j] == nullptr) break;
								PARSER_ASSERT(expr_list[j]->expression_kind == c_expression_kind::EXPRESSION_IDENTIFIER && "'expr_list' must contain only identifiers.");
								string comp = static_pointer_cast<c_identifier>(expr_list[j])->token->get_qualifier();
								if (ident == comp) {
									shared_ptr<c_type_alias> ta = make_shared<c_type_alias>();
									ta->complete_node = true;
									ta->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_ALIAS;
									ta->error_node = false;
									ta->identifer = expr_list[j];
									ta->source_begin_pos = save1;
									parser->pos++;
									ta->source_end_pos = parser->pos;
									shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(ta);
									declspec_list.push_back(declspec);
									complete = true;
									passed_type_specifier = true;
									break;
								}
							}
						}
						if (complete);
						else {
							goto declaration_specifier_list_parsing_completed;
						}
					}
					}
				}
			}
			PARSER_ASSERT(true && "this should be unreachable");
		}

		vector<shared_ptr<c_declaration_specifier>> parse_type_qualifier_list(shared_ptr<c_parser> parser, bool check_one_type_qualifier) {
			int save = parser->pos;
			vector<shared_ptr<c_declaration_specifier>> typequal_list;
			if (check_one_type_qualifier) {
				//return an unfilled c_declaration_specifier struct (dummy struct for checking against nullptr)
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				switch (tok->get_id()) {
				case token::_CONST:
				case token::RESTRICT:
				case token::VOLATILE: {
					shared_ptr<c_declaration_specifier> declspec = make_shared<c_declaration_specifier>();
					typequal_list.push_back(declspec);
					return typequal_list;
				}
					break;
				default: {
					typequal_list.push_back(nullptr);
					return typequal_list;
				}
					break;
				}
				PARSER_ASSERT(true && "this should be unreachable.");
			}
			else {
				while (true) {
					int save1 = parser->pos;
					shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
					switch (tok->get_id()) {
					case token::_CONST:
					case token::RESTRICT:
					case token::VOLATILE: {
						parser->pos++;
						shared_ptr<c_type_qualifier> typequal = make_shared<c_type_qualifier>();
						typequal->complete_node = true;
						typequal->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_QUALIFIER;
						typequal->error_node = false;
						typequal->source_begin_pos = save1;
						typequal->source_end_pos = parser->pos;
						if (tok->get_id() == token::_CONST)
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_CONST;
						else if (tok->get_id() == token::RESTRICT)
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_RESTRICT;
						else
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_VOLATILE;
						shared_ptr<c_declaration_specifier> ret = static_pointer_cast<c_declaration_specifier>(typequal);
						typequal_list.push_back(ret);
					}
						break;
					default:
						return typequal_list;
						break;
					}
				}
			}
			PARSER_ASSERT(true && "this should be unreachable");
		}

		shared_ptr<c_declaration_specifier> parse_enum_specifier(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::ENUM && "expected an 'enum' token to start parsing an enum-specifier.");
			parser->pos++;
			tok = parser->token_list[parser->pos];
			shared_ptr<c_expression> expr = make_shared<c_expression>();
			shared_ptr<c_enum_specifier> espec = make_shared<c_enum_specifier>();
			if (tok->get_id() == token::OPEN_BRACE) {
				expr = nullptr;
			}
			else if (tok->get_id() == token::IDENTIFIER) {
				int save1 = parser->pos;
				parser->pos++;
				shared_ptr<c_identifier> ident = make_shared<c_identifier>();
				ident->complete_node = true;
				ident->error_node = false;
				ident->expression_kind = c_expression_kind::EXPRESSION_IDENTIFIER;
				ident->source_begin_pos = save1;
				ident->source_end_pos = parser->pos;
				ident->token = tok;
				expr = static_pointer_cast<c_expression>(ident);
			}
			else {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_a_named_or_unnamed_with_definition_enum_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
			tok = parser->token_list[parser->pos];
			if (tok->get_id() != token::OPEN_BRACE) {
				espec->complete_node = true;
				espec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER_ENUM;
				espec->delimiter_kind = c_enum_specifier_comma_delimiter_present_kind::ENUM_SPECIFIER_COMMA_PRESENT_FALSE;
				vector<shared_ptr<c_enumerator>> enum_list;
				espec->enumerator_list = enum_list;
				espec->error_node = false;
				espec->identifier = expr;
				espec->present_kind = c_enum_specifier_enumeration_list_present_kind::ENUM_SPECIFIER_ENUMERATION_LIST_PRESENT_FALSE;
				espec->source_begin_pos = save;
				espec->source_end_pos = parser->pos;
				return espec;
			}
			PARSER_ASSERT(tok->get_id() == token::OPEN_BRACE && "this must be a '{'.");
			espec = nullptr;
			espec = make_shared<c_enum_specifier>();
			shared_ptr<c_enum_specifier> finished = parse_enum_specifier_body(parser, espec, expr);
			if (espec == nullptr) return nullptr;
			finished->source_begin_pos = save;
			finished->source_end_pos = parser->pos;
			shared_ptr<c_declaration_specifier> ret = static_pointer_cast<c_declaration_specifier>(finished);
			return ret;
		}

		shared_ptr<c_enum_specifier> parse_enum_specifier_body(shared_ptr<c_parser> parser, shared_ptr<c_enum_specifier> espec, shared_ptr<c_expression> ident) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::OPEN_BRACE && "expected a '{' to start an enum-body.");
			parser->pos++;
			if (tok->get_id() == token::CLOSE_BRACE) {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), enum_body_empty_warning_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_WARNING);
				parser->pos++;
				espec->complete_node = true;
				espec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER_ENUM;
				espec->delimiter_kind = c_enum_specifier_comma_delimiter_present_kind::ENUM_SPECIFIER_COMMA_PRESENT_FALSE;
				vector<shared_ptr<c_enumerator>> enum_list;
				espec->enumerator_list = enum_list;
				espec->error_node = false;
				espec->identifier = ident;
				espec->present_kind = c_enum_specifier_enumeration_list_present_kind::ENUM_SPECIFIER_ENUMERATION_LIST_PRESENT_FALSE;
				espec->source_begin_pos = save;
				espec->source_end_pos = parser->pos;
				return espec;
			}
			vector<shared_ptr<c_enumerator>> enum_list;
			tok = parser->token_list[parser->pos];
			init_new_scope(global_typedef_scope, c_scope_kind::SCOPE_ENUM);
			c_enum_specifier_comma_delimiter_present_kind present_kind = c_enum_specifier_comma_delimiter_present_kind::ENUM_SPECIFIER_COMMA_PRESENT_FALSE;
			while (tok->get_id() != token::CLOSE_BRACE) {
				int save1 = parser->pos;
				shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
				if (tok2->get_id() != token::IDENTIFIER) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_an_identifier_for_an_enumerator_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					enum_list.clear();
					enum_list.push_back(nullptr);
					break;
				}
				PARSER_ASSERT(tok->get_id() == token::IDENTIFIER && "tok must be a token::IDENTIFER.");
				shared_ptr<c_identifier> id = make_shared<c_identifier>();
				parser->pos++;
				id->complete_node = true;
				id->error_node = false;
				id->expression_kind = c_expression_kind::EXPRESSION_IDENTIFIER;
				id->source_begin_pos = save1;
				id->source_end_pos = parser->pos;
				id->token = tok;
				shared_ptr<c_expression> idnt = static_pointer_cast<c_expression>(id);
				shared_ptr<c_enumerator> enumrtr = make_shared<c_enumerator>();
				shared_ptr<c_expression> expr = make_shared<c_expression>();
				tok2 = parser->token_list[parser->pos];
				if (tok2->get_id() == token::EQUALS) {
					parser->pos++;
					expr = parse_constant_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
				}
				else
					expr = nullptr;

				enumrtr->complete_node = true;
				enumrtr->error_node = false;
				enumrtr->expression = expr;
				enumrtr->identifier = idnt;
				enumrtr->source_begin_pos = save1;
				enumrtr->source_end_pos = parser->pos;
				enum_list.push_back(enumrtr);
				tok2 = parser->token_list[parser->pos];
				shared_ptr<cpp_token> look_ahead = parser->token_list[parser->pos + 1];
				if (tok2->get_id() == token::CLOSE_BRACE);
				else if (tok2->get_id() == token::COMMA && look_ahead->get_id() == token::CLOSE_BRACE) {
					present_kind = c_enum_specifier_comma_delimiter_present_kind::ENUM_SPECIFIER_COMMA_PRESENT_TRUE;
					parser->pos++;
				}
				if (tok2->get_id() == token::COMMA)
					parser->pos++;
				else if (tok2->get_id() == token::IDENTIFIER) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_a_comma_to_separate_enumerators_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					enum_list.clear();
					enum_list.push_back(nullptr);
					break;
				}
			}
			remove_last_scope(global_typedef_scope);
			espec->complete_node = true;
			espec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER_ENUM;
			espec->delimiter_kind = present_kind;
			espec->enumerator_list = enum_list;
			if (enum_list.size() == 1 && enum_list[0] == nullptr)
				return nullptr;
			else
				espec->error_node = false;
			espec->identifier = ident;
			espec->present_kind = c_enum_specifier_enumeration_list_present_kind::ENUM_SPECIFIER_ENUMERATION_LIST_PRESENT_TRUE;
			//source_begin_pos and source_end_pos are set in the main function
			return espec;
		}

		vector<shared_ptr<c_declaration_specifier>> parse_specifier_qualifier_list(shared_ptr<c_parser> parser, bool check_one_specifier_qualifier) {
			int save = parser->pos;
			vector<shared_ptr<c_declaration_specifier>> specqual_list;
			if (check_one_specifier_qualifier) {
				//return an unfilled c_declaration_specifier struct (dummy struct for checking against nullptr)
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				switch (tok->get_id()) {
				case token::_VOID:
				case token::CHAR:
				case token::SHORT:
				case token::INT:
				case token::LONG:
				case token::FLOAT:
				case token::DOUBLE:
				case token::SIGNED:
				case token::UNSIGNED:
				case token::_BOOL:
				case token::_COMPLEX:
				case token::STRUCT:
				case token::UNION:
				case token::_CONST:
				case token::RESTRICT:
				case token::VOLATILE:
				case token::IDENTIFIER: {
					string ident = tok->get_qualifier();
					vector<shared_ptr<c_c_declaration>> decl_list = global_typedef_scope->declaration_list;
					for (int i = 0; i < decl_list.size(); i++) {
						vector<shared_ptr<c_expression>> expr_list = query_identifier(decl_list[i]);
						for (int j = 0; j < expr_list.size(); j++) {
							if (expr_list[j] == nullptr) break;
							string comp = static_pointer_cast<c_identifier>(expr_list[j])->token->get_qualifier();
							if (ident == comp) {
								shared_ptr<c_declaration_specifier> declspec = make_shared<c_declaration_specifier>();
								specqual_list.push_back(declspec);
								return specqual_list;
							}
						}
					}
					specqual_list.push_back(nullptr);
					return specqual_list;
				}
					break;
				default:
					specqual_list.push_back(nullptr);
					return specqual_list;
					break;
				}
				PARSER_ASSERT(true && "this should be unreachable");
			}
			else {
				bool passed_type_specifier = false;
				while (true) {
					int save1 = parser->pos;
					shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
					switch (tok->get_id()) {
					default: {
					specifier_qualifier_list_parsing_completed:
						if (specqual_list.size() == 0 || !passed_type_specifier) {
							parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_declaration_specifier_warning_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_WARNING);
							parser_message(parser, tok->get_file_name(), tok->get_line_number(), implicit_int_note_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_NOTE);
						}
						return specqual_list;
					}
					case token::_VOID:
					case token::CHAR:
					case token::SHORT:
					case token::INT:
					case token::LONG:
					case token::FLOAT:
					case token::DOUBLE:
					case token::SIGNED:
					case token::UNSIGNED:
					case token::_BOOL:
					case token::_COMPLEX: {
						parser->pos++;
						shared_ptr<c_type_specifier> typespec = make_shared<c_type_specifier>();
						typespec->complete_node = true;
						typespec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER;
						typespec->error_node = false;
						typespec->source_begin_pos = save1;
						typespec->source_end_pos = parser->pos;
						if (tok->get_id() == token::_VOID)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_VOID;
						else if (tok->get_id() == token::CHAR)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_CHAR;
						else if (tok->get_id() == token::SHORT)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_SHORT;
						else if (tok->get_id() == token::INT)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_INT;
						else if (tok->get_id() == token::FLOAT)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_FLOAT;
						else if (tok->get_id() == token::LONG)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_LONG;
						else if (tok->get_id() == token::DOUBLE)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_DOUBLE;
						else if (tok->get_id() == token::SIGNED)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_SIGNED;
						else if (tok->get_id() == token::UNSIGNED)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER_UNSIGNED;
						else if (tok->get_id() == token::_BOOL)
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER__BOOL;
						else
							typespec->type_specifier_kind = c_type_specifier_kind::TYPE_SPECIFIER__COMPLEX;
						shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(typespec);
						specqual_list.push_back(declspec);
						passed_type_specifier = true;
					}
						break;
					case token::_CONST:
					case token::RESTRICT:
					case token::VOLATILE: {
						parser->pos++;
						shared_ptr<c_type_qualifier> typequal = make_shared<c_type_qualifier>();
						typequal->complete_node = true;
						typequal->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_QUALIFIER;
						typequal->error_node = false;
						typequal->source_begin_pos = save1;
						typequal->source_end_pos = parser->pos;
						if (tok->get_id() == token::_CONST)
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_CONST;
						else if (tok->get_id() == token::RESTRICT)
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_RESTRICT;
						else
							typequal->type_qualifier_kind = c_type_qualifier_kind::TYPE_QUALIFIER_VOLATILE;
						shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(typequal);
						specqual_list.push_back(declspec);
					}
						break;
					case token::STRUCT:
					case token::UNION: {
						shared_ptr<c_declaration_specifier> declspec = parse_struct_union_specifier(parser);
						if (declspec == nullptr) {
							specqual_list.clear();
							specqual_list.push_back(nullptr);
							goto specifier_qualifier_list_parsing_completed;
						}
						specqual_list.push_back(declspec);
						passed_type_specifier = true;
					}
						break;
					case token::ENUM: {
						shared_ptr<c_declaration_specifier> declspec = parse_enum_specifier(parser);
						if (declspec == nullptr) {
							specqual_list.clear();
							specqual_list.push_back(nullptr);
							goto specifier_qualifier_list_parsing_completed;
						}
						specqual_list.push_back(declspec);
						passed_type_specifier = true;
					}
						break;
					case token::IDENTIFIER: {
						int save1 = parser->pos;
						if (passed_type_specifier)
							goto specifier_qualifier_list_parsing_completed;
						bool complete = false;
						string ident = tok->get_qualifier();
						vector<shared_ptr<c_c_declaration>> decl_list = global_typedef_scope->declaration_list;
						for (int i = 0; i < decl_list.size(); i++) {
							vector<shared_ptr<c_expression>> expr_list = query_identifier(decl_list[i]);
							for (int j = 0; j < expr_list.size(); j++) {
								if (expr_list[j] == nullptr) break;
								PARSER_ASSERT(expr_list[j]->expression_kind == c_expression_kind::EXPRESSION_IDENTIFIER && "'expr_list' must contain only identifiers.");
								string comp = static_pointer_cast<c_identifier>(expr_list[j])->token->get_qualifier();
								if (ident == comp) {
									shared_ptr<c_type_alias> ta = make_shared<c_type_alias>();
									ta->complete_node = true;
									ta->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_ALIAS;
									ta->error_node = false;
									ta->identifer = expr_list[j];
									ta->source_begin_pos = save1;
									parser->pos++;
									ta->source_end_pos = parser->pos;
									shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(ta);
									specqual_list.push_back(declspec);
									complete = true;
									passed_type_specifier = true;
									break;
								}
							}
						}
						if (complete);
						else {
							goto specifier_qualifier_list_parsing_completed;
						}
					}
					}
				}
			}
			PARSER_ASSERT(true && "this should be unreachable");
		}

		shared_ptr<c_declaration_specifier> parse_struct_union_specifier(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT((tok->get_id() == token::STRUCT || tok->get_id() == token::UNION) && "this should be a token::STRUCT or a token::UNION.");
			c_struct_union_specifier_kind sukind = c_struct_union_specifier_kind::STRUCT_UNION_SPECIFIER_STRUCT;
			if (tok->get_id() == token::UNION)
				sukind = c_struct_union_specifier_kind::STRUCT_UNION_SPECIFIER_UNION;
			shared_ptr<c_struct_union_specifier> suspec = make_shared<c_struct_union_specifier>();
			parser->pos++;
			tok = parser->token_list[parser->pos];
			shared_ptr<c_expression> ident = make_shared<c_expression>();
			if (tok->get_id() == token::IDENTIFIER) {
				int save1 = parser->pos;
				parser->pos++;
				shared_ptr<c_identifier> idnt = make_shared<c_identifier>();
				idnt->complete_node = true;
				idnt->error_node = false;
				idnt->expression_kind = c_expression_kind::EXPRESSION_IDENTIFIER;
				idnt->source_begin_pos = save1;
				idnt->source_end_pos = parser->pos;
				idnt->token = tok;
				ident = static_pointer_cast<c_expression>(idnt);
			}
			else if (tok->get_id() == token::OPEN_BRACE) {
				ident = nullptr;
			}
			else {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_a_named_or_unnamed_with_definition_struct_union_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
			tok = parser->token_list[parser->pos];
			if (tok->get_id() != token::OPEN_BRACE) {
				suspec->complete_node = true;
				suspec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER_STRUCT_UNION;
				suspec->error_node = false;
				suspec->identifier = ident;
				suspec->present_kind = c_struct_union_specifier_declaration_list_present_kind::STRUCT_UNION_SPECIFIER_DECLARATION_LIST_PRESENT_FALSE;
				suspec->source_begin_pos = save;
				suspec->source_end_pos = parser->pos;
				vector<shared_ptr<c_struct_declaration>> sdecl_list;
				suspec->struct_declaration_list = sdecl_list;
				suspec->struct_union_specifier_kind = sukind;
				shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(suspec);
				return declspec;
			}
			tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::OPEN_BRACE && "all other conditions other than token::OPEN_BRACE have been tested for.");
			suspec = nullptr;
			suspec = make_shared<c_struct_union_specifier>();
			shared_ptr<c_struct_union_specifier> finished = parse_struct_union_specifier_body(parser, suspec, ident, sukind);
			finished->source_begin_pos = save;
			finished->source_end_pos = parser->pos;
			shared_ptr<c_declaration_specifier> declspec = static_pointer_cast<c_declaration_specifier>(finished);
			return declspec;
		}

		shared_ptr<c_struct_union_specifier> parse_struct_union_specifier_body(shared_ptr<c_parser> parser, shared_ptr<c_struct_union_specifier> suspec, shared_ptr<c_expression> ident, c_struct_union_specifier_kind sukind) {
			int save = parser->pos;
			init_new_scope(global_typedef_scope, c_scope_kind::SCOPE_STRUCT_UNION);
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			PARSER_ASSERT(tok->get_id() == token::OPEN_BRACE && "need a '{' to being parsing a struct-union-specifier-body.");
			vector<shared_ptr<c_struct_declaration>> sdecl_list;
			parser->pos++;
			tok = parser->token_list[parser->pos];
			while (tok->get_id() != token::CLOSE_BRACE) {
				shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
				if (tok2->get_id() == token::SEMICOLON) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), ignoring_extraneous_semicolon_warning_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_WARNING);
					parser->pos++;
				}
				shared_ptr<c_struct_declaration> sdecl = parse_struct_declaration(parser);
				if (sdecl == nullptr) {
					sdecl_list.clear();
					sdecl_list.push_back(nullptr);
					break;
				}
				sdecl_list.push_back(sdecl);
				tok = parser->token_list[parser->pos];
			}
			suspec->complete_node = true;
			suspec->declaration_specifier_kind = c_declaration_specifier_kind::DECLARATION_SPECIFIER_TYPE_SPECIFIER_STRUCT_UNION;
			if (sdecl_list.size() == 1 && sdecl_list[0] == nullptr)
				suspec->error_node = true;
			else suspec->error_node = false;
			suspec->identifier = ident;
			suspec->present_kind = c_struct_union_specifier_declaration_list_present_kind::STRUCT_UNION_SPECIFIER_DECLARATION_LIST_PRESENT_TRUE;
			suspec->struct_declaration_list = sdecl_list;
			suspec->struct_union_specifier_kind = sukind;
			//source_being_pos and source_end_pos are set by the parent function
			return suspec;
		}

		shared_ptr<c_struct_declaration> parse_struct_declaration(shared_ptr<c_parser> parser) {
			int save = parser->pos;
			vector<shared_ptr<c_declaration_specifier>> specqual_list = parse_specifier_qualifier_list(parser, false);
			shared_ptr<c_struct_declaration> sdecl = make_shared<c_struct_declaration>();
			vector<pair<shared_ptr<c_declarator>, shared_ptr<c_expression>>> decl_list;
			while (true) {
				int save1 = parser->pos;
				pair<shared_ptr<c_declarator>, shared_ptr<c_expression>> p;
				shared_ptr<c_declarator> declr = parse_declarator(parser, false);
				if (declr == nullptr) {
					decl_list.clear();
					p.first = nullptr;
					p.second = nullptr;
					decl_list.push_back(p);
					break;
				}
				p.first = declr;
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				if (tok->get_id() == token::COLON) {
					parser->pos++;
					shared_ptr<c_expression> expr = parse_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
					if (expr == nullptr) {
						decl_list.clear();
						p.first = nullptr;
						p.second = nullptr;
						decl_list.push_back(p);
						break;
					}
					p.second = expr;
				}
				else p.second = nullptr;
				decl_list.push_back(p);
				tok = parser->token_list[parser->pos];
				if (tok->get_id() == token::SEMICOLON) break;
				else if (tok->get_id() == token::COMMA);
				else
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_a_comma_to_separate_struct_declarators_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
			}
			sdecl->complete_node = true;
			sdecl->error_node = false;
			sdecl->source_begin_pos = save;
			sdecl->source_end_pos = parser->pos;
			sdecl->specifier_qualifier_list = specqual_list;
			sdecl->struct_declarator_list = decl_list;
			return sdecl;
		}

		shared_ptr<c_expression> parse_rhs_binary_expression(shared_ptr<c_parser> parser, shared_ptr<c_expression> lhs, c_precedence_level prec_level) {
			int save = parser->pos;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			c_precedence_level plev = get_binary_operator_c_precedence_level(tok);
			auto temporary_is_not_expression_start = [&]() -> bool { return tok->get_id() == token::OPEN_BRACE || tok->get_id() == token::CLOSE_BRACE || tok->get_id() == token::FOR ||
				tok->get_id() == token::WHILE || tok->get_id() == token::IF || tok->get_id() == token::ELSE || tok->get_id() == token::GOTO || parse_declaration_specifier_list(parser, true)[0] != nullptr; };
			while (true) {
				int save1 = parser->pos;
				if (plev < prec_level)
					return lhs;
				shared_ptr<cpp_token> op = parser->token_list[parser->pos];
				parser->pos++;
				tok = parser->token_list[parser->pos];
				if (temporary_is_not_expression_start()) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), expected_an_expression_after_a_comma_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					return nullptr;
				}
				shared_ptr<c_expression> expr = make_shared<c_expression>();
				if (plev == c_precedence_level::PRECEDENCE_LEVEL_CONDITIONAL) {
					expr = parse_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
					if (expr == nullptr)
						return nullptr;
					shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
					if (tok2->get_id() != token::COLON) {
						parser_message(parser, tok2->get_file_name(), tok2->get_line_number(), expected_colon_for_ternary_expression_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
						return nullptr;
					}
					parser->pos++;
				}
				shared_ptr<c_expression> rhs = parse_cast_unary_postfix_primary_expression(parser, false, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
				if (rhs == nullptr)
					return nullptr;
				c_precedence_level nplev = plev;
				plev = get_binary_operator_c_precedence_level(parser->token_list[parser->pos]);
				bool right_associative = nplev == c_precedence_level::PRECEDENCE_LEVEL_CONDITIONAL || c_precedence_level::PRECEDENCE_LEVEL_ASSIGNMENT;
				if (nplev < plev || (nplev == plev && right_associative)) {
					rhs = parse_rhs_binary_expression(parser, rhs, static_cast<c_precedence_level>(nplev + !right_associative));
					if (rhs == nullptr)
						return nullptr;
					plev = get_binary_operator_c_precedence_level(parser->token_list[parser->pos]);
				}
				PARSER_ASSERT(plev <= nplev && "this should reduce expression to an AST");
				if (lhs == nullptr)
					return nullptr;
				if (expr != nullptr) {
					shared_ptr<c_ternary_operation> t_op = make_shared<c_ternary_operation>();
					t_op->complete_node = true;
					t_op->error_node = false;
					t_op->expression_kind = c_expression_kind::EXPRESSION_TERNARY_OPERATION;
					t_op->false_path = rhs;
					t_op->source_begin_pos = save1;
					t_op->source_end_pos = parser->pos;
					t_op->test = expr;
					t_op->true_path = rhs;
					lhs = static_pointer_cast<c_expression>(t_op);
				}
				else {
					shared_ptr<c_binary_operation> bin_op = make_shared<c_binary_operation>();
					bin_op->binary_operator = op;
					bin_op->complete_node = true;
					bin_op->error_node = false;
					bin_op->expression_kind = c_expression_kind::EXPRESSION_BINARY_OPERATION;
					bin_op->lhs = lhs;
					bin_op->rhs = rhs;
					bin_op->source_begin_pos = save1;
					bin_op->source_end_pos = parser->pos;
					lhs = static_pointer_cast<c_binary_operation>(bin_op);
				}
			}
		}

		shared_ptr<c_expression> parse_assignment_expression(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state) {
			shared_ptr<c_expression> lhs = parse_cast_unary_postfix_primary_expression(parser, false, type_cast_state);
			if (lhs == nullptr) return nullptr;
			return parse_rhs_binary_expression(parser, lhs, c_precedence_level::PRECEDENCE_LEVEL_ASSIGNMENT);
		}

		shared_ptr<c_expression> parse_expression(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state) {
			shared_ptr<c_expression> lhs = parse_assignment_expression(parser, type_cast_state);
			if (lhs == nullptr) return nullptr;
			return parse_rhs_binary_expression(parser, lhs, c_precedence_level::PRECEDENCE_LEVEL_COMMA);
		}

		shared_ptr<c_expression> parse_constant_expression(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state) {
			shared_ptr<c_expression> lhs = parse_cast_unary_postfix_primary_expression(parser, false, type_cast_state);
			if (lhs == nullptr) return nullptr;
			return parse_rhs_binary_expression(parser, lhs, c_precedence_level::PRECEDENCE_LEVEL_CONDITIONAL);
		}

		shared_ptr<c_expression> parse_cast_unary_postfix_primary_expression(shared_ptr<c_parser> parser, bool unary_expression, c_type_cast_state type_cast_state) {
			bool cast_expression;
			shared_ptr<c_expression> expr = parse_cast_unary_postfix_primary_expression2(parser, unary_expression, cast_expression, type_cast_state);
			if (!cast_expression || expr == nullptr) {
				parser_message(parser, parser->token_list[parser->pos]->get_file_name(), parser->token_list[parser->pos]->get_line_number(), expected_an_expression_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
			return expr;
		}

		shared_ptr<c_expression> parse_cast_unary_postfix_primary_expression2(shared_ptr<c_parser> parser, bool unary_expression, bool& cast_expression, c_type_cast_state type_cast_state) {
			int save = parser->pos;
			shared_ptr<c_expression> expr = make_shared<c_expression>();
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			cast_expression = true;
			switch (tok->get_id()) {
			case token::OPEN_PARENTHESIS: {
				c_parenthesis_parse_option pparenopt = unary_expression ? c_parenthesis_parse_option::C_PARENTHESIS_PARSE_COMPOUND_LITERAL :
					c_parenthesis_parse_option::C_PARENTHESIS_PARSE_CAST;
				expr = parse_parenthesized_expression(parser, pparenopt);
				switch (pparenopt) {
				case c_parenthesis_parse_option::C_PARENTHESIS_PARSE_EXPRESSION:
				case c_parenthesis_parse_option::C_PARENTHESIS_PARSE_COMPOUND_LITERAL:
					break;
				case c_parenthesis_parse_option::C_PARENTHESIS_PARSE_CAST:
					return expr;
					break;
				};
			}
				break;
			case token::INTEGER:
			case token::FLOATING_POINT:
			case token::QUOTE:
			case token::WQUOTE:
			case token::CHARACTER:
			case token::WCHARACTER: {
				int save1 = parser->pos;
				parser->pos++;
				shared_ptr<c_constant> constant = make_shared<c_constant>();
				constant->complete_node = true;
				constant->error_node = false;
				constant->expression_kind = c_expression_kind::EXPRESSION_CONSTANT;
				constant->source_begin_pos = save1;
				constant->source_end_pos = parser->pos;
				constant->token = tok;
				if (tok->get_id() == token::INTEGER) constant->constant_kind = c_constant_kind::CONSTANT_INTEGER;
				else if (tok->get_id() == token::FLOATING_POINT) constant->constant_kind = c_constant_kind::CONSTANT_FLOATING_POINT;
				else if (tok->get_id() == token::QUOTE) constant->constant_kind = c_constant_kind::CONSTANT_QUOTE;
				else if (tok->get_id() == token::WQUOTE) constant->constant_kind = c_constant_kind::CONSTANT_WQUOTE;
				else if (tok->get_id() == token::CHARACTER) constant->constant_kind = c_constant_kind::CONSTANT_WCHARACTER;
				else if (tok->get_id() == token::CHARACTER) constant->constant_kind = c_constant_kind::CONSTANT_CHARACTER;
				else PARSER_ASSERT(true && "this should be unreachable");
				expr = static_pointer_cast<c_constant>(constant);
			}
				break;
			case token::IDENTIFIER: {
				int save1 = parser->pos;
				parser->pos++;
				shared_ptr<c_identifier> ident = make_shared<c_identifier>();
				ident->complete_node = true;
				ident->error_node = false;
				ident->expression_kind = c_expression_kind::EXPRESSION_IDENTIFIER;
				ident->source_begin_pos = save1;
				ident->source_end_pos = parser->pos;
				ident->token = tok;
				expr = static_pointer_cast<c_expression>(ident);
			}
				break;
			case token::PLUS:
			case token::MINUS:
			case token::CMPL:
			case token::STAR:
			case token::NOT:
			case token::SINGLE_AND:
			case token::INCREMENT:
			case token::DECREMENT: {
				int save1 = parser->pos;
				parser->pos++;
				shared_ptr<c_expression> texpr = parse_cast_unary_postfix_primary_expression(parser, unary_expression, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
				if (texpr == nullptr)
					return nullptr;
				shared_ptr<c_unary_operation> uexpr = make_shared<c_unary_operation>();
				uexpr->complete_node = true;
				uexpr->error_node = false;
				uexpr->expression = texpr;
				uexpr->expression_kind = c_expression_kind::EXPRESSION_UNARY_OPERATION;
				uexpr->source_begin_pos = save1;
				uexpr->source_end_pos = parser->pos;
				if (tok->get_id() == token::PLUS)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_PLUS;
				else if (tok->get_id() == token::MINUS)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_MINUS;
				else if (tok->get_id() == token::CMPL)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_CMPL;
				else if (tok->get_id() == token::STAR)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_INDIRECTION;
				else if (tok->get_id() == token::SINGLE_AND)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_ADDRESS;
				else if (tok->get_id() == token::INCREMENT)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_INCREMENT;
				else if (tok->get_id() == token::DECREMENT)
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_DECREMENT;
				else
					uexpr->unary_operation_kind = c_unary_operation_kind::UNARY_NOT;
				expr = static_pointer_cast<c_expression>(uexpr);
			}
				return expr;
				break;
			case token::SIZEOF: {
				bool cast_expression;
				return parse_sizeof_expression(parser, cast_expression);
			}
			default:
				return nullptr;
			};
			return parse_postfix_suffix_expression(parser, expr);
		}

		shared_ptr<c_expression> parse_postfix_suffix_expression(shared_ptr<c_parser> parser, shared_ptr<c_expression> expr) {
			while (true) {
				int save = parser->pos;
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				switch (tok->get_id()) {
				case token::IDENTIFIER:
				default:
					return expr;
				case token::OPEN_BRACKET: {
					parser->pos++;
					shared_ptr<c_expression> exp = parse_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
					shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
					if (tok2->get_id() == token::CLOSE_BRACKET) {
						parser_message(parser, tok2->get_file_name(), tok2->get_line_number(), missing_a_close_bracket_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
						return nullptr;
					}
					shared_ptr<c_array_subscript> asubscr = make_shared<c_array_subscript>();
					asubscr->array_identifier = expr;
					asubscr->complete_node = true;
					asubscr->error_node = false;
					asubscr->expression_kind = c_expression_kind::EXPRESSION_ARRAY_SUBSCRIPT;
					asubscr->source_begin_pos = save;
					asubscr->source_end_pos = parser->pos;
					asubscr->subscript_expression = exp;
					expr = static_pointer_cast<c_expression>(asubscr);
				}
					break;
				case token::OPEN_PARENTHESIS: {
					parser->pos++;
					vector<shared_ptr<c_expression>> expr_list = parse_expression_list(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
					shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
					if (tok2->get_id() != token::CLOSE_PARENTHESIS)
						return nullptr;
					parser->pos++;
					shared_ptr<c_function_call> func_call = make_shared<c_function_call>();
					func_call->argument_list = expr_list;
					func_call->complete_node = true;
					func_call->error_node = false;
					func_call->expression_kind = c_expression_kind::EXPRESSION_FUNCTION_CALL;
					func_call->function_identifier = expr;
					func_call->source_begin_pos = save;
					func_call->source_end_pos = parser->pos;
					expr = static_pointer_cast<c_expression>(func_call);
				}
					break;
				case token::DOT:
				case token::STRUCTURE_DEREFERENCE: {
					parser->pos++;
					int save1 = parser->pos;
					shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
					if (tok2->get_id() != token::IDENTIFIER) {
						parser_message(parser, tok2->get_file_name(), tok2->get_line_number(), expected_an_identifier_for_a_structure_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
						return nullptr;
					}
					parser->pos++;
					shared_ptr<c_identifier> ident = make_shared<c_identifier>();
					ident->complete_node = true;
					ident->error_node = false;
					ident->expression_kind = c_expression_kind::EXPRESSION_IDENTIFIER;
					ident->source_begin_pos = save1;
					ident->source_end_pos = parser->pos;
					ident->token = tok2;
					shared_ptr<c_expression> exp = static_pointer_cast<c_expression>(ident);
					shared_ptr<c_member_access> mem_acc = make_shared<c_member_access>();
					mem_acc->complete_node = true;
					mem_acc->error_node = false;
					mem_acc->expression_kind = c_expression_kind::EXPRESSION_MEMBER_ACCESS;
					mem_acc->identifier_expression = expr;
					if (tok->get_id() == token::STRUCTURE_DEREFERENCE)
						mem_acc->member_access_kind = c_member_access_kind::MEMBER_ACCESS_STRUCTURE_DEREFERENCE;
					else
						mem_acc->member_access_kind = c_member_access_kind::MEMBER_ACCESS_DOT;
					mem_acc->member_identifier = exp;
					mem_acc->source_begin_pos = save;
					mem_acc->source_end_pos = parser->pos;
					expr = static_pointer_cast<c_expression>(mem_acc);
				}
					break;
				case token::INCREMENT:
				case token::DECREMENT: {
					int save1 = parser->pos;
					parser->pos++;
					shared_ptr<c_postfix_operation> postfix_operation = make_shared<c_postfix_operation>();
					postfix_operation->complete_node = true;
					postfix_operation->error_node = false;
					postfix_operation->expression = expr;
					postfix_operation->expression_kind = c_expression_kind::EXPRESSION_POSTFIX_OPERATION;
					if (tok->get_id() == token::INCREMENT)
						postfix_operation->postfix_operation_kind = c_postfix_operation_kind::POSTFIX_INCREMENT;
					else
						postfix_operation->postfix_operation_kind = c_postfix_operation_kind::POSTFIX_DECREMENT;
					postfix_operation->source_begin_pos = save1;
					postfix_operation->source_end_pos = parser->pos;
					expr = static_pointer_cast<c_postfix_operation>(postfix_operation);
				}
					break;
				}
			}
			return expr;
		}

		shared_ptr<c_expression> parse_sizeof_expression(shared_ptr<c_parser> parser, bool& cast_expression) {
			int save = parser->pos;
			PARSER_ASSERT(parser->token_list[save]->get_id() == token::SIZEOF && "A sizeof expression should begin with a 'sizeof' token.");
			parser->pos++;
			int save1 = parser->pos;
			shared_ptr<c_expression> expr;
			if (parser->token_list[save1]->get_id() == token::OPEN_PARENTHESIS) {
				expr = parse_parenthesized_expression(parser, c_parenthesis_parse_option::C_PARENTHESIS_PARSE_CAST);
				expr = parse_postfix_suffix_expression(parser, expr);
			}
			else {
				cast_expression = false;
				expr = parse_cast_unary_postfix_primary_expression(parser, true, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
			}
			cast_expression = false;
			return expr;
		}

		vector<shared_ptr<c_expression>> parse_expression_list(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state) {
			vector<shared_ptr<c_expression>> expr_list;
			while (true) {
				shared_ptr<c_expression> expr = parse_assignment_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
				if (expr == nullptr) {
					parser_message(parser, parser->token_list[parser->pos]->get_file_name(), parser->token_list[parser->pos]->get_line_number(), expected_an_expression_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					expr_list.clear();
					expr_list.push_back(nullptr);
					return expr_list;
				}
				expr_list.push_back(expr);
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				if (tok->get_id() != token::COMMA) {
					parser_message(parser, parser->token_list[parser->pos]->get_file_name(), parser->token_list[parser->pos]->get_line_number(), expected_a_comma_to_separate_expressions_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					expr_list.clear();
					expr_list.push_back(nullptr);
					return expr_list;
				}
				parser->pos++;
			}
			return expr_list;
		}

		shared_ptr<c_expression> parse_parenthesized_expression(shared_ptr<c_parser> parser, c_parenthesis_parse_option parenthesis_parse_option) {
			PARSER_ASSERT(parser->token_list[parser->pos]->get_id() != token::OPEN_PARENTHESIS && "Expected a '(' to begin an open parenthesis.");
			int save = parser->pos;
			parser->pos++;
			shared_ptr<c_expression> expr = make_shared<c_expression>();
			if (parenthesis_parse_option >= c_parenthesis_parse_option::C_PARENTHESIS_PARSE_COMPOUND_LITERAL) {
				shared_ptr<c_type_name> type = parse_type_name(parser);
				shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
				if (tok->get_id() != token::CLOSE_PARENTHESIS) {
					parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_close_parenthesis_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					return nullptr;
				}
				shared_ptr<cpp_token> tok2 = parser->token_list[parser->pos];
				if (tok2->get_id() == token::OPEN_BRACE) {
					parenthesis_parse_option = c_parenthesis_parse_option::C_PARENTHESIS_PARSE_COMPOUND_LITERAL;
					return parse_compound_literal(parser);
				}
				if (parenthesis_parse_option == c_parenthesis_parse_option::C_PARENTHESIS_PARSE_CAST) {
					expr = parse_cast_unary_postfix_primary_expression(parser, false, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
					if (expr == nullptr) {
						return nullptr;
					}
					return expr;
				}
				return nullptr;
			}
			else {
				expr = parse_expression(parser, c_type_cast_state::TYPE_CAST_STATE_NOT_CAST);
				parenthesis_parse_option = c_parenthesis_parse_option::C_PARENTHESIS_PARSE_EXPRESSION;
				if (expr == nullptr)
					return nullptr;
			}
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() != token::CLOSE_PARENTHESIS) {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), missing_a_close_parenthesis_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
			return expr;
		}

		shared_ptr<c_type_name> parse_type_name(shared_ptr<c_parser> parser) {
			//user-defined for flexibility
			return nullptr;
		}

		shared_ptr<c_expression> parse_compound_literal(shared_ptr<c_parser> parser) {
			//user-defined for flexibility
			return nullptr;
		}

		shared_ptr<c_statement> parse_compound_statement(shared_ptr<c_parser> parser) {
			//user-defined for flexibility
			return nullptr;
		}

		shared_ptr<c_expression> parse_initializer(shared_ptr<c_parser> parser) {
			//user-defined for flexibility
			return nullptr;
		}
	}
}	
