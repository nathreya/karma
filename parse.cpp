#include "stdafx.h"
#include "parse.h"

namespace __karma {
	namespace __parse {

		string const semicolon_missing_error_message = "Error: expected a semicolon or a comma after the declaration.\nRegion given here for reference:";
		string const declarator_missing_error_message = "Error: expected a declarator after a declaration of a fundamental type.\nRegion given here for reference:";

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

		//NOTE: need to add parse_program function to populate parser's scope

		shared_ptr<c_c_declaration> parse_external_declaration(shared_ptr<c_parser> parser) {
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			switch (tok->get_id()) {
			case token::TYPEDEF: {
				int save = parser->pos;
				parser->pos++;
				shared_ptr<c_c_declaration> c_c_decl = parse_declaration(parser);
				shared_ptr<c_typedef> tdef;
				tdef->complete_node = true;
				tdef->error_node = false;
				tdef->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_TYPEDEF;
				tdef->source_begin_pos = save;
				tdef->source_end_pos = parser->pos;
				tdef->typedef_declaration = c_c_decl;
				shared_ptr<c_c_declaration> ret = static_pointer_cast<c_c_declaration>(tdef);
				return ret;
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
			vector<shared_ptr<c_declaration_specifier>> declspec_list = parse_declaration_specifier_list(parser);
			if (parser->token_list[parser->pos]->get_id() == token::SEMICOLON) {
				parser->pos++;
				shared_ptr<c_declaration> decl;
				decl->complete_node = true;
				decl->declaration_specifier_list = declspec_list;
				decl->error_node = false;
				vector<shared_ptr<c_declarator>> declr_list;
				declr_list.push_back(nullptr);
				decl->init_declarator_list = declr_list;
				decl->source_begin_pos = save;
				decl->source_end_pos = parser->pos;
				shared_ptr<c_c_declaration> c_c_decl = static_pointer_cast<c_c_declaration>(decl);
				c_c_decl->complete_node = true;
				c_type_kind kind = determine_declaration_specifier_list_type_kind(declspec_list);
				if (kind == c_type_kind::TYPE_KIND_UNKNOWN) {
					parser_message(parser, parser->token_list[save]->get_file_name(), parser->token_list[save]->get_line_number(), "Error: could not recognize this declaration's type name.\nRegion given here for reference:", c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
					return nullptr;
				}
				if (kind == c_type_kind::TYPE_KIND_STRUCT_UNION_SPECIFIER) c_c_decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_STRUCT_UNION_DECLARATION;
				else if (kind == c_type_kind::TYPE_KIND_ENUM_SPECIFIER) c_c_decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_ENUM_DECLARATION;
				else if (kind == c_type_kind::TYPE_KIND_TYPEDEF) c_c_decl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_TYPEDEF;
				else parser_message(parser, parser->token_list[parser->pos - 1]->get_file_name(), parser->token_list[parser->pos - 1]->get_line_number(), declarator_missing_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				c_c_decl->error_node = false;
				c_c_decl->source_begin_pos = save;
				c_c_decl->source_end_pos = parser->pos;
				return c_c_decl;
			}
			vector<shared_ptr<c_declarator>> cdeclr = parse_declarator_list(parser);
			shared_ptr<c_c_declaration> ccdecl;
			shared_ptr<cpp_token> tok = parser->token_list[parser->pos];
			if (tok->get_id() == token::SEMICOLON) {
				parser->pos++;
				ccdecl->complete_node = true;
				ccdecl->c_declaration_kind = c_c_declaration_kind::C_C_DECLARATION_DECLARATION;
				ccdecl->error_node = false;
				ccdecl->source_begin_pos = save;
				ccdecl->source_end_pos = parser->pos;
				return ccdecl;
			}
			//Could be the start of a declaration for a K&R function, but those are not supported
			else if (tok->get_id() == token::OPEN_BRACE) {
				parser->pos++;
				
			}
			else {
				parser_message(parser, tok->get_file_name(), tok->get_line_number(), semicolon_missing_error_message, c_parser_diagnostic_kind::PARSER_DIAGNOSTIC_KIND_ERROR);
				return nullptr;
			}
		}
	}
}	
