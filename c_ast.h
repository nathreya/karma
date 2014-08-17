#ifndef C_AST_H
#define C_AST_H

#include "cpp_token.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>

using std::cout;
using std::cin;
using std::string;
using std::vector;
using std::shared_ptr;
using std::ostringstream;
using std::pair;
using namespace __karma::__preprocess::__token;

namespace __karma {
	namespace __ast {

		struct c_type_name;

		struct c_initializer_list;

		struct c_initializer;

		enum c_c_declaration_kind {
			C_C_DECLARATION_DECLARATION = 0x0001, C_C_DECLARATION_FUNCTION_DEFINITION = 0x0002, C_C_DECLARATION_TYPEDEF = 0x0004,
			C_C_DECLARATION_STRUCT_UNION_DECLARATION = 0x0008, C_C_DECLARATION_ENUM_DECLARATION = 0x0010, C_C_DECLARATION_EMPTY = 0x0020
		};

		struct c_ast_node {
			bool complete_node;
			bool error_node;
		};

		struct c_c_declaration : public c_ast_node {
			c_c_declaration_kind kind;
		};

		enum c_expression_kind {
			EXPRESSION_COMMA_OPERATION = 0x0001, EXPRESSION_ASSIGNMENT_OPERATION = 0x0002, EXPRESSION_TERNARY_OPERATION = 0x0004,
			EXPRESSION_BINARY_OPERATION = 0x0008, EXPRESSION_CAST = 0x0010, EXPRESSION_SIZEOF_TYPE = 0x0020, EXPRESSION_SIZEOF_EXPRESSION = 0x0040,
			EXPRESSION_UNARY_OPERATION = 0x0080, EXPRESSION_COMPOUND_LITERAL = 0x0100, EXPRESSION_POSTFIX_OPERATION = 0x0200,
			EXPRESSION_MEMBER_ACCESS = 0x0400, EXPRESSION_FUNCTION_CALL = 0x0800, EXPRESSION_ARRAY_SUBSCRIPT = 0x1000, EXPRESSION_CONSTANT = 0x2000,
			EXPRESSION_IDENTIFIER = 0x4000, EXPRESSION_EMPTY = 0x8000
		};

		struct c_expression : public c_ast_node {
			c_expression_kind kind;
		};

		struct c_identifier : public c_expression {
			shared_ptr <cpp_token> token;
		};

		enum c_constant_kind {
			CONSTANT_INTEGER = 0x0001, CONSTANT_FLOATING_POINT = 0x0002, CONSTANT_CHARACTER = 0x0004, CONSTANT_WCHARACTER = 0x0008,
			CONSTANT_QUOTE = 0x0010, CONSTANT_WQUOTE = 0x0020
		};

		struct c_constant : public c_expression {
			shared_ptr <cpp_token> token;
			c_constant_kind kind;
		};

		struct c_array_subscript : public c_expression {
			shared_ptr <c_expression> array_identifier;
			shared_ptr <c_expression> subscript_expression;
		};

		struct c_function_call : public c_expression {
			vector <shared_ptr<c_expression>> argument_list;
			shared_ptr <c_expression> function_identifier;
		};

		enum c_member_access_kind {
			MEMBER_ACCESS_DOT = 0x0001, MEMBER_ACCESS_STRUCTURE_DEREFERENCE = 0x0002
		};

		struct c_member_access : public c_expression {
			shared_ptr <c_expression> identifier_expression;
			shared_ptr <c_expression> member_identifier;
			c_member_access_kind kind;
		};

		enum c_postfix_operation_kind {
			POSTFIX_INCREMENT = 0x0001, POSTFIX_DECREMENT = 0x0002
		};

		struct c_postfix_operation : public c_expression {
			shared_ptr <c_expression> expression;
			c_postfix_operation_kind kind;
		};

		enum c_compound_literal_comma_delimiter_present_kind {
			COMPOUND_LITERAL_COMMA_DELIMITER_PRESENT = 0x0001, COMPOUND_LITERAL_COMMA_DELIMITER_NOT_PRESENT = 0x0002
		};

		struct c_compound_literal : public c_expression {
			shared_ptr <c_type_name> type_name;
			shared_ptr <c_initializer_list> initializer;
			c_compound_literal_comma_delimiter_present_kind kind;
		};

		enum c_unary_operation_kind {
			UNARY_INCREMENT = 0x0001, UNARY_DECREMENT = 0x0002, UNARY_ADDRESS = 0x0004, UNARY_INDIRECTION = 0x0008, UNARY_PLUS = 0x0010,
			UNARY_MINUS = 0x0020, UNARY_CMPL = 0x0040, UNARY_NOT = 0x0080
		};

		struct c_unary_operation : public c_expression {
			c_unary_operation_kind kind;
			shared_ptr <c_expression> expression;
		};

		struct c_sizeof_expression : public c_expression {
			shared_ptr <c_expression> expression;
		};

		struct c_sizeof_type : public c_expression {
			shared_ptr <c_type_name> type_name;
		};

		struct c_cast : public c_expression {
			shared_ptr <c_type_name> type_name;
			shared_ptr <c_expression> expression;
		};

		enum c_binary_operation_kind {
			BINARY_MULTIPLY = 0x0001, BINARY_DIVIDE = 0x0002, BINARY_MODULUS = 0x0004, BINARY_PLUS = 0x0008, BINARY_MINUS = 0x0010,
			BINARY_SHIFT_RIGHT = 0x0020, BINARY_SHIFT_LEFT = 0x0040, BINARY_GREATER_THAN = 0x0080, BINARY_LESS_THAN = 0x0100,
			BINARY_GREATER_THAN_OR_EQUAL_TO = 0x0200, BINARY_LESS_THAN_OR_EQUAL_TO = 0x0400, BINARY_EQUALS = 0x0800, BINARY_NOT_EQUALS = 0x1000,
			BINARY_B_AND = 0x2000, BINARY_B_OR = 0x4000, BINARY_XOR = 0x8000, BINARY_L_AND = 0x20000, BINARY_L_OR = 0x40000
		};

		struct c_binary_operation : public c_expression {
			shared_ptr <c_expression> lhs;
			c_binary_operation_kind kind;
			shared_ptr <c_expression> rhs;
		};

		struct c_ternary_operation : public c_expression {
			shared_ptr <c_expression> test;
			shared_ptr <c_expression> true_path;
			shared_ptr <c_expression> false_path;
		};

		enum c_assignment_operation_kind {
			ASSIGNMENT_EQUALS = 0x0001, ASSIGNMENT_PLUS_EQUALS = 0x0002, ASSIGNMENT_MINUS_EQUALS = 0x0004, ASSIGNMENT_MULTIPLY_EQUALS = 0x0008,
			ASSIGNMENT_DIVIDE_EQUALS = 0x0010, ASSIGNMENT_MODULUS_EQUALS = 0x0020, ASSIGNMENT_SHIFT_RIGHT_EQUALS = 0x0040,
			ASSIGNMENT_SHIFT_LEFT_EQUALS = 0x0080, ASSIGNMENT_B_AND_EQUALS = 0x0100, ASSIGNMENT_B_OR_EQUALS = 0x0200, ASSIGNMENT_XOR_EQUALS = 0x0400
		};

		struct c_assignment_operation : public c_expression {
			shared_ptr <c_expression> lhs;
			c_assignment_operation_kind kind;
			shared_ptr <c_expression> rhs;
		};

		struct c_comma_operation : public c_expression {
			vector <shared_ptr<c_expression>> expression_list;
		};

		enum c_declaration_specifier_kind {
			DECLARATION_SPECIFIER_STORAGE_CLASS_SPECIFIER = 0x0001, DECLARATION_SPECIFIER_TYPE_SPECIFIER = 0x0002,
			DECLARATION_SPECIFIER_TYPE_QUALIFIER = 0x0004, DECLARATION_SPECIFIER_FUNCTION_SPECIFIER = 0x0008,
			DECLARATION_SPECIFIER_TYPE_SPECIFIER_STRUCT_UNION = 0x0010, DECLARATION_SPECIFIER_TYPE_SPECIFIER_ENUM = 0x0020,
			DECLARATION_SPECIFIER_TYPE_ALIAS = 0x0040, DECLARATION_SPECIFIER_EMPTY = 0x0080
		};

		struct c_declaration_specifier : public c_ast_node {
			c_declaration_specifier_kind kind;
		};

		enum c_storage_struct_specifier_kind {
			STORAGE_struct_SPECIFIER_EXTERN = 0x0001, STORAGE_struct_SPECIFIER_STATIC = 0x0002, STORAGE_struct_SPECIFIER_AUTO = 0x0004,
			STORAGE_struct_SPECIFIER_REGISTER = 0x0008
		};

		struct c_storage_struct_specifier : public c_declaration_specifier {
			c_storage_struct_specifier_kind kind;
		};

		enum c_type_specifier_kind {
			TYPE_SPECIFIER_VOID = 0x0001, TYPE_SPECIFIER_CHAR = 0x0002, TYPE_SPECIFIER_SHORT = 0x0004, TYPE_SPECIFIER_INT = 0x0008,
			TYPE_SPECIFIER_LONG = 0x0010, TYPE_SPECIFIER_FLOAT = 0x0020, TYPE_SPECIFIER_DOUBLE = 0x0040, TYPE_SPECIFIER_SIGNED = 0x0080,
			TYPE_SPECIFIER_UNSIGNED = 0x0100, TYPE_SPECIFIER__BOOL = 0x0200, TYPE_SPECIFIER__COMPLEX = 0x0400, TYPE_SPECIFIER__IMAGINARY = 0x0800,
			TYPE_SPECIFIER_EMPTY = 0x1000
		};

		struct c_type_specifier : public c_declaration_specifier {
			c_type_specifier_kind kind;
		};

		struct c_type_alias : public c_declaration_specifier {
			shared_ptr <cpp_token> identifier;
		};

		enum c_type_qualifier_kind {
			TYPE_QUALIFIER_CONST = 0x0001, TYPE_QUALIFIER_VOLATILE = 0x0002, TYPE_QUALIFIER_RESTRICT = 0x0004
		};

		struct c_type_qualifier : public c_declaration_specifier {
			c_type_qualifier_kind kind;
		};

		enum c_function_specifier_kind {
			FUNCTION_SPECIFIER_INLINE = 0x0001
		};

		struct c_function_specifier : public c_declaration_specifier {
			c_function_specifier_kind kind;
		};

		struct c_declarator;

		struct c_declaration : public c_c_declaration {
			vector <shared_ptr<c_declaration_specifier>> declaration_specifier_list;
			vector <shared_ptr<c_declarator>> init_declarator_list;
		};

		struct c_derived_declarator;

		enum c_declarator_kind {
			DECLARATOR_DECLARATOR = 0x0001, DECLARATOR_IDENTIFIER = 0x0002, DECLARATOR_UNNAMED = 0x0004
		};

		enum c_type_completeness_kind {
			TYPE_COMPLETENESS_INCOMPLETE = 0x0001, TYPE_COMPLETENESS_COMPLETE = 0x0002, TYPE_COMPLETENESS_UNKOWN = 0x0004
		};

		struct c_declarator : public c_ast_node {
			shared_ptr <c_derived_declarator> pointer;
			shared_ptr <c_declarator> declarator;
			shared_ptr <c_expression> identifier;
			vector <shared_ptr<c_derived_declarator>> derived_declarator_list;
			c_declarator_kind kind;
			shared_ptr <c_expression> initialized;
			c_type_completeness_kind kind2;
		};

		enum c_struct_union_specifier_kind {
			STRUCT_UNION_SPECIFIER_STRUCT = 0x0001, STRUCT_UNION_SPECIFIER_UNION = 0x0002
		};

		enum c_struct_union_specifier_declaration_list_present_kind {
			STRUCT_UNION_SPECIFIER_DECLARATION_LIST_PRESENT_TRUE = 0x0001, STRUCT_UNION_SPECIFIER_DECLARATION_LIST_PRESENT_FALSE = 0x0002
		};

		struct c_struct_declaration;

		struct c_struct_union_specifier : public c_declaration_specifier {
			c_struct_union_specifier_kind kind;
			shared_ptr <c_expression> identifier;
			c_struct_union_specifier_declaration_list_present_kind present_kind;
			vector <shared_ptr<c_c_declaration>> struct_declaration_list;
			bool defined;
			bool declared;
			bool referred;
		};

		struct c_struct_declaration : public c_c_declaration {
			vector <shared_ptr<c_declaration_specifier>> specifier_qualifier_list;
			vector <pair<shared_ptr<c_declarator>, shared_ptr<c_expression>> > struct_declarator_list;
		};

		struct c_enumerator;

		enum c_enum_specifier_comma_delimiter_present_kind {
			ENUM_SPECIFIER_COMMA_PRESENT_TRUE = 0x0001, ENUM_SPECIFIER_COMMA_PRESENT_FALSE = 0x0002
		};

		enum c_enum_specifier_enumeration_list_present_kind {
			ENUM_SPECIFIER_ENUMERATION_LIST_PRESENT_TRUE = 0x0001, ENUM_SPECIFIER_ENUMERATION_LIST_PRESENT_FALSE = 0x0002
		};

		struct c_enum_specifier : public c_declaration_specifier {
			shared_ptr <c_expression> identifier;
			c_enum_specifier_comma_delimiter_present_kind delimiter_kind;
			c_enum_specifier_enumeration_list_present_kind present_kind;
			vector <shared_ptr<c_enumerator>> enumerator_list;
			bool defined;
			bool declared;
			bool referred;
		};

		struct c_enumerator : public c_ast_node {
			shared_ptr <c_expression> identifier;
			shared_ptr <c_expression> expression;
		};

		struct c_array_size;

		enum c_derived_declarator_kind {
			DERIVED_DECLARATOR_POINTER = 0x0001, DERIVED_DECLARATOR_FUNCTION = 0x0002, DERIVED_DECLARATOR_ARRAY = 0x0004,
			DERIVED_DECLARATOR_UNKOWN = 0x0008
		};

		struct c_derived_declarator : public c_ast_node {
			c_derived_declarator_kind kind;
		};

		struct c_pointer : public c_ast_node {
			vector <shared_ptr<c_declaration_specifier>> type_qualifier_list;
		};

		struct c_pointer_declarator : public c_derived_declarator {
			vector <shared_ptr<c_pointer>> pointer_list;
		};

		enum c_array_declarator_kind {
			ARRAY_DECLARATOR_INITIAL_STATIC = 0x0001, ARRAY_DECLARATOR_MIDDLE_STATIC = 0x0002, ARRAY_DECLARATOR_NO_STATIC = 0x0004,
			ARRAY_DECLARATOR_STAR = 0x0008
		};

		struct c_array_declarator : public c_derived_declarator {
			vector <shared_ptr<c_declaration_specifier>> type_qualifier_list;
			shared_ptr <c_array_size> array_size;
			c_array_declarator_kind kind;
		};

		enum c_function_declarator_parameters_kind {
			FUNCTION_DECLARATOR_PARAMETERS_NEW_STYLE = 0x0001, FUNCTION_DECLARATOR_PARAMETERS_OLD_STYLE = 0x0002,
			FUNCTION_DECLARATOR_PARAMETERS_NONE = 0x0004
		};

		enum c_function_declarator_va_args_present_kind {
			FUNCTION_DECLARATOR_VA_ARGS_PRESENT = 0x0001, FUNCTION_DECLARATOR_VA_ARGS_NOT_PRESENT = 0x0002
		};

		struct c_function_declarator : public c_derived_declarator {
			vector <shared_ptr<c_c_declaration>> new_style_parameter_list;
			vector <shared_ptr<c_expression>> old_style_parameter_list;
			c_function_declarator_parameters_kind kind;
			c_function_declarator_va_args_present_kind kind2;
		};

		struct c_array_size : public c_ast_node {
			shared_ptr <c_expression> expression;
		};

		struct c_type_name : public c_ast_node {
			vector <shared_ptr<c_declaration_specifier>> declaration_specifier_list;
			shared_ptr <c_declarator> declarator;
		};

		enum c_initializer_comma_delimeter_present_kind {
			INITIALIZER_COMMA_DELIMETER_PRESENT = 0x001, INITIALIZER_COMMA_DELIMETER_NOT_PRESENT = 0x002
		};

		struct c_initializer_list;

		struct c_designator;

		struct c_initializer : public c_ast_node {
			shared_ptr <c_expression> expression;
			shared_ptr <c_initializer_list> initializer_list;
			c_initializer_comma_delimeter_present_kind kind;
		};

		struct c_initializer_list {
			vector <pair<vector<shared_ptr<c_designator>> , shared_ptr<c_initializer>> > desig_init_list;
			public:
			c_initializer_list(vector <pair<vector<shared_ptr<c_designator>> , shared_ptr<c_initializer>> > list);
			~ c_initializer_list();
			vector <pair<vector<shared_ptr<c_designator>> , shared_ptr<c_initializer>> > get_c_desig_init_list() const;
		};

		enum c_designator_kind {
			DESIGNATOR_ARRAY = 0x0001, DESIGNATOR_MEMBER = 0x0002
		};

		struct c_designator : public c_ast_node {
			shared_ptr <c_expression> expression;
			shared_ptr <c_expression> identifier;
			c_designator_kind kind;
		};

		enum c_statement_kind {
			STATEMENT_LABEL = 0x0001, STATEMENT_CASE = 0x0002, STATEMENT_DEFAULT = 0x0004, STATEMENT_EXPRESSION = 0x0008, STATEMENT_COMPOUND = 0x0010,
			STATEMENT_SWITCH = 0x0020, STATEMENT_IF = 0x0040, STATEMENT_WHILE = 0x0080, STATEMENT_FOR = 0x0100, STATEMENT_GOTO = 0x0200,
			STATEMENT_CONTINUE = 0x0400, STATEMENT_BREAK = 0x0800, STATEMENT_RETURN = 0x1000, STATEMENT_EMPTY = 0x2000
		};

		struct c_block_item;

		struct c_statement : public c_ast_node {
			c_statement_kind kind;
			int position;
		};

		struct c_label : public c_statement {
			shared_ptr <c_expression> identifier;
			vector <shared_ptr<c_block_item>> block_item_list;
		};

		struct c_case : public c_statement {
			shared_ptr <c_expression> expression;
			vector <shared_ptr<c_block_item>> block_item_list;
		};

		struct c_default : public c_statement {
			vector <shared_ptr<c_block_item>> block_item_list;
		};

		struct c_expression_statement : public c_statement {
			shared_ptr <c_expression> expression;
		};

		struct c_compound_statement : public c_statement {
			vector <shared_ptr<c_block_item>> block_item_list;
		};

		enum c_if_kind {
			IF_ELSE_STATEMENT = 0x0001, IF_NO_ELSE_STATEMENT = 0x0002
		};

		struct c_if : public c_statement {
			c_if_kind kind;
			shared_ptr <c_expression> expression;
			shared_ptr <c_block_item> block_item1;
			shared_ptr <c_block_item> block_item2;
		};

		struct c_switch : public c_statement {
			shared_ptr <c_expression> expression;
			shared_ptr <c_block_item> block;
		};

		enum c_while_kind {
			WHILE_WHILE = 0x0001, WHILE_DO_WHILE = 0x0002
		};

		struct c_while : public c_statement {
			c_while_kind kind;
			shared_ptr <c_expression> expression;
			shared_ptr <c_block_item> statement;
		};

		enum c_for_initialization_kind {
			FOR_INITIALIZATION_DECLARATION = 0x0001, FOR_INITIALIZATION_EXPRESSION = 0x0002
		};

		struct c_for : public c_statement {
			c_for_initialization_kind kind;
			shared_ptr <c_expression> expression;
			shared_ptr <c_c_declaration> declaration;
			shared_ptr <c_expression> expression2;
			shared_ptr <c_expression> expression3;
			shared_ptr <c_block_item> block;
		};

		struct c_goto : public c_statement {
			shared_ptr <c_expression> identifier;
		};

		struct c_continue : public c_statement {
		};

		struct c_break : public c_statement {
		};

		struct c_return : public c_statement {
			shared_ptr <c_expression> expression;
		};

		enum c_block_item_kind {
			BLOCK_ITEM_STATEMENT = 0x0001, BLOCK_ITEM_DECLARATION = 0x0002, BLOCK_ITEM_EMPTY = 0x0004
		};

		struct c_block_item : public c_ast_node {
			c_block_item_kind kind;
			shared_ptr <c_statement> statement;
			shared_ptr <c_c_declaration> declaration;
		};

		struct c_function_definition : public c_c_declaration {
			vector <shared_ptr<c_declaration_specifier>> declaration_specifier_list;
			shared_ptr <c_declarator> declarator;
			vector <shared_ptr<c_c_declaration>> parameter_list;
			shared_ptr <c_statement> statement;
		};

		struct c_translation_unit : public c_ast_node {
			vector <shared_ptr<c_c_declaration>> C_declaration_list;
		};

		struct c_typedef : public c_c_declaration {
			shared_ptr <c_c_declaration> typedef_declaration;
		};

		enum c_scope_kind {
			SCOPE_FUNCTION = 0x0001, SCOPE_LOOP = 0x0002, SCOPE_CONDITIONAL = 0x0004, SCOPE_SWITCH = 0x0008, SCOPE_STRUCT_UNION = 0x0010,
			SCOPE_EMPTY = 0x0020
		};

		struct c_scope;

		enum c_symbol_kind {
			C_SYMBOL_DECLARATION = 0x0001, C_SYMBOL_FUNCTION_DEFINITION = 0x0002, C_SYMBOL_STRUCT_UNION = 0x0004, C_SYMBOL_ENUM = 0x0008,
			C_SYMBOL_TYPEDEF = 0x0010, C_SYMBOL_EMPTY = 0x0020
		};

		enum c_declaration_type_kind {
			C_DECLARATION_TYPE_DECLARED = 0x0001, C_DECLARATION_TYPE_REFERRED = 0x0002
		};

		struct c_symbol : public c_ast_node {
		};

		struct c_declaration_symbol : public c_symbol {
			vector <shared_ptr<c_declaration_specifier>> declaration_specifier_list;
			shared_ptr <cpp_token> identifier;
			c_type_completeness_kind type_kind;
			vector <shared_ptr<c_derived_declarator>> derived_declarator_list;
			shared_ptr <c_c_declaration> source_declaration;
			c_c_declaration_kind declaration_kind;
		};

		struct c_function_definition_symbol : public c_symbol {
			vector <shared_ptr<c_declaration_specifier>> declaration_specifier_list;
			shared_ptr <cpp_token> identifier;
			vector <shared_ptr<c_derived_declarator>> derived_declarator_list;
			vector <shared_ptr<c_symbol>> parameters;
			shared_ptr <c_statement> function_body;
		};

		struct c_struct_union_symbol : public c_symbol {
			vector <shared_ptr<c_declaration_specifier>> declaration_specifier_list;
			c_struct_union_specifier_kind struct_union_specifier_kind;
			shared_ptr <c_scope> struct_union_scope;
			shared_ptr <cpp_token> identifier;
			c_struct_union_specifier_declaration_list_present_kind struct_union_specifier_declaration_list_present_kind;
		};

		struct c_enum_symbol : public c_symbol {
			shared_ptr <cpp_token> identifier;
			vector <shared_ptr<cpp_token>> identifier_list;
			vector <shared_ptr<c_expression>> expression_list;
			c_enum_specifier_comma_delimiter_present_kind enum_specifier_comma_delimiter_present_kind;
			c_enum_specifier_enumeration_list_present_kind enum_specifier_enumeration_list_present_kind;
		};

		struct c_typedef_symbol : public c_symbol {
			shared_ptr <c_symbol> declaration_symbol;
		};

		struct c_scope : public c_ast_node {
			c_scope_kind scope_kind;
			vector <shared_ptr<c_symbol>> declaration_list;
			shared_ptr <c_scope> parent_scope;
		};

		enum c_type_kind {
							
		};
	}
}

#endif