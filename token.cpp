#include "stdafx.h"
#include "token.h"

namespace __karma {
	namespace __lex {
		namespace __token {

			token::token(string ident) {
				flag = false;
				suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_NONE;
				//runs through all un-differentiating tokens(operators, digits, constants, and identifiers)

				if(ident == "!!EOF!!") {
					id = token::END_OF_FILE;
					qualifier = ident;
				}

				else if(ident == _hundef) {

					id = token::HUNDEF;
					qualifier = _hundef;
				}

				//for recognizing whether a token describes a wide-string or wide-character
				else if(ident.substr(0, 9) == "@wchar_t*") {

					flag = true;
					ident.erase(0, 9);
					id = token::WQUOTE;
					qualifier = ident;
				}

				else if(ident.substr(0, 9) == "@wchar_t ") {

					flag = true;
					ident.erase(0, 9);
					id = token::WCHARACTER;
					qualifier = ident;
				}

				else if(ident == _herror) {

					id = token::HERROR;
					qualifier = _herror;
				}

				else if(ident == _hwarning) {

					id = token::HWARNING;
					qualifier = _hwarning;
				}

				else if(ident == _shift_left_equals) {

					id = token::SHIFT_LEFT_EQUALS;
					qualifier = _shift_left_equals;
				}

				else if(ident == _shift_right_equals) {

					id = token::SHIFT_RIGHT_EQUALS;
					qualifier = _shift_right_equals;
				}

				else if(string(1,ident[0]) == _double_quote) {

					id = token::QUOTE;
					qualifier = ident;
				}

				else if(string(1,ident[0]) == _single_quote) {

					id = token::CHARACTER;
					qualifier = ident;
				}

				else if(ident == _escape_character) {

					id = token::ESCAPE_CHARACTER;
					qualifier = _escape_character;
				}

				else if(ident == _plus) {

					id = token::PLUS;
					qualifier = _plus;
				}

				else if(ident == _variable_arguments) {

					id = token::VA_ARGS;
					qualifier = _variable_arguments;
				}

				else if(ident == _structure_dereference) {
					id = token::STRUCTURE_DEREFERENCE;
					qualifier = _structure_dereference;
				}

				else if(ident == _minus) {

					id = token::MINUS;
					qualifier = _minus;
				}

				else if(ident == _star) {

					id = token::STAR;
					qualifier = _star;
				}

				else if(ident == _divide) {

					id = token::DIVIDE;
					qualifier = _divide;
				}

				else if(ident == _modulus) {

					id = token::MODULUS;
					qualifier = _modulus;
				}

				else if(ident == _single_and) {

					id = token::SINGLE_AND;
					qualifier = _single_and;
				}

				else if(ident == _single_or) {

					id = token::SINGLE_OR;
					qualifier = _single_or;
				}

				else if(ident == _xor) {

					id = token::XOR;
					qualifier = _xor;
				}

				else if(ident == _cmpl) {

					id = token::CMPL;
					qualifier = _cmpl;
				}

				else if(ident == _not) {

					id = token::NOT;
					qualifier = _not;
				}

				else if(ident == _equals) {

					id = token::EQUALS;
					qualifier = _equals;
				}

				else if(ident == _shift_left) {

					id = token::SHIFT_LEFT;
					qualifier = _shift_left;
				}

				else if(ident == _shift_right) {

					id = token::SHIFT_RIGHT;
					qualifier = _shift_right;
				}

				else if(ident == _and) {

					id = token::AND;
					qualifier = _and;
				}

				else if(ident == _or) {

					id = token::OR;
					qualifier = _or;
				}

				else if(ident == _less_than) {

					id = token::LESS_THAN;
					qualifier = _less_than;
				}

				else if(ident == _greater_than) {

					id = token::GREATER_THAN;
					qualifier = _greater_than;
				}

				else if(ident == _greater_than_or_equal_to) {

					id = token::GREATER_THAN_OR_EQUAL_TO;
					qualifier = _greater_than_or_equal_to;
				}

				else if(ident == _less_than_or_equal_to) {

					id = token::LESS_THAN_OR_EQUAL_TO;
					qualifier = _less_than_or_equal_to;
				}

				else if(ident == _equality) {

					id = token::EQUALITY;
					qualifier = _equality;
				}

				else if(ident == _inequality) {

					id = token::INEQUALITY;
					qualifier = _inequality;
				}

				else if(ident == _hash) {

					id = token::HASH;
					qualifier = _hash;
				}

				else if(ident == _hdefine) {

					id = token::HDEFINE;
					qualifier = _hdefine;
				}

				else if(ident == _hpragma) {

					id = token::HPRAGMA;
					qualifier = _hpragma;
				}

				else if(ident == _hif) {

					id = token::HIF;
					qualifier = _hif;
				}

				else if(ident == _helif) {

					id = token::HELIF;
					qualifier = _helif;
				}

				else if(ident == _helse) {

					id = token::HELSE;
					qualifier = _helse;
				}

				else if(ident == _hendif) {

					id = token::HENDIF;
					qualifier = _hendif;
				}

				else if(ident == _hinclude) {

					id = token::HINCLUDE;
					qualifier = _hinclude;
				}

				else if(ident == _hline) {

					id = token::HLINE;
					qualifier = _hline;
				}

				//Microsoft only
				//else if(ident == _himport) {

				//id = token::HIMPORT;
				//qualifier = _himport;
				//}

				//else if(ident == _hpragma) {

				//id = token::HPRAGMA;
				//qualifier = _hpragma;
				//}

				else if(ident == _hifdef) {

					id = token::HIFDEF;
					qualifier = _hifdef;
				}

				else if(ident == _hifndef) {

					id = token::HIFNDEF;
					qualifier = _hifndef;
				}

				else if(ident == _paste) {

					id = token::PASTE;
					qualifier = _paste;
				}

				else if(ident == _plus_equals) {

					id = token::PLUS_EQUALS;
					qualifier = _plus_equals;
				}

				else if(ident == _minus_equals) {

					id = token::MINUS_EQUALS;
					qualifier = _minus_equals;
				}

				else if(ident == _star_equals) {

					id = token::STAR_EQUALS;
					qualifier = _star_equals;
				}

				else if(ident == _divide_equals) {

					id = token::DIVIDE_EQUALS;
					qualifier = _divide_equals;
				}

				else if(ident == _modulus_equals) {

					id = token::MODULUS_EQUALS;
					qualifier = _modulus_equals;
				}

				else if(ident == _and_equals) {

					id = token::AND_EQUALS;
					qualifier = _and_equals;
				}

				else if(ident == _or_equals) {

					id = token::OR_EQUALS;
					qualifier = _or_equals;
				}

				else if(ident == _xor_equals) {

					id = token::XOR_EQUALS;
					qualifier = _xor_equals;
				}

				else if(ident == _open_parenthesis) {

					id = token::OPEN_PARENTHESIS;
					qualifier = _open_parenthesis;
				}

				else if(ident == _close_parenthesis) {

					id = token::CLOSE_PARENTHESIS;
					qualifier = _close_parenthesis;
				}

				else if(ident == _open_bracket) {

					id = token::OPEN_BRACKET;
					qualifier = _open_bracket;
				}

				else if(ident == _close_bracket) {

					id = token::CLOSE_BRACKET;
					qualifier = _close_bracket;
				}

				else if(ident == _open_brace) {

					id = token::OPEN_BRACE;
					qualifier = _open_brace;
				}

				else if(ident == _close_brace) {

					id = token::CLOSE_BRACE;
					qualifier = _close_brace;
				}

				else if(ident == _semicolon) {

					id = token::SEMICOLON;
					qualifier = _semicolon;
				}

				else if(ident == _dot) {

					id = token::DOT;
					qualifier = _dot;
				}

				else if(ident == _comma) {

					id = token::COMMA;
					qualifier = _comma;
				}

				else if(ident == _colon) {

					id = token::COLON;
					qualifier = _colon;
				}

				else if(ident == _question_mark) {

					id = token::QUESTION_MARK;
					qualifier = _question_mark;
				}

				else if(ident == _new_line) {

					id = token::NEW_LINE;
					qualifier = _new_line;
				}

				else if(ident == _general_increment) {

					id = token::INCREMENT;
					qualifier = _general_increment;
				}

				else if(ident == _general_decrement) {

					id = token::DECREMENT;
					qualifier = _general_decrement;
				}

				else if(isdigit(ident[0]) &&((int) ident.find(_dot) < 0 &&(int) ident.find(_e) < 0 &&(int) ident.find(_E) < 0)) {
					vector<string> spl = split(ident, ' ');
					if(spl.size() == 1);
					else {
						string suffix = spl[1];
						transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
						ident = spl[0];
						if(suffix == "ll") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_LL;
						else if(suffix == "ull") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_ULL;
						else if(suffix == "l") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_L;
						else if(suffix == "d") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_D;
						else if(suffix == "f") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_F;
						else if(suffix == "u") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_U;
					}
					id = token::INTEGER;
					qualifier = ident;
				}

				else if((isdigit(ident[0]) || ident[0] == '.') &&((int) ident.find(_dot) >= 0 ||(int) ident.find(_e) >= 0 ||(int) ident.find(_E) >= 0)) {
					vector<string> spl = split(ident, ' ');
					if(spl.size() == 1);
					else {
						string suffix = spl[1];
						transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
						ident = spl[0];
						if(suffix == "ll") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_LL;
						else if(suffix == "ull") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_ULL;
						else if(suffix == "l") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_L;
						else if(suffix == "d") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_D;
						else if(suffix == "f") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_F;
						else if(suffix == "u") suffix_token_kind = c_suffix_token_kind::SUFFIX_TOKEN_U;
					}
					id = token::FLOATING_POINT;
					qualifier = ident;
				}

				else {
					id = token::IDENTIFIER;
					qualifier = ident;
				}
			}
			token::~ token() {}
			string token::get_qualifier() { return qualifier; }
			int token::get_id() { return id; }
			bool token::wide() { return flag; }
			c_suffix_token_kind token::get_c_suffix_token_kind() const { return this->suffix_token_kind; }
		}
	}
}