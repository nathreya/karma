#ifndef PARSE_H
#define PARSE_H

#include "c_ast.h"
#include "preprocess.h"
#include "graphs.h"
#include "lex_utils.h"
#include "expr.h"
#include "cpp_token.h"
#include "semantics.h"
#include "traverse_c_ast.h"
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
using namespace __karma::__semantics;

using std::cout;
using std::string;
using std::vector;
using std::shared_ptr;

namespace __karma {
	namespace __parse {
		
		extern string const semicolon_missing_error_message;
		extern string const declarator_missing_error_message;
		extern string const knr_declarations_not_supported_note_message;
		extern string const only_one_declarator_for_a_function_definition_expected_error_message;
		extern string const misformed_declarator_error_message;
		extern string const missing_a_close_parenthesis_error_message;
		extern string const missing_a_declaration_specifier_warning_message;
		extern string const expected_at_least_one_parameter_before_va_args_error_message;
		extern string const misformed_or_missing_function_parameter;
		extern string const missing_a_close_bracket_error_message;
		extern string const implicit_int_note_message;
		extern string const expected_a_named_or_unnamed_with_definition_enum_error_message;
		extern string const expected_an_identifier_for_an_enumerator_error_message;
		extern string const expected_a_named_or_unnamed_with_definition_enum_error_message;
		extern string const expected_a_named_or_unnamed_with_definition_struct_union_error_message;
		extern string const ignoring_extraneous_semicolon_warning_message;
		extern string const expected_a_comma_to_separate_struct_declarators_error_message;
		extern string const expected_an_expression_after_a_comma_error_message;
		extern string const expected_colon_for_ternary_expression_error_message;
		extern string const expected_an_identifier_for_a_structure_error_message = "Error: expected an identifier to refer to.\nRegion given here for reference:";

		extern shared_ptr<c_scope> global_typedef_scope;

		void parser_assert(char* message, char* file, int line);
		bool init_c_parser(shared_ptr<c_parser> parser, vector<shared_ptr<cpp_token>> tokl);
		shared_ptr<c_c_declaration> parse_external_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_c_declaration> parse_declaration_or_function_definition(shared_ptr<c_parser> parser);
		//an empty declaration specifier list defaults to 'int'
		vector<shared_ptr<c_declaration_specifier>> parse_declaration_specifier_list(shared_ptr<c_parser> parser, bool check_one_declaration_specifier);
		vector<shared_ptr<c_declarator>> parse_declarator_list(shared_ptr<c_parser> parser, bool can_omit_identifier);
		shared_ptr<c_declarator> parse_declarator(shared_ptr<c_parser> parser, bool can_omit_identifier);
		shared_ptr<c_statement> parse_compound_statement(shared_ptr<c_parser> parser);
		shared_ptr<c_declarator> parse_declarator_2(shared_ptr<c_parser> parser, shared_ptr<c_declarator> declarator, bool can_omit_identifier);

		//handles abstract-declarator's too.
		shared_ptr<c_declarator> parse_direct_declarator(shared_ptr<c_parser> parser, shared_ptr<c_declarator> declarator, bool can_omit_identifier);
		
		vector<shared_ptr<c_declaration_specifier>> parse_type_qualifier_list(shared_ptr<c_parser> parser, bool check_one_type_qualifier);
		shared_ptr<c_declarator> parse_potential_parenthesized_declarator(shared_ptr<c_parser> parser, shared_ptr<c_declarator> declarator, bool can_omit_identifier);
		shared_ptr<c_derived_declarator> parse_function_declarator(shared_ptr<c_parser> parser);
		
		//supports [] and ['expression']
		shared_ptr<c_derived_declarator> parse_array_declarator(shared_ptr<c_parser> parser);
		shared_ptr<c_expression> parse_expression(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state);
		shared_ptr<c_derived_declarator> parse_parameter_list(shared_ptr<c_parser> parser);
		shared_ptr<c_declaration_specifier> parse_struct_union_specifier(shared_ptr<c_parser> parser);
		shared_ptr<c_declaration_specifier> parse_enum_specifier(shared_ptr<c_parser> parser);
		shared_ptr<c_enum_specifier> parse_enum_specifier_body(shared_ptr<c_parser> parser, shared_ptr<c_enum_specifier> espec, shared_ptr<c_expression> ident);
		shared_ptr<c_expression> parse_constant_expression(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state);
		vector<shared_ptr<c_declaration_specifier>> parse_specifier_qualifier_list(shared_ptr<c_parser> parser, bool check_one_specifier_qualifier);
		shared_ptr<c_struct_union_specifier> parse_struct_union_specifier_body(shared_ptr<c_parser> parser, shared_ptr<c_struct_union_specifier> suspec, shared_ptr<c_expression> ident, c_struct_union_specifier_kind sukind);
		shared_ptr<c_struct_declaration> parse_struct_declaration(shared_ptr<c_parser> parser);
		shared_ptr<c_expression> parse_initializer(shared_ptr<c_parser> parser);
		shared_ptr<c_expression> parse_rhs_binary_expression(shared_ptr<c_parser> parser, shared_ptr<c_expression> lhs, c_precedence_level prec_level);
		shared_ptr<c_expression> parse_cast_unary_postfix_primary_expression(shared_ptr<c_parser> parser, bool unary_expression, c_type_cast_state type_cast_state);
		shared_ptr<c_expression> parse_assignment_expression(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state);
		shared_ptr<c_expression> parse_cast_unary_postfix_primary_expression2(shared_ptr<c_parser> parser, bool unary_expression, bool& cast_expression, c_type_cast_state type_cast_state);
		shared_ptr<c_expression> parse_parenthesized_expression(shared_ptr<c_parser> parser);
		shared_ptr<c_expression> parse_unary_expression(shared_ptr<c_parser> parser);
		shared_ptr<c_expression> parse_postfix_suffix_expression(shared_ptr<c_parser> parser, shared_ptr<c_expression> expr);
		vector<shared_ptr<c_expression>> parse_expression_list(shared_ptr<c_parser> parser, c_type_cast_state type_cast_state);
	}
}

#endif
