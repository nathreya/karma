#ifndef TOKEN_H
#define TOKEN_H

#include "lex_utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include <locale>
#include <sstream>
#include <algorithm>

using std::cout;
using std::string;
using std::vector;
using std::istream;
using std::fstream;
using std::ostringstream;
using std::stringstream;
using std::istringstream;
using std::locale;
using std::transform;

using namespace __karma::__util;

namespace __karma {
	namespace __lex {
		namespace __token {

			static string const _bool = "_Bool";
			static string const _complex_ = "_Complex";
			static string const _imaginary = "_Imaginary";
			static string const _restrict_ = "restrict";
			static string const _e = "e";
			static string const _E = "E";
			static string const _p = "p";
			static string const _P = "P";
			static string const _0X = "0X";
			static string const _0x = "0x";
			static string const _break = "break";
			static string const _case = "case";
			static string const _const = "const";
			static string const _continue = "continue";
			static string const _default = "default";
			static string const _double = "double";
			static string const _enum = "enum";
			static string const _extern = "extern";
			static string const _goto = "goto";
			static string const _int = "int";
			static string const _long = "long";
			static string const _float = "float";
			static string const _char = "char";
			static string const _unsigned = "unsigned";
			static string const _signed = "signed";
			static string const _void = "void";
			static string const _short = "short";
			static string const _auto = "auto";
			static string const _register = "register";
			static string const _return = "return";
			static string const _sizeof = "sizeof";
			static string const _static = "static";
			static string const _struct = "struct";
			static string const _switch = "switch";
			static string const _typedef = "typedef";
			static string const _union = "union";
			static string const _volatile = "volatile";
			static string const _double_quote = "\"";
			static string const _escape_character = "\\";
			static string const _single_quote = "\'";
			static string const _plus = "+";
			static string const _minus = "-";
			static string const _star = "*";
			static string const _divide = "/";
			static string const _modulus = "%";
			static string const _single_and = "&";
			static string const _single_or = "|";
			static string const _xor = "^";
			static string const _cmpl = "~";
			static string const _not = "!";
			static string const _equals = "=";
			static string const _shift_left = "<<";
			static string const _shift_right = ">>";
			static string const _and = "&&";
			static string const _or = "||";
			static string const _less_than = "<";
			static string const _greater_than = ">";
			static string const _less_than_or_equal_to = "<=";
			static string const _greater_than_or_equal_to = ">=";
			static string const _equality = "==";
			static string const _inequality = "!=";
			static string const _hash = "#";
			static string const _hdefine = "#define";
			static string const _hif = "#if";
			static string const _helse = "#else";
			static string const _helif = "#elif";
			static string const _hifdef = "#ifdef";
			static string const _hifndef = "#ifndef";
			static string const _hpragma = "#pragma";
			static string const _hinclude = "#include";
			static string const _hline = "#line";
			static string const _hwarning = "#warning";
			static string const _hendif = "#endif";
			static string const _hundef = "#undef";
			static string const _herror = "#error";
			static string const _paste = "##";
			static string const _plus_equals = "+=";
			static string const _minus_equals = "-=";
			static string const _star_equals = "*=";
			static string const _divide_equals = "/=";
			static string const _modulus_equals = "%=";
			static string const _and_equals = "&=";
			static string const _or_equals = "|=";
			static string const _xor_equals = "^=";
			static string const _shift_left_equals = "<<=";
			static string const _shift_right_equals = ">>=";
			static string const _if = "if";
			static string const _else = "else";
			static string const _do = "do";
			static string const _while = "while";
			static string const _for = "for";
			static string const _open_parenthesis = "(";
			static string const _close_parenthesis = ")";
			static string const _open_brace = "{";
			static string const _close_brace = "}";
			static string const _open_bracket = "[";
			static string const _close_bracket = "]";
			static string const _semicolon = ";";
			static string const _single_line_comment = "//";
			static string const _open_multi_line_comment = "/*";
			static string const _close_multi_line_comment = "*/";
			static string const _space = " ";
			static string const _tab = "\t";
			static string const _new_line = "\n";
			static string const _comma = ",";
			static string const _dot = ".";
			static string const _colon = ":";
			static string const _question_mark = "?";
			static string const _variable_arguments = "...";
			static string const _general_increment = "++";
			static string const _general_decrement = "--";
			static string const _structure_dereference = "->";
			static string const _inline_ = "inline";
			static string const _suffixes = "lLuUFfDd";
			enum c_suffix_token_kind {
				SUFFIX_TOKEN_ULL = 0x0001, SUFFIX_TOKEN_LL = 0x0002, SUFFIX_TOKEN_L = 0x0004, SUFFIX_TOKEN_F = 0x0008, SUFFIX_TOKEN_D = 0x0010,
				SUFFIX_TOKEN_U = 0x0020, SUFFIX_TOKEN_NONE = 0x0040
			};

			class token {
				public:
					static int const DOUBLE = 256;
					static int const INT = 257;
					static int const LONG = 258;
					static int const FLOAT = 259;
					static int const CHAR = 260;
					static int const UNSIGNED = 261;
					static int const SIGNED = 262;
					static int const QUOTE = 263;
					static int const CHARACTER = 264;
					static int const PLUS = 265;
					static int const MINUS = 266;
					static int const STAR = 267;
					static int const DIVIDE = 268;
					static int const MODULUS = 269;
					static int const SINGLE_AND = 270;
					static int const SINGLE_OR = 271;
					static int const XOR = 272;
					static int const EQUALS = 273;
					static int const SHIFT_LEFT = 274;
					static int const SHIFT_RIGHT = 275;
					static int const AND = 276;
					static int const OR = 277;
					static int const LESS_THAN = 278;
					static int const GREATER_THAN = 279;
					static int const GREATER_THAN_OR_EQUAL_TO = 280;
					static int const LESS_THAN_OR_EQUAL_TO = 281;
					static int const EQUALITY = 282;
					static int const INEQUALITY = 283;
					static int const HASH = 284;
					static int const HDEFINE = 285;
					static int const HIF = 286;
					static int const HELSE = 287;
					static int const HELIF = 288;
					static int const HIFDEF = 289;
					static int const HIFNDEF = 290;
					static int const HPRAGMA = 291;
					static int const HINCLUDE = 292;
					static int const HLINE = 293;
					static int const HENDIF = 296;
					static int const PASTE = 297;
					static int const PLUS_EQUALS = 298;
					static int const MINUS_EQUALS = 299;
					static int const STAR_EQUALS = 300;
					static int const DIVIDE_EQUALS = 301;
					static int const MODULUS_EQUALS = 302;
					static int const IF = 303;
					static int const ELSE = 304;
					static int const DO = 305;
					static int const WHILE = 306;
					static int const FOR = 307;
					static int const OPEN_PARENTHESIS = 308;
					static int const CLOSE_PARENTHESIS = 309;
					static int const OPEN_BRACE = 310;
					static int const CLOSE_BRACE = 311;
					static int const OPEN_BRACKET = 312;
					static int const CLOSE_BRACKET = 313;
					static int const SEMICOLON = 314;
					static int const COMMA = 315;
					static int const DOT = 316;
					static int const SHORT = 317;
					static int const COLON = 318;
					static int const QUESTION_MARK = 319;
					static int const INTEGER = 320;
					static int const IDENTIFIER = 321;
					static int const ESCAPE_CHARACTER = 322;
					static int const AND_EQUALS = 323;
					static int const OR_EQUALS = 324;
					static int const XOR_EQUALS = 325;
					static int const CMPL = 326;
					static int const NOT = 327;
					static int const NEW_LINE = 328;
					static int const _VOID = 329;
					static int const AUTO = 330;
					static int const SHIFT_RIGHT_EQUALS = 331;
					static int const SHIFT_LEFT_EQUALS = 332;
					static int const CASE = 333;
					static int const _CONST = 334;
					static int const CONTINUE = 335;
					static int const DEFAULT = 336;
					static int const BREAK = 337;
					static int const ENUM = 338;
					static int const EXTERN = 339;
					static int const GOTO = 340;
					static int const REGISTER = 341;
					static int const RETURN = 342;
					static int const SIZEOF = 343;
					static int const STATIC = 344;
					static int const STRUCT = 345;
					static int const SWITCH = 346;
					static int const TYPEDEF = 347;
					static int const UNION = 348;
					static int const VOLATILE = 349;
					static int const HUNDEF = 350;
					static int const HERROR = 351;
					static int const VA_ARGS = 352;
					static int const FLOATING_POINT = 353;
					static int const WQUOTE = 354;
					static int const WCHARACTER = 355;
					static int const HWARNING = 356;
					static int const INCREMENT = 357;
					static int const DECREMENT = 358;
					static int const STRUCTURE_DEREFERENCE = 359;
					static int const _BOOL = 360;
					static int const _COMPLEX = 361;
					static int const RESTRICT = 362;
					static int const _INLINE_ = 363;
					static int const END_OF_FILE = 364;
					static int const _IMAGINARY = 365;

					token(string ident);
					~ token();
					string get_qualifier();
					int get_id();
					bool wide();
					c_suffix_token_kind get_c_suffix_token_kind() const;
				private:
					string qualifier;
					int id;
					bool flag;
					c_suffix_token_kind suffix_token_kind;
			};
		}
	}
}

#endif
