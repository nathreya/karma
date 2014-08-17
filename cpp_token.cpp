#include "stdafx.h"
#include "cpp_token.h"

using std::cout;
using std::string;
using std::fstream;
using std::vector;
using std::ostringstream;
using std::pair;

namespace __karma {
	namespace __preprocess {
		namespace __token {
			cpp_token::cpp_token(string f, int l, token t) {
				string s = t.get_qualifier();
				_file_name = f;
				_line_number = l;
				flag = t.wide();
				suffix_token_kind = t.get_c_suffix_token_kind();
				//runs through all the keywords
				if(s == _auto) {
					_qual = s;
					_id = token::AUTO;
				}
				else if(s == _restrict_) {
					_qual = s;
					_id = token::RESTRICT;
				}
				else if(s == _complex_) {
					_qual = s;
					_id = token::_COMPLEX;
				}
				else if(s == _imaginary) {
					_qual = s;
					_id = token::_IMAGINARY;
				}
				else if(s == _bool) {
					_qual = s;
					_id = token::_BOOL;
				}
				else if(s == _break) {
					_qual = s;
					_id = token::BREAK;
				}
				else if(s == _case) {
					_qual = s;
					_id = token::CASE;
				}
				else if(s == _char) {
					_qual = s;
					_id = token::CHAR;
				}
				else if(s == _const) {
					_qual = s;
					_id = token::_CONST;
				}
				else if(s == _continue) {
					_qual = s;
					_id = token::CONTINUE;
				}
				else if(s == _double) {
					_qual = s;
					_id = token::DOUBLE;
				}
				else if(s == _do) {
					_qual = s;
					_id = token::DO;
				}
				else if(s == _default) {
					_qual = s;
					_id = token::DEFAULT;
				}
				else if(s == _else) {
					_qual = s;
					_id = token::ELSE;
				}
				else if(s == _enum) {
					_qual = s;
					_id = token::ENUM;
				}
				else if(s == _extern) {
					_qual = s;
					_id = token::EXTERN;
				}
				else if(s == _float) {
					_qual = s;
					_id = token::FLOAT;
				}
				else if(s == _for) {
					_qual = s;
					_id = token::FOR;
				}
				else if(s == _goto) {
					_qual = s;
					_id = token::GOTO;
				}
				else if(s == _if) {
					_qual = s;
					_id = token::IF;
				}
				else if(s == _inline_) {
					_qual = s;
					_id = token::_INLINE_;
				}
				else if(s == _int) {
					_qual = s;
					_id = token::INT;
				}
				else if(s == _long) {
					_qual = s;
					_id = token::LONG;
				}
				else if(s == _register) {
					_qual = s;
					_id = token::REGISTER;
				}
				else if(s == _return) {
					_qual = s;
					_id = token::RETURN;
				}
				else if(s == _short) {
					_qual = s;
					_id = token::SHORT;
				}
				else if(s == _signed) {
					_qual = s;
					_id = token::SIGNED;
				}
				else if(s == _sizeof) {
					_qual = s;
					_id = token::SIZEOF;
				}
				else if(s == _static) {
					_qual = s;
					_id = token::STATIC;
				}
				else if(s == _struct) {
					_qual = s;
					_id = token::STRUCT;
				}
				else if(s == _switch) {
					_qual = s;
					_id = token::SWITCH;
				}
				else if(s == _typedef) {
					_qual = s;
					_id = token::TYPEDEF;
				}
				else if(s == _union) {
					_qual = s;
					_id = token::UNION;
				}
				else if(s == _unsigned) {
					_qual = s;
					_id = token::UNSIGNED;
				}
				else if(s == _void) {
					_qual = s;
					_id = token::_VOID;
				}
				else if(s == _volatile) {
					_qual = s;
					_id = token::VOLATILE;
				}
				else if(s == _while) {
					_qual = s;
					_id = token::WHILE;
				}

				else if(t.wide() == true) {

					if(s[0] == '\"') {
						_qual = s;
						_id = token::WQUOTE;
					}

					else {

						_qual = s;
						_id = token::WCHARACTER;
					}
				}

				//no need to re-write used code, just use the original token code to handle all other cases.
				else {
					_qual = token(s).get_qualifier();
					_id = token(s).get_id();
				}
			}

			cpp_token::~ cpp_token() {}
			string cpp_token::get_qualifier() { return _qual; }
			int cpp_token::get_id() { return _id; }
			int cpp_token::get_line_number() { return _line_number; }
			string cpp_token::get_file_name() { return _file_name; }
			bool cpp_token::wide() { return flag;  }
			c_suffix_token_kind cpp_token::get_c_suffix_token_kind() const { return this->suffix_token_kind; }
		}
	}
}
